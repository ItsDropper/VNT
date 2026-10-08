#include "x86_backend.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct { char *name; int offset; } Var;
typedef struct { char *value; int label; } StringLit;
typedef struct { double value; int label; } FloatLit;

typedef struct {
    FILE *out;
    Var *vars;
    int var_count, var_capacity;
    int label_count;
    int loop_depth;
    int loop_start[64];
    int loop_end[64];
    int error;
    int temp_depth;
    StringLit *strings;
    int string_count, string_capacity;
    FloatLit *floats;
    int float_count, float_capacity;
} X86Gen;

static void fail(X86Gen *g, const char *msg) {
    if (!g->error) fprintf(stderr, "Native compiler error: %s\n", msg);
    g->error = 1;
}

static int new_label(X86Gen *g) { return g->label_count++; }

static void label(X86Gen *g, int n) { fprintf(g->out, ".L%d:\n", n); }

static void cname(FILE *out, const char *prefix, const char *name) {
    fputs(prefix, out);
    for (const unsigned char *p=(const unsigned char*)name; *p; ++p)
        fputc(isalnum(*p) || *p=='_' ? *p : '_', out);
}

static int var_find(X86Gen *g, const char *name) {
    for (int i=0;i<g->var_count;i++)
        if (!strcmp(g->vars[i].name,name)) return i;
    return -1;
}

static void var_add(X86Gen *g, const char *name) {
    if (g->error || var_find(g,name)>=0) return;
    if (g->var_count==g->var_capacity) {
        int cap=g->var_capacity?g->var_capacity*2:16;
        Var *v=realloc(g->vars,sizeof(*v)*cap);
        if(!v){fail(g,"out of memory.");return;}
        g->vars=v; g->var_capacity=cap;
    }
    g->vars[g->var_count].name=strdup(name);
    if(!g->vars[g->var_count].name){fail(g,"out of memory.");return;}
    g->vars[g->var_count++].offset=0;
}

static void free_vars(X86Gen *g) {
    for(int i=0;i<g->var_count;i++) free(g->vars[i].name);
    free(g->vars); g->vars=NULL; g->var_count=g->var_capacity=0;
}

static int var_offset(X86Gen *g,const char *name) {
    int i=var_find(g,name);
    if(i<0){fail(g,"unknown variable.");return 0;}
    return g->vars[i].offset;
}

static void collect_vars(X86Gen *g, AstNode *n) {
    for(;n;n=n->next) {
        switch(n->type) {
            case AST_VARIABLE_DECLARATION: var_add(g,n->variable_declaration.name); break;
            case AST_VARIABLE: var_add(g,n->variable.name); break;
            case AST_ASSIGNMENT:
                if(n->assignment.target && n->assignment.target->type==AST_VARIABLE)
                    var_add(g,n->assignment.target->variable.name);
                else if(n->assignment.target && n->assignment.target->type==AST_INDEX_EXPRESSION) {
                    collect_vars(g,n->assignment.target->index_expression.array);
                    collect_vars(g,n->assignment.target->index_expression.index);
                } else if(n->assignment.target && n->assignment.target->type==AST_MEMBER_EXPRESSION) {
                    collect_vars(g,n->assignment.target->member_expression.object);
                }
                collect_vars(g,n->assignment.value);
                break;
            case AST_PRINT_STATEMENT: collect_vars(g,n->print_statement.expression); break;
            case AST_IF_STATEMENT:
                collect_vars(g,n->if_statement.condition);
                collect_vars(g,n->if_statement.then_branch);
                collect_vars(g,n->if_statement.else_branch); break;
            case AST_WHILE_STATEMENT:
                collect_vars(g,n->while_statement.condition);
                collect_vars(g,n->while_statement.body); break;
            case AST_FUNCTION_CALL: collect_vars(g,n->function_call.arguments); break;
            case AST_ARRAY_LITERAL: collect_vars(g,n->array_literal.elements); break;
            case AST_INDEX_EXPRESSION:
                collect_vars(g,n->index_expression.array);
                collect_vars(g,n->index_expression.index); break;
            case AST_MEMBER_EXPRESSION:
                collect_vars(g,n->member_expression.object); break;
            case AST_BINARY_EXPRESSION:
                collect_vars(g,n->binary_expression.left);
                collect_vars(g,n->binary_expression.right); break;
            case AST_UNARY_EXPRESSION: collect_vars(g,n->unary_expression.operand); break;
            case AST_RETURN_STATEMENT: collect_vars(g,n->return_statement.expression); break;
            default: break;
        }
    }
}

