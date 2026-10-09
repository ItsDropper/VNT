#include "x86_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

typedef struct { char *name; int offset; int global; } HirVar;
typedef struct { char *value; int label; } HirString;
typedef struct { double value; int label; } HirFloat;
typedef struct {
    const VntIrProgram *ir;
    FILE *out;
    HirVar *vars; size_t var_count, var_cap;
    HirVar *globals; size_t global_count, global_cap;
    HirString *strings; size_t string_count, string_cap;
    HirFloat *floats; size_t float_count, float_cap;
    char **structs; size_t struct_count, struct_cap;
    int labels, loop_depth, loop_start[64], loop_end[64], error;
} HirGen;

static const VntIrNode *node(const HirGen *g, size_t i) {
    return i < g->ir->node_count ? &g->ir->nodes[i] : NULL;
}
static size_t child(const HirGen *g, size_t i, VntIrEdgeRole role) {
    const VntIrNode *n = node(g, i);
    if (!n) return VNT_IR_NO_NODE;
    for (size_t c=n->first_child; c!=VNT_IR_NO_NODE; c=g->ir->nodes[c].next_sibling)
        if (g->ir->nodes[c].role==role) return c;
    return VNT_IR_NO_NODE;
}
static size_t next_role(const HirGen *g, size_t i, VntIrEdgeRole role) {
    const VntIrNode *n = node(g, i);
    if (!n) return VNT_IR_NO_NODE;
    for (size_t c=n->next_sibling; c!=VNT_IR_NO_NODE; c=g->ir->nodes[c].next_sibling)
        if (g->ir->nodes[c].role==role) return c;
    return VNT_IR_NO_NODE;
}
static void fail(HirGen *g, const char *s) {
    if (!g->error) fprintf(stderr, "Native compiler error: %s\n", s);
    g->error=1;
}
static void cname(FILE *f, const char *prefix, const char *s) {
    fputs(prefix,f);
    for (const unsigned char *p=(const unsigned char*)s; *p; ++p)
        fputc(isalnum(*p)||*p=='_'?*p:'_',f);
}
static int label_new(HirGen *g) { return g->labels++; }
static void label_emit(HirGen *g,int l) { fprintf(g->out,".L%d:\n",l); }
static int add_name(HirVar **a,size_t *count,size_t *cap,const char *name) {
    for(size_t i=0;i<*count;i++) if(!strcmp((*a)[i].name,name)) return 1;
    if(*count==*cap) { size_t nc=*cap?*cap*2:16; HirVar *p=realloc(*a,nc*sizeof(*p)); if(!p)return 0;*a=p;*cap=nc; }
    (*a)[*count]=(HirVar){strdup(name),0,0};
    if(!(*a)[*count].name)return 0;
    ++*count; return 1;
}
static int var_index(HirVar *a,size_t count,const char *name) {
    for(size_t i=0;i<count;i++) if(!strcmp(a[i].name,name))return (int)i;
    return -1;
}
static int var_add(HirGen *g,const char *name) {
    if(!add_name(&g->vars,&g->var_count,&g->var_cap,name)){fail(g,"out of memory collecting HIR variables.");return 0;}
    return 1;
}
static int global_add(HirGen *g,const char *name) {
    if(!add_name(&g->globals,&g->global_count,&g->global_cap,name)){fail(g,"out of memory collecting HIR globals.");return 0;}
    return 1;
}
static void collect_vars(HirGen *g,size_t i) {
    const VntIrNode *n=node(g,i); if(!n||g->error)return;
    switch(n->opcode) {
    case VNT_IR_VARIABLE_DECL: case VNT_IR_REASSIGN:
        var_add(g,n->value.text);
        collect_vars(g,child(g,i,VNT_IR_EDGE_VALUE)); break;
    case VNT_IR_VARIABLE: var_add(g,n->value.text); break;
    case VNT_IR_ASSIGN:
        collect_vars(g,child(g,i,VNT_IR_EDGE_TARGET)); collect_vars(g,child(g,i,VNT_IR_EDGE_VALUE)); break;
    case VNT_IR_PRINT: case VNT_IR_RETURN: collect_vars(g,child(g,i,VNT_IR_EDGE_VALUE)); break;
    case VNT_IR_IF:
        collect_vars(g,child(g,i,VNT_IR_EDGE_CONDITION));
        for(size_t c=child(g,i,VNT_IR_EDGE_THEN);c!=VNT_IR_NO_NODE;c=next_role(g,c,VNT_IR_EDGE_THEN))collect_vars(g,c);
        for(size_t c=child(g,i,VNT_IR_EDGE_ELSE);c!=VNT_IR_NO_NODE;c=next_role(g,c,VNT_IR_EDGE_ELSE))collect_vars(g,c); break;
    case VNT_IR_WHILE:
        collect_vars(g,child(g,i,VNT_IR_EDGE_CONDITION));
        for(size_t c=child(g,i,VNT_IR_EDGE_BODY);c!=VNT_IR_NO_NODE;c=next_role(g,c,VNT_IR_EDGE_BODY))collect_vars(g,c); break;
    case VNT_IR_CALL: case VNT_IR_ARRAY:
        for(size_t c=n->first_child;c!=VNT_IR_NO_NODE;c=g->ir->nodes[c].next_sibling)collect_vars(g,c);break;
    case VNT_IR_INDEX: collect_vars(g,child(g,i,VNT_IR_EDGE_OBJECT));collect_vars(g,child(g,i,VNT_IR_EDGE_INDEX));break;
    case VNT_IR_MEMBER: collect_vars(g,child(g,i,VNT_IR_EDGE_OBJECT));break;
    case VNT_IR_BINARY: collect_vars(g,child(g,i,VNT_IR_EDGE_LEFT));collect_vars(g,child(g,i,VNT_IR_EDGE_RIGHT));break;
    case VNT_IR_UNARY: collect_vars(g,child(g,i,VNT_IR_EDGE_OPERAND));break;
    default: break;
    }
}
static int string_label(HirGen *g,const char *s) {
    for(size_t i=0;i<g->string_count;i++)if(!strcmp(g->strings[i].value,s))return g->strings[i].label;
    if(g->string_count==g->string_cap){size_t nc=g->string_cap?g->string_cap*2:8;HirString*p=realloc(g->strings,nc*sizeof(*p));if(!p){fail(g,"out of memory in string table.");return 0;}g->strings=p;g->string_cap=nc;}
    char *copy=strdup(s);if(!copy){fail(g,"out of memory in string table.");return 0;}
    int l=label_new(g);g->strings[g->string_count++]=(HirString){copy,l};return l;
}
static int float_label(HirGen *g,double d) {
    for(size_t i=0;i<g->float_count;i++)if(g->floats[i].value==d)return g->floats[i].label;
    if(g->float_count==g->float_cap){size_t nc=g->float_cap?g->float_cap*2:8;HirFloat*p=realloc(g->floats,nc*sizeof(*p));if(!p){fail(g,"out of memory in float table.");return 0;}g->floats=p;g->float_cap=nc;}
    int l=label_new(g);g->floats[g->float_count++]=(HirFloat){d,l};return l;
}
static void call0(HirGen *g,const char *name) { fputs("    subq $32,%rsp\n    call ",g->out);fputs(name,g->out);fputs("\n    addq $32,%rsp\n",g->out); }
static void emit_expr(HirGen *g,size_t i);
static void emit_stmt(HirGen *g,size_t i);
static void emit_stmt_role(HirGen *g,size_t i,VntIrEdgeRole role) {
    for(size_t c=child(g,i,role);c!=VNT_IR_NO_NODE&&!g->error;c=next_role(g,c,role))emit_stmt(g,c);
}
static void emit_user_call(HirGen *g,const VntIrNode*n,size_t i) {
    size_t args[32];int count=0;
    for(size_t c=n->first_child;c!=VNT_IR_NO_NODE;c=g->ir->nodes[c].next_sibling) {
        if(count==32){fail(g,"native functions support at most 32 arguments.");return;}
        args[count++]=c;
    }
    for(int a=0;a<count;a++){emit_expr(g,args[a]);fputs("    pushq %rax\n",g->out);}
    static const char *regs[]={"%rcx","%rdx","%r8","%r9"};
    if(count<=4){
        for(int a=0;a<count;a++)fprintf(g->out,"    movq %d(%%rsp),%s\n",(count-1-a)*8,regs[a]);
        if(count)fprintf(g->out,"    addq $%d,%%rsp\n",count*8);
        fputs("    subq $32,%rsp\n    call vnt_fn_",g->out);
        cname(g->out,"",n->value.text);fputs("\n    addq $32,%rsp\n",g->out);
    } else {
        int stack_count=count-4,area=32+stack_count*8;
        if(((count*8+area)&15)!=0)area+=8;
        fprintf(g->out,"    subq $%d,%%rsp\n",area);
        for(int a=0;a<4;a++)fprintf(g->out,"    movq %d(%%rsp),%s\n",area+(count-1-a)*8,regs[a]);
        for(int a=4;a<count;a++)fprintf(g->out,"    movq %d(%%rsp),%%r10\n    movq %%r10,%d(%%rsp)\n",area+(count-1-a)*8,32+(a-4)*8);
        fputs("    call vnt_fn_",g->out);cname(g->out,"",n->value.text);fputc('\n',g->out);
        fprintf(g->out,"    addq $%d,%%rsp\n",area+count*8);
    }
    (void)i;
}
static const char *builtin(const char *s,int *arity) {
    struct Entry { const char *name,*target; int arity; };
    static const struct Entry e[]={
      {"sqrt","vnt_sqrt",1},{"sin","vnt_sin",1},{"cos","vnt_cos",1},{"tan","vnt_tan",1},
      {"abs","vnt_abs",1},{"floor","vnt_floor",1},{"ceil","vnt_ceil",1},{"min","vnt_min",2},{"max","vnt_max",2},
      {"mod","vnt_mod",2},{"len","vnt_len",1},{"input","vnt_input",1},{"fs_exists","vnt_fs_exists",1},
      {"fs_read","vnt_fs_read",1},{"fs_write","vnt_fs_write",2},{"fs_append","vnt_fs_append",2},{"fs_delete","vnt_fs_delete",1},
      {"dir_create","vnt_dir_create",1},{"cwd","vnt_cwd",0},{"env_get","vnt_env_get",1},{"env_set","vnt_env_set",2},
      {"time_ms","vnt_time_ms",0},{"sleep_ms","vnt_sleep_ms",1},{"process_start","vnt_process_start",2},
      {"process_poll","vnt_process_poll",1},{"process_wait","vnt_process_wait",2},{"process_pid","vnt_process_pid",1},
      {"process_terminate","vnt_process_terminate",1},{"process_stdout","vnt_process_stdout",1},{"process_stderr","vnt_process_stderr",1},
      {"process_exit_code","vnt_process_exit_code",1},{"gui_css","vnt_gui_css",1},{"gui_button","vnt_gui_button",3},
      {"gui_button_sized","vnt_gui_button_sized",5},{"gui_textarea","vnt_gui_textarea",5},{"gui_input_set","vnt_gui_input_set",3},
      {"gui_panel_color","vnt_gui_panel_color",5},{"gui_text_style","vnt_gui_text_style",5},{"gui_input","vnt_gui_input",4},
      {"gui_checkbox","vnt_gui_checkbox",4},{"gui_progress","vnt_gui_progress",4},{"gui_separator","vnt_gui_separator",3},
      {"gui_present","vnt_gui_present",0},{"gui_open","vnt_gui_open",1},{"gui_size","vnt_gui_size",2},{"gui_text","vnt_gui_text",1},
      {"gui_text_at","vnt_gui_text_at",3},{"gui_title","vnt_gui_title",3},{"gui_panel","vnt_gui_panel",4},
      {"gui_fill","vnt_gui_fill",1},{"gui_rect","vnt_gui_rect",1},{"gui_poll","vnt_gui_poll",0},{"gui_key","vnt_gui_key",0},{"gui_close","vnt_gui_close",0}
    };
    for(size_t i=0;i<sizeof(e)/sizeof(e[0]);i++)if(!strcmp(s,e[i].name)){*arity=e[i].arity;return e[i].target;}
    return NULL;
}
static void emit_call(HirGen *g,size_t i,const VntIrNode*n) {
    const char *name=n->value.text;size_t args[32];int count=0;
    for(size_t c=n->first_child;c!=VNT_IR_NO_NODE;c=g->ir->nodes[c].next_sibling){if(count==32){fail(g,"too many call arguments.");return;}args[count++]=c;}
    if(!strcmp(name,"ffi_int")) {
        if(count<2||count>8){fail(g,"ffi_int() expects a library, symbol, and 0-6 integer arguments.");return;}
        for(int a=0;a<count;a++){emit_expr(g,args[a]);fputs("    pushq %rax\n",g->out);}
        fprintf(g->out,"    pushq $%d\n",count-2);
        int area=72;
        if((((count+1)*8+area)&15)!=0)area+=8;
        fprintf(g->out,"    subq $%d,%%rsp\n",area);
        fprintf(g->out,"    movq %d(%%rsp),%%rcx\n",area+count*8);
        fprintf(g->out,"    movq %d(%%rsp),%%rdx\n",area+(count-1)*8);
        for(int reg=2;reg<4;reg++){
            int arg_index=reg;
            if(arg_index>=count)fprintf(g->out,"    xorq %s,%s\n",reg==2?"%r8":"%r9",reg==2?"%r8":"%r9");
            else fprintf(g->out,"    movq %d(%%rsp),%s\n",area+8+(count-1-arg_index)*8,reg==2?"%r8":"%r9");
        }
        for(int a=4;a<8;a++){
            int dest=32+(a-4)*8;
            if(a>=count)fprintf(g->out,"    movq $0,%d(%%rsp)\n",dest);
            else fprintf(g->out,"    movq %d(%%rsp),%%r10\n    movq %%r10,%d(%%rsp)\n",area+8+(count-1-a)*8,dest);
        }
        fprintf(g->out,"    movq %d(%%rsp),%%r10\n    movq %%r10,64(%%rsp)\n",area);
        fputs("    call vnt_ffi_int\n",g->out);
        fprintf(g->out,"    addq $%d,%%rsp\n",area+((count+1)*8));
        return;
    }
    int arity=-1;const char *target=builtin(name,&arity);
    if(!strcmp(name,"object")){if(count){fail(g,"object() takes no arguments.");return;}call0(g,"vnt_object_new");return;}
    if(!strcmp(name,"range")) {
        if(count<1||count>3){fail(g,"range() expects 1-3 arguments.");return;}
        for(int a=0;a<count;a++){emit_expr(g,args[a]);fputs("    pushq %rax\n",g->out);}
        if(count==1){fputs("    popq %rcx\n    xorl %edx,%edx\n    xorl %r8d,%r8d\n    movl $1,%r9d\n",g->out);}
        else if(count==2){fputs("    popq %rdx\n    popq %rcx\n    xorl %r8d,%r8d\n    movl $2,%r9d\n",g->out);}
        else fputs("    popq %r8\n    popq %rdx\n    popq %rcx\n    movl $3,%r9d\n",g->out);
        call0(g,"vnt_range");return;
    }
    int is_struct=0;for(size_t a=0;a<g->struct_count;a++)if(!strcmp(g->structs[a],name))is_struct=1;
    if(is_struct){if(count){fail(g,"struct constructors take no arguments.");return;}fprintf(g->out,"    lea .Lstr%d(%%rip),%%rcx\n",string_label(g,name));call0(g,"vnt_struct_new");return;}
    if(!target){emit_user_call(g,n,i);return;}
    if(arity!=count){fail(g,"native builtin called with wrong argument count.");return;}
    if(count>32){fail(g,"native builtin supports at most 32 arguments.");return;}
    for(int a=0;a<count;a++){emit_expr(g,args[a]);fputs("    pushq %rax\n",g->out);}
    static const char *regs[]={"%rcx","%rdx","%r8","%r9"};
    if(count<=4){
        for(int a=0;a<count;a++)fprintf(g->out,"    movq %d(%%rsp),%s\n",(count-1-a)*8,regs[a]);
        if(count)fprintf(g->out,"    addq $%d,%%rsp\n",count*8);
        call0(g,target);
    } else {
        int stack_count=count-4,area=32+stack_count*8;
        if(((count*8+area)&15)!=0)area+=8;
        fprintf(g->out,"    subq $%d,%%rsp\n",area);
        for(int a=0;a<4;a++)fprintf(g->out,"    movq %d(%%rsp),%s\n",area+(count-1-a)*8,regs[a]);
        for(int a=4;a<count;a++)fprintf(g->out,"    movq %d(%%rsp),%%r10\n    movq %%r10,%d(%%rsp)\n",area+(count-1-a)*8,32+(a-4)*8);
        fprintf(g->out,"    call %s\n    addq $%d,%%rsp\n",target,area+count*8);
    }
}
static void emit_expr(HirGen *g,size_t i) {
    const VntIrNode*n=node(g,i);if(g->error)return;if(!n){fail(g,"missing HIR expression.");return;}
    switch(n->opcode){
    case VNT_IR_INTEGER:fprintf(g->out,"    movl $%d,%%ecx\n",n->value.integer);call0(g,"vnt_int");break;
    case VNT_IR_BOOLEAN:fprintf(g->out,"    movl $%d,%%ecx\n",!!n->value.boolean);call0(g,"vnt_bool");break;
    case VNT_IR_FLOAT:fprintf(g->out,"    movsd .Lflt%d(%%rip),%%xmm0\n",float_label(g,n->value.floating));call0(g,"vnt_float");break;
    case VNT_IR_STRING:fprintf(g->out,"    lea .Lstr%d(%%rip),%%rcx\n",string_label(g,n->value.text));call0(g,"vnt_string");break;
    case VNT_IR_VARIABLE:{
        int vi=var_index(g->vars,g->var_count,n->value.text),gi=var_index(g->globals,g->global_count,n->value.text);
        if(gi>=0&&(vi<0||g->vars[vi].global)){fputs("    movq vnt_global_",g->out);cname(g->out,"",n->value.text);fputs("(%rip),%rax\n",g->out);}
        else if(vi>=0)fprintf(g->out,"    movq -%d(%%rbp),%%rax\n",g->vars[vi].offset);
        else fail(g,"unknown HIR variable.");
        break;}
    case VNT_IR_ARRAY:
        call0(g,"vnt_array_new");
        for(size_t c=n->first_child;c!=VNT_IR_NO_NODE;c=g->ir->nodes[c].next_sibling){fputs("    pushq %rax\n",g->out);emit_expr(g,c);fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);call0(g,"vnt_array_push");}
        break;
    case VNT_IR_INDEX:{
        size_t a=child(g,i,VNT_IR_EDGE_OBJECT),b=child(g,i,VNT_IR_EDGE_INDEX);
        emit_expr(g,a);fputs("    pushq %rax\n",g->out);emit_expr(g,b);fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);call0(g,"vnt_array_get");break;}
    case VNT_IR_MEMBER:
        emit_expr(g,child(g,i,VNT_IR_EDGE_OBJECT));fputs("    pushq %rax\n",g->out);fprintf(g->out,"    lea .Lstr%d(%%rip),%%rdx\n",string_label(g,n->value.text));fputs("    popq %rcx\n",g->out);call0(g,"vnt_object_get");break;
    case VNT_IR_UNARY:{
        size_t a=child(g,i,VNT_IR_EDGE_OPERAND);
        if(n->operation==UNARY_REFERENCE){
            const VntIrNode*op=node(g,a);
            if(!op||op->opcode!=VNT_IR_VARIABLE){fail(g,"references require a variable.");break;}
            int vi=var_index(g->vars,g->var_count,op->value.text),gi=var_index(g->globals,g->global_count,op->value.text);
            if(gi>=0&&(vi<0||g->vars[vi].global)){fputs("    leaq vnt_global_",g->out);cname(g->out,"",op->value.text);fputs("(%rip),%rcx\n",g->out);}
            else if(vi>=0)fprintf(g->out,"    leaq -%d(%%rbp),%%rcx\n",g->vars[vi].offset);
            else {fail(g,"unknown reference variable.");break;}
            call0(g,"vnt_ref");
        } else if(n->operation==UNARY_DEREFERENCE){emit_expr(g,a);call0(g,"vnt_deref");}
        else {emit_expr(g,a);fputs("    movq %rax,%rcx\n",g->out);call0(g,n->operation==UNARY_NEGATE?"vnt_neg":"vnt_not");}
        break;}
    case VNT_IR_CALL:emit_call(g,i,n);break;
    case VNT_IR_BINARY:{
        size_t l=child(g,i,VNT_IR_EDGE_LEFT),r=child(g,i,VNT_IR_EDGE_RIGHT);
        if(n->operation==BINARY_AND||n->operation==BINARY_OR){
            int shortl=label_new(g),done=label_new(g);emit_expr(g,l);fputs("    movq %rax,%rcx\n",g->out);call0(g,"vnt_truth");
            fprintf(g->out,n->operation==BINARY_AND?"    testl %%eax,%%eax\n    jz .L%d\n":"    testl %%eax,%%eax\n    jnz .L%d\n",shortl);
            emit_expr(g,r);fputs("    movq %rax,%rcx\n",g->out);call0(g,"vnt_truth");fputs("    movl %eax,%ecx\n",g->out);call0(g,"vnt_bool");fprintf(g->out,"    jmp .L%d\n",done);
            label_emit(g,shortl);fprintf(g->out,"    movl $%d,%%ecx\n",n->operation==BINARY_AND?0:1);call0(g,"vnt_bool");label_emit(g,done);break;
        }
        emit_expr(g,l);fputs("    pushq %rax\n",g->out);emit_expr(g,r);fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);
        const char *fn=NULL;switch(n->operation){
        case BINARY_ADD:fn="vnt_add";break;case BINARY_SUBTRACT:fn="vnt_sub";break;case BINARY_MULTIPLY:fn="vnt_mul";break;
        case BINARY_DIVIDE:fn="vnt_div";break;case BINARY_MODULO:fn="vnt_mod";break;case BINARY_EQUAL:fn="vnt_eq";break;
        case BINARY_NOT_EQUAL:fn="vnt_ne";break;case BINARY_GREATER:fn="vnt_gt";break;case BINARY_LESS:fn="vnt_lt";break;
        case BINARY_GREATER_EQUAL:fn="vnt_ge";break;case BINARY_LESS_EQUAL:fn="vnt_le";break;default:break;}
        if(fn)call0(g,fn);else fail(g,"unsupported HIR binary operator.");break;}
    default:fail(g,"unsupported HIR expression opcode.");break;
    }
}
static void emit_assignment(HirGen*g,size_t i) {
    size_t t=child(g,i,VNT_IR_EDGE_TARGET),v=child(g,i,VNT_IR_EDGE_VALUE);const VntIrNode*tn=node(g,t);
    if(!tn){fail(g,"missing HIR assignment target.");return;}
    if(tn->opcode==VNT_IR_VARIABLE){
        int vi=var_index(g->vars,g->var_count,tn->value.text),gi=var_index(g->globals,g->global_count,tn->value.text);
        emit_expr(g,v);
        if(gi>=0&&(vi<0||g->vars[vi].global)){fputs("    movq %rax,vnt_global_",g->out);cname(g->out,"",tn->value.text);fputs("(%rip)\n",g->out);}
        else if(vi>=0)fprintf(g->out,"    movq %%rax,-%d(%%rbp)\n",g->vars[vi].offset);
        else fail(g,"unknown HIR assignment variable.");return;
    }
    if(tn->opcode==VNT_IR_INDEX){emit_expr(g,child(g,t,VNT_IR_EDGE_OBJECT));fputs("    pushq %rax\n",g->out);emit_expr(g,child(g,t,VNT_IR_EDGE_INDEX));fputs("    pushq %rax\n",g->out);emit_expr(g,v);fputs("    movq %rax,%r8\n    popq %rdx\n    popq %rcx\n",g->out);call0(g,"vnt_array_set");return;}
    if(tn->opcode==VNT_IR_MEMBER){emit_expr(g,child(g,t,VNT_IR_EDGE_OBJECT));fputs("    pushq %rax\n",g->out);emit_expr(g,v);fputs("    movq %rax,%r8\n",g->out);fprintf(g->out,"    lea .Lstr%d(%%rip),%%rdx\n",string_label(g,tn->value.text));fputs("    popq %rcx\n",g->out);call0(g,"vnt_object_set");return;}
    if(tn->opcode==VNT_IR_UNARY&&tn->operation==UNARY_DEREFERENCE){emit_expr(g,child(g,t,VNT_IR_EDGE_OPERAND));fputs("    pushq %rax\n",g->out);emit_expr(g,v);fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);call0(g,"vnt_ref_set");return;}
    fail(g,"unsupported HIR assignment target.");
}
static void emit_stmt(HirGen*g,size_t i) {
    const VntIrNode*n=node(g,i);if(!n||g->error)return;
    switch(n->opcode){
    case VNT_IR_PROGRAM:emit_stmt_role(g,i,VNT_IR_EDGE_STATEMENT);break;
    case VNT_IR_PRINT:emit_expr(g,child(g,i,VNT_IR_EDGE_VALUE));fputs("    movq %rax,%rcx\n",g->out);call0(g,"vnt_print");break;
    case VNT_IR_VARIABLE_DECL:case VNT_IR_REASSIGN:{
        int vi=var_index(g->vars,g->var_count,n->value.text),gi=var_index(g->globals,g->global_count,n->value.text);
        emit_expr(g,child(g,i,VNT_IR_EDGE_VALUE));
        if(gi>=0&&(vi<0||g->vars[vi].global)){fputs("    movq %rax,vnt_global_",g->out);cname(g->out,"",n->value.text);fputs("(%rip)\n",g->out);}
        else if(vi>=0)fprintf(g->out,"    movq %%rax,-%d(%%rbp)\n",g->vars[vi].offset);
        else fail(g,"unknown HIR declaration variable.");break;}
    case VNT_IR_ASSIGN:emit_assignment(g,i);break;
    case VNT_IR_IF:{
        int els=label_new(g),done=label_new(g);emit_expr(g,child(g,i,VNT_IR_EDGE_CONDITION));fputs("    movq %rax,%rcx\n",g->out);call0(g,"vnt_truth");fprintf(g->out,"    testl %%eax,%%eax\n    jz .L%d\n",els);
        emit_stmt_role(g,i,VNT_IR_EDGE_THEN);fprintf(g->out,"    jmp .L%d\n",done);label_emit(g,els);emit_stmt_role(g,i,VNT_IR_EDGE_ELSE);label_emit(g,done);break;}
    case VNT_IR_WHILE:{
        int s=label_new(g),e=label_new(g);if(g->loop_depth>=64){fail(g,"loop nesting too deep.");break;}
        g->loop_start[g->loop_depth]=s;g->loop_end[g->loop_depth]=e;g->loop_depth++;label_emit(g,s);
        emit_expr(g,child(g,i,VNT_IR_EDGE_CONDITION));fputs("    movq %rax,%rcx\n",g->out);call0(g,"vnt_truth");fprintf(g->out,"    testl %%eax,%%eax\n    jz .L%d\n",e);
        emit_stmt_role(g,i,VNT_IR_EDGE_BODY);fprintf(g->out,"    jmp .L%d\n",s);label_emit(g,e);g->loop_depth--;break;}
    case VNT_IR_BREAK:if(!g->loop_depth)fail(g,"break outside loop.");else fprintf(g->out,"    jmp .L%d\n",g->loop_end[g->loop_depth-1]);break;
    case VNT_IR_CONTINUE:if(!g->loop_depth)fail(g,"continue outside loop.");else fprintf(g->out,"    jmp .L%d\n",g->loop_start[g->loop_depth-1]);break;
    case VNT_IR_RETURN:{size_t v=child(g,i,VNT_IR_EDGE_VALUE);if(v!=VNT_IR_NO_NODE)emit_expr(g,v);else fputs("    xorl %eax,%eax\n",g->out);fputs("    leave\n    ret\n",g->out);break;}
    case VNT_IR_CALL:emit_expr(g,i);break;
    case VNT_IR_FUNCTION:case VNT_IR_STRUCT:break;
    default:fail(g,"unsupported HIR statement opcode.");break;
    }
}
static void free_vars(HirVar*a,size_t n){for(size_t i=0;i<n;i++)free(a[i].name);free(a);}
static void emit_function(HirGen*g,size_t i) {
    const VntIrNode*n=node(g,i);free_vars(g->vars,g->var_count);g->vars=NULL;g->var_count=g->var_cap=0;
    for(size_t c=child(g,i,VNT_IR_EDGE_BODY);c!=VNT_IR_NO_NODE;c=next_role(g,c,VNT_IR_EDGE_BODY))collect_vars(g,c);
    for(size_t p=0;p<n->name_count;p++)var_add(g,n->names[p]);
    for(size_t v=0;v<g->var_count;v++){int gi=var_index(g->globals,g->global_count,g->vars[v].name);if(gi>=0)g->vars[v].global=1;}
    for(size_t p=0;p<n->name_count;p++){int vi=var_index(g->vars,g->var_count,n->names[p]);if(vi>=0)g->vars[vi].global=0;}
    for(size_t v=0;v<g->var_count;v++)g->vars[v].offset=(int)(8+v*8);
    fprintf(g->out,".globl vnt_fn_");cname(g->out,"",n->value.text);fprintf(g->out,"\nvnt_fn_");cname(g->out,"",n->value.text);fputs(":\n    pushq %rbp\n    movq %rsp,%rbp\n",g->out);
    int frame=(int)(((g->var_count*8+15)/16)*16);if(frame)fprintf(g->out,"    subq $%d,%%rsp\n",frame);
    static const char*regs[]={"%rcx","%rdx","%r8","%r9"};
    for(size_t p=0;p<n->name_count&&p<4;p++){int vi=var_index(g->vars,g->var_count,n->names[p]);if(vi>=0)fprintf(g->out,"    movq %s,-%d(%%rbp)\n",regs[p],g->vars[vi].offset);}
    for(size_t p=4;p<n->name_count;p++){int vi=var_index(g->vars,g->var_count,n->names[p]);if(vi>=0)fprintf(g->out,"    movq %zu(%%rbp),%%r10\n    movq %%r10,-%d(%%rbp)\n",48+(p-4)*8,g->vars[vi].offset);}
    for(size_t c=child(g,i,VNT_IR_EDGE_BODY);c!=VNT_IR_NO_NODE&&!g->error;c=next_role(g,c,VNT_IR_EDGE_BODY))emit_stmt(g,c);
    if(!g->error)fputs("    xorl %eax,%eax\n    leave\n    ret\n",g->out);
}
static void emit_tables(HirGen*g) {
    if(g->float_count||g->string_count)fputs("\n.section .rdata\n",g->out);
    for(size_t i=0;i<g->float_count;i++)fprintf(g->out,".Lflt%d:\n    .double %.17g\n",g->floats[i].label,g->floats[i].value);
    for(size_t i=0;i<g->string_count;i++){fprintf(g->out,".Lstr%d:\n    .asciz \"",g->strings[i].label);for(const unsigned char*p=(const unsigned char*)g->strings[i].value;*p;p++){switch(*p){case '\\':fputs("\\\\",g->out);break;case '"':fputs("\\\"",g->out);break;case '\n':fputs("\\n",g->out);break;case '\r':fputs("\\r",g->out);break;case '\t':fputs("\\t",g->out);break;default:fputc(*p,g->out);}}fputs("\"\n",g->out);}
}
int vnt_emit_x86_64(const VntIrProgram *ir,const char *assembly_path) {
    if(!ir||!assembly_path||!vnt_ir_validate(ir)){fprintf(stderr,"Native compiler error: invalid HIR program.\n");return 0;}
    HirGen g={0};g.ir=ir;g.out=fopen(assembly_path,"wb");if(!g.out){fprintf(stderr,"Could not create assembly file: %s\n",assembly_path);return 0;}
    const VntIrNode*root=node(&g,ir->root);fputs(".text\n",g.out);
    for(size_t c=root->first_child;c!=VNT_IR_NO_NODE;c=g.ir->nodes[c].next_sibling){
        const VntIrNode*n=node(&g,c);
        if(n->opcode==VNT_IR_VARIABLE_DECL||n->opcode==VNT_IR_REASSIGN)global_add(&g,n->value.text);
        if(n->opcode==VNT_IR_STRUCT){if(g.struct_count==g.struct_cap){size_t nc=g.struct_cap?g.struct_cap*2:8;char**p=realloc(g.structs,nc*sizeof(*p));if(!p){fail(&g,"out of memory collecting structs.");break;}g.structs=p;g.struct_cap=nc;}g.structs[g.struct_count++]=strdup(n->value.text);}
    }
    if(g.global_count){fputs(".bss\n",g.out);for(size_t i=0;i<g.global_count;i++){fputs(".globl vnt_global_",g.out);cname(g.out,"",g.globals[i].name);fputs("\n.comm vnt_global_",g.out);cname(g.out,"",g.globals[i].name);fputs(",8,8\n",g.out);}fputs(".text\n",g.out);}
    for(size_t c=root->first_child;c!=VNT_IR_NO_NODE&&!g.error;c=g.ir->nodes[c].next_sibling)if(g.ir->nodes[c].opcode==VNT_IR_FUNCTION){emit_function(&g,c);fputc('\n',g.out);}
    if(!g.error){
        free_vars(g.vars,g.var_count);g.vars=NULL;g.var_count=g.var_cap=0;
        for(size_t c=root->first_child;c!=VNT_IR_NO_NODE;c=g.ir->nodes[c].next_sibling)if(g.ir->nodes[c].opcode!=VNT_IR_FUNCTION&&g.ir->nodes[c].opcode!=VNT_IR_STRUCT)collect_vars(&g,c);
        for(size_t v=0;v<g.var_count;v++){int gi=var_index(g.globals,g.global_count,g.vars[v].name);if(gi>=0)g.vars[v].global=1;g.vars[v].offset=(int)(8+v*8);}
        fputs(".globl main\nmain:\n    pushq %rbp\n    movq %rsp,%rbp\n",g.out);int frame=(int)(((g.var_count*8+15)/16)*16);if(frame)fprintf(g.out,"    subq $%d,%%rsp\n",frame);
        for(size_t c=root->first_child;c!=VNT_IR_NO_NODE&&!g.error;c=g.ir->nodes[c].next_sibling)if(g.ir->nodes[c].opcode!=VNT_IR_FUNCTION&&g.ir->nodes[c].opcode!=VNT_IR_STRUCT)emit_stmt(&g,c);
        if(!g.error)fputs("    xorl %eax,%eax\n    leave\n    ret\n",g.out);
    }
    if(!g.error){fputs(".Lvnt_int_add_overflow:\n    subq $32,%rsp\n    call vnt_int_add_overflow\n    addq $32,%rsp\n.Lvnt_int_sub_overflow:\n    subq $32,%rsp\n    call vnt_int_sub_overflow\n    addq $32,%rsp\n.Lvnt_int_mul_overflow:\n    subq $32,%rsp\n    call vnt_int_mul_overflow\n    addq $32,%rsp\n.Lvnt_int_neg_overflow:\n    subq $32,%rsp\n    call vnt_int_neg_overflow\n    addq $32,%rsp\n.Lvnt_int_div_zero:\n    subq $32,%rsp\n    call vnt_int_div_zero\n    addq $32,%rsp\n",g.out);emit_tables(&g);}
    fclose(g.out);
    free_vars(g.vars,g.var_count);free_vars(g.globals,g.global_count);
    for(size_t i=0;i<g.string_count;i++)free(g.strings[i].value);free(g.strings);
    free(g.floats);
    for(size_t i=0;i<g.struct_count;i++)free(g.structs[i]);free(g.structs);
    if(g.error){remove(assembly_path);return 0;}return 1;
}