static void assign_offsets(X86Gen *g) {
    for(int i=0;i<g->var_count;i++) g->vars[i].offset=8+i*8;
}

static void mem(X86Gen *g,int off) { fprintf(g->out,"-%d(%%rbp)",off); }

static void call0(X86Gen *g,const char *fn) {
    fputs("    subq $32, %rsp\n    call ",g->out);
    fputs(fn,g->out); fputc('\n',g->out);
    fputs("    addq $32, %rsp\n",g->out);
}

static void call1(X86Gen *g,const char *fn) {
    fputs("    movq %rax, %rcx\n",g->out);
    call0(g,fn);
}

static void call2_from_stack(X86Gen *g,const char *fn) {
    fputs("    movq %rax, %rdx\n    popq %rcx\n",g->out);
    call0(g,fn);
}

static int string_label(X86Gen *g, const char *value) {
    for (int i=0;i<g->string_count;i++)
        if (!strcmp(g->strings[i].value,value)) return g->strings[i].label;
    if (g->string_count==g->string_capacity) {
        int cap=g->string_capacity?g->string_capacity*2:8;
        StringLit *s=realloc(g->strings,sizeof(*s)*cap);
        if(!s){fail(g,"out of memory.");return 0;}
        g->strings=s;g->string_capacity=cap;
    }
    int label=new_label(g);
    g->strings[g->string_count].value=strdup(value);
    if(!g->strings[g->string_count].value){fail(g,"out of memory.");return 0;}
    g->strings[g->string_count].label=label;
    g->string_count++;
    return label;
}

static int float_label(X86Gen *g, double value) {
    for (int i=0;i<g->float_count;i++)
        if (g->floats[i].value == value) return g->floats[i].label;
    if (g->float_count == g->float_capacity) {
        int cap = g->float_capacity ? g->float_capacity * 2 : 8;
        FloatLit *f = realloc(g->floats, sizeof(*f) * cap);
        if (!f) { fail(g, "out of memory."); return 0; }
        g->floats = f;
        g->float_capacity = cap;
    }
    int l = new_label(g);
    g->floats[g->float_count].value = value;
    g->floats[g->float_count].label = l;
    g->float_count++;
    return l;
}

static void free_floats(X86Gen *g) {
    free(g->floats);
    g->floats = NULL;
    g->float_count = g->float_capacity = 0;
}

static void free_strings(X86Gen *g) {
    for(int i=0;i<g->string_count;i++) free(g->strings[i].value);
    free(g->strings);g->strings=NULL;g->string_count=g->string_capacity=0;
}

static void emit_expr(X86Gen *g,AstNode *n);

static void emit_call(X86Gen *g,AstNode *n) {
    const char *name=n->function_call.name;
    int count=n->function_call.argument_count;

    if(count>4){fail(g,"native functions currently support at most 4 arguments.");return;}

    if(!strcmp(name,"range")) {
        if(count<1||count>3){fail(g,"range() expects 1, 2, or 3 arguments.");return;}
        AstNode *a=n->function_call.arguments;
        for(int i=0;i<count;i++,a=a->next){emit_expr(g,a);fputs("    pushq %rax\n",g->out);g->temp_depth++;}
        if(count==1){
            fputs("    popq %rcx\n",g->out); g->temp_depth--;
            fputs("    xorl %edx,%edx\n    xorl %r8d,%r8d\n    movl $1,%r9d\n",g->out);
        } else if(count==2) {
            fputs("    popq %rdx\n    popq %rcx\n",g->out); g->temp_depth-=2;
            fputs("    xorl %r8d,%r8d\n    movl $2,%r9d\n",g->out);
        } else {
            fputs("    popq %r8\n    popq %rdx\n    popq %rcx\n    movl $3,%r9d\n",g->out); g->temp_depth-=3;
        }
        call0(g,"vnt_range");
        return;
    }

    if(!strcmp(name,"object")) {
        if(count!=0){fail(g,"object() expects no arguments.");return;}
        call0(g,"vnt_object_new");
        return;
    }

    if(!strcmp(name,"sqrt")||!strcmp(name,"sin")||!strcmp(name,"cos")||
       !strcmp(name,"tan")||!strcmp(name,"abs")||!strcmp(name,"floor")||
       !strcmp(name,"ceil")) {
        if(count!=1){fail(g,"math function expects 1 argument.");return;}
        emit_expr(g,n->function_call.arguments);
        fputs("    movq %rax,%rcx\\n",g->out);
        const char *fn=!strcmp(name,"sqrt")?"vnt_sqrt":
                      !strcmp(name,"sin")?"vnt_sin":
                      !strcmp(name,"cos")?"vnt_cos":
                      !strcmp(name,"tan")?"vnt_tan":
                      !strcmp(name,"abs")?"vnt_abs":
                      !strcmp(name,"floor")?"vnt_floor":"vnt_ceil";
        call0(g,fn);
        return;
    }

    if(!strcmp(name,"min")||!strcmp(name,"max")) {
        if(count!=2){fail(g,"min()/max() expect 2 arguments.");return;}
        AstNode *a=n->function_call.arguments;
        emit_expr(g,a);
        fputs("    pushq %rax\\n",g->out);
        emit_expr(g,a->next);
        fputs("    movq %rax,%rdx\\n    popq %rcx\\n",g->out);
        call0(g,!strcmp(name,"min")?"vnt_min":"vnt_max");
        return;
    }

    if(!strcmp(name,"mod")||!strcmp(name,"len")||!strcmp(name,"input")) {
        if(!strcmp(name,"mod") && count!=2){fail(g,"mod() expects 2 arguments.");return;}
        if(!strcmp(name,"len") && count!=1){fail(g,"len() expects 1 argument.");return;}
        if(!strcmp(name,"input") && count!=1){fail(g,"input() expects 1 argument.");return;}
        AstNode *a=n->function_call.arguments;
        if(count==1) {
            emit_expr(g,a);
            fputs("    movq %rax,%rcx\n",g->out);
        } else {
            emit_expr(g,a);
            fputs("    pushq %rax\n",g->out);
            emit_expr(g,a->next);
            fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);
        }
        call0(g,!strcmp(name,"mod")?"vnt_mod":(!strcmp(name,"len")?"vnt_len":"vnt_input"));
        return;
    }

    int i=0;
    for(AstNode *a=n->function_call.arguments;a;a=a->next,i++){
        emit_expr(g,a);
        fputs("    pushq %rax\n",g->out);
        g->temp_depth++;
    }

    static const char *regs[]={"%rcx","%rdx","%r8","%r9"};
    for(i=count-1;i>=0;i--)
        fprintf(g->out,"    movq %d(%%rsp), %s\n",(count-1-i)*8,regs[i]);

    int pad=(g->temp_depth&1)?8:0;
    if(pad) fputs("    subq $8,%rsp\n",g->out);
    fputs("    subq $32,%rsp\n    call vnt_fn_",g->out);
    cname(g->out,"",name); fputc('\n',g->out);
    fputs("    addq $32,%rsp\n",g->out);
    if(pad) fputs("    addq $8,%rsp\n",g->out);
    for(i=0;i<count;i++){fputs("    popq %r10\n",g->out);g->temp_depth--;}
}

static void emit_expr(X86Gen *g,AstNode *n) {
    if(g->error)return;
    if(!n){fail(g,"missing expression.");return;}

    switch(n->type) {
        case AST_INTEGER_LITERAL:
            fprintf(g->out,"    movl $%d,%%ecx\n",n->integer_literal.value);
            call0(g,"vnt_int");
            break;
        case AST_FLOAT_LITERAL: {
            int l=float_label(g,n->float_literal.value);
            fprintf(g->out,"    movsd .Lflt%d(%%rip),%%xmm0\n",l);
            call0(g,"vnt_float");
            break;
        }

        case AST_BOOLEAN_LITERAL:
            fprintf(g->out,"    movl $%d,%%ecx\n",n->boolean_literal.value?1:0);
            call0(g,"vnt_bool");
            break;
        case AST_STRING_LITERAL:
            fprintf(g->out,"    lea .Lstr%d(%%rip),%%rcx\n",string_label(g,n->string_literal.value));
            call0(g,"vnt_string");
            break;
        case AST_VARIABLE:
            fprintf(g->out,"    movq "); mem(g,var_offset(g,n->variable.name)); fputs(",%rax\n",g->out);
            break;
        case AST_ARRAY_LITERAL: {
            call0(g,"vnt_array_new");
            for(AstNode *e=n->array_literal.elements;e;e=e->next){
                fputs("    pushq %rax\n",g->out);
                emit_expr(g,e);
                fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);
                call0(g,"vnt_array_push");
            }
            break;
        }
        case AST_INDEX_EXPRESSION:
            emit_expr(g,n->index_expression.array);
            fputs("    pushq %rax\n",g->out);
            emit_expr(g,n->index_expression.index);
            fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);
            call0(g,"vnt_array_get");
            break;
        case AST_MEMBER_EXPRESSION:
            emit_expr(g,n->member_expression.object);
            fputs("    pushq %rax\n",g->out);
            fprintf(g->out,"    lea .Lstr%d(%%rip),%%rdx\n",string_label(g,n->member_expression.member));
            fputs("    popq %rcx\n",g->out);
            call0(g,"vnt_object_get");
            break;
        case AST_UNARY_EXPRESSION:
            emit_expr(g,n->unary_expression.operand);
            call1(g,n->unary_expression.operator==UNARY_NEGATE?"vnt_neg":"vnt_not");
            break;
        case AST_FUNCTION_CALL:
            emit_call(g,n); break;
        case AST_BINARY_EXPRESSION: {
            BinaryOperator op=n->binary_expression.operator;
            if(op==BINARY_AND||op==BINARY_OR){
                int short_l=new_label(g),done=new_label(g);
                emit_expr(g,n->binary_expression.left);
                call1(g,"vnt_truth");
                if(op==BINARY_AND) fprintf(g->out,"    testl %%eax,%%eax\n    jz .L%d\n",short_l);
                else fprintf(g->out,"    testl %%eax,%%eax\n    jnz .L%d\n",short_l);
                emit_expr(g,n->binary_expression.right);
                call1(g,"vnt_truth");
                fputs("    movl %eax,%ecx\n    movq %rcx,%rax\n    movq %rax,%rcx\n",g->out);
                call0(g,"vnt_bool");
                fprintf(g->out,"    jmp .L%d\n",done);
                label(g,short_l);
                fprintf(g->out,"    movl $%d,%%eax\n    movq %%rax,%%rcx\n",op==BINARY_AND?0:1);
                call0(g,"vnt_bool");
                label(g,done);
                break;
            }
            emit_expr(g,n->binary_expression.left);
            fputs("    pushq %rax\n",g->out);
            emit_expr(g,n->binary_expression.right);
            fputs("    movq %rax,%rdx\n    popq %rcx\n",g->out);
            const char *fn=NULL;
            switch(op){
                case BINARY_ADD:fn="vnt_add";break; case BINARY_SUBTRACT:fn="vnt_sub";break;
                case BINARY_MULTIPLY:fn="vnt_mul";break; case BINARY_DIVIDE:fn="vnt_div";break;
                case BINARY_MODULO:fn="vnt_mod";break; case BINARY_EQUAL:fn="vnt_eq";break;
                case BINARY_NOT_EQUAL:fn="vnt_ne";break; case BINARY_GREATER:fn="vnt_gt";break;
                case BINARY_LESS:fn="vnt_lt";break; case BINARY_GREATER_EQUAL:fn="vnt_ge";break;
                case BINARY_LESS_EQUAL:fn="vnt_le";break; default:break;
            }
            if(fn) call0(g,fn); else fail(g,"unsupported binary operator.");
            break;
        }
        default: fail(g,"unsupported native expression."); break;
    }
}

static void emit_print(X86Gen *g,AstNode *e){
    emit_expr(g,e);
    fputs("    movq %rax,%rcx\n",g->out);
    call0(g,"vnt_print");
}

static void emit_assignment(X86Gen *g,AstNode *n){
    AstNode *t=n->assignment.target;
    if(t->type==AST_VARIABLE){
        emit_expr(g,n->assignment.value);
        fprintf(g->out,"    movq %%rax,");mem(g,var_offset(g,t->variable.name));fputc('\n',g->out);
        return;
    }
    if(t->type==AST_INDEX_EXPRESSION){
        emit_expr(g,t->index_expression.array);
        fputs("    pushq %rax\n",g->out);
        emit_expr(g,t->index_expression.index);
        fputs("    pushq %rax\n",g->out);
        emit_expr(g,n->assignment.value);
        fputs("    movq %rax,%r8\n    popq %rdx\n    popq %rcx\n    movq %r8,%r8\n",g->out);
        fputs("    subq $32,%rsp\n    call vnt_array_set\n    addq $32,%rsp\n",g->out);
        return;
    }
    if(t->type==AST_MEMBER_EXPRESSION){
        emit_expr(g,t->member_expression.object);
        fputs("    pushq %rax\n",g->out);
        emit_expr(g,n->assignment.value);
        fputs("    movq %rax,%r8\n",g->out);
        fprintf(g->out,"    lea .Lstr%d(%%rip),%%rdx\n",string_label(g,t->member_expression.member));
        fputs("    popq %rcx\n",g->out);
        fputs("    subq $32,%rsp\n    call vnt_object_set\n    addq $32,%rsp\n",g->out);
        return;
    }
    fail(g,"invalid assignment target.");
}

static void emit_stmt_list(X86Gen *g,AstNode *n);

static void emit_stmt(X86Gen *g,AstNode *n){
    if(g->error)return;
    switch(n->type){
        case AST_PRINT_STATEMENT:emit_print(g,n->print_statement.expression);break;
        case AST_VARIABLE_DECLARATION:
            emit_expr(g,n->variable_declaration.value);
            fprintf(g->out,"    movq %%rax,");mem(g,var_offset(g,n->variable_declaration.name));fputc('\n',g->out);break;
        case AST_ASSIGNMENT:emit_assignment(g,n);break;
        case AST_IF_STATEMENT:{
            int els=new_label(g),done=new_label(g);
            emit_expr(g,n->if_statement.condition);call1(g,"vnt_truth");
            fprintf(g->out,"    testl %%eax,%%eax\n    jz .L%d\n",els);
            emit_stmt_list(g,n->if_statement.then_branch);
            fprintf(g->out,"    jmp .L%d\n",done);label(g,els);
            emit_stmt_list(g,n->if_statement.else_branch);label(g,done);break;
        }
        case AST_WHILE_STATEMENT:{
            int s=new_label(g),e=new_label(g);
            if(g->loop_depth>=64){fail(g,"loop nesting is too deep.");return;}
            g->loop_start[g->loop_depth]=s;g->loop_end[g->loop_depth]=e;g->loop_depth++;
            label(g,s);emit_expr(g,n->while_statement.condition);call1(g,"vnt_truth");
            fprintf(g->out,"    testl %%eax,%%eax\n    jz .L%d\n",e);
            emit_stmt_list(g,n->while_statement.body);fprintf(g->out,"    jmp .L%d\n",s);label(g,e);
            g->loop_depth--;break;
        }
        case AST_BREAK_STATEMENT:
            if(!g->loop_depth){fail(g,"break outside a loop.");return;}
            fprintf(g->out,"    jmp .L%d\n",g->loop_end[g->loop_depth-1]);break;
        case AST_CONTINUE_STATEMENT:
            if(!g->loop_depth){fail(g,"continue outside a loop.");return;}
            fprintf(g->out,"    jmp .L%d\n",g->loop_start[g->loop_depth-1]);break;
        case AST_RETURN_STATEMENT:
            if(n->return_statement.expression)emit_expr(g,n->return_statement.expression);
            else fputs("    xorl %eax,%eax\n",g->out);
            fputs("    leave\n    ret\n",g->out);break;
        case AST_FUNCTION_CALL:emit_expr(g,n);break;
        case AST_FUNCTION_DECLARATION:break;
        default:fail(g,"unsupported native statement.");break;
    }
}

static void emit_stmt_list(X86Gen *g,AstNode *n){for(;n&&!g->error;n=n->next)emit_stmt(g,n);}

static void emit_function(X86Gen *g,AstNode *fn){
    int count=fn->function_declaration.parameter_count;
    if(count>4){fail(g,"native functions currently support at most 4 parameters.");return;}
    free_vars(g);collect_vars(g,fn->function_declaration.body);
    for(int i=0;i<count;i++)var_add(g,fn->function_declaration.parameters[i]);
    assign_offsets(g);
    fputs(".globl vnt_fn_",g->out);cname(g->out,"",fn->function_declaration.name);fputc('\n',g->out);
    fputs("vnt_fn_",g->out);cname(g->out,"",fn->function_declaration.name);fputs(":\n    pushq %rbp\n    movq %rsp,%rbp\n",g->out);
    int frame=((g->var_count*8+15)/16)*16;if(frame)fprintf(g->out,"    subq $%d,%%rsp\n",frame);
    static const char *regs[]={"%rcx","%rdx","%r8","%r9"};
    for(int i=0;i<count;i++){fprintf(g->out,"    movq %s,",regs[i]);mem(g,var_offset(g,fn->function_declaration.parameters[i]));fputc('\n',g->out);}
    emit_stmt_list(g,fn->function_declaration.body);
    if(!g->error)fputs("    xorl %eax,%eax\n    leave\n    ret\n",g->out);
}

static void emit_float_table(X86Gen *g) {
    for(int i=0;i<g->float_count;i++)
        fprintf(g->out,".Lflt%d:\n    .double %.17g\n",g->floats[i].label,g->floats[i].value);
}

static void emit_string_table(X86Gen *g) {
    for(int i=0;i<g->string_count;i++) {
        fprintf(g->out,".Lstr%d:\n    .asciz ",g->strings[i].label);
        fputc('"',g->out);
        for(const unsigned char *p=(const unsigned char*)g->strings[i].value;*p;p++) {
            switch(*p) {
                case '\\': fputs("\\\\",g->out); break;
                case '"': fputs("\\\"",g->out); break;
                case '\n': fputs("\\n",g->out); break;
                case '\r': fputs("\\r",g->out); break;
                case '\t': fputs("\\t",g->out); break;
                default: fputc(*p,g->out); break;
            }
        }
        fputc('"',g->out);
        fputc('\n',g->out);
    }
}

int vnt_emit_x86_64(AstNode *program,const char *assembly_path){
    if(!program||program->type!=AST_PROGRAM){fprintf(stderr,"Native compiler error: invalid program AST.\n");return 0;}
    X86Gen g={0};g.out=fopen(assembly_path,"wb");
    if(!g.out){fprintf(stderr,"Could not create assembly file: %s\n",assembly_path);return 0;}
    fputs(".text\n",g.out);
    for(AstNode *n=program->program.statements;n;n=n->next)
        if(n->type==AST_FUNCTION_DECLARATION){emit_function(&g,n);if(g.error)break;fputc('\n',g.out);}
    if(!g.error){
        free_vars(&g);collect_vars(&g,program->program.statements);assign_offsets(&g);
        fputs(".globl main\nmain:\n    pushq %rbp\n    movq %rsp,%rbp\n",g.out);
        int frame=((g.var_count*8+15)/16)*16;if(frame)fprintf(g.out,"    subq $%d,%%rsp\n",frame);
        emit_stmt_list(&g,program->program.statements);
        if(!g.error)fputs("    xorl %eax,%eax\n    leave\n    ret\n",g.out);
    }
    if(!g.error){
        if(g.string_count || g.float_count){
            fputs("\n.section .rdata\n",g.out);
            emit_float_table(&g);
            emit_string_table(&g);
        }
    }
    fclose(g.out);free_vars(&g);free_strings(&g);free_floats(&g);
    if(g.error){remove(assembly_path);return 0;}return 1;
}
