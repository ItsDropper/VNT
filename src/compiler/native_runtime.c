#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

typedef enum {
    VNT_INT,
    VNT_BOOL,
    VNT_STRING,
    VNT_ARRAY
} VntType;

typedef struct VntValue VntValue;

struct VntValue {
    VntType type;
    union {
        int integer;
        int boolean;
        char *string;
        struct {
            VntValue **items;
            int count;
            int capacity;
        } array;
    };
};

static VntValue *alloc_value(VntType type) {
    VntValue *v = calloc(1, sizeof(*v));
    if (!v) {
        fprintf(stderr, "Runtime error: out of memory.\n");
        exit(1);
    }
    v->type = type;
    return v;
}

VntValue *vnt_int(int x) {
    VntValue *v = alloc_value(VNT_INT);
    v->integer = x;
    return v;
}

VntValue *vnt_bool(int x) {
    VntValue *v = alloc_value(VNT_BOOL);
    v->boolean = x ? 1 : 0;
    return v;
}

VntValue *vnt_string(const char *s) {
    VntValue *v = alloc_value(VNT_STRING);
    v->string = strdup(s ? s : "");
    if (!v->string) {
        fprintf(stderr, "Runtime error: out of memory.\n");
        exit(1);
    }
    return v;
}

VntValue *vnt_array_new(void) {
    return alloc_value(VNT_ARRAY);
}

VntValue *vnt_array_push(VntValue *a, VntValue *item) {
    if (!a || a->type != VNT_ARRAY) {
        fprintf(stderr, "Runtime error: expected an array.\n");
        exit(1);
    }
    if (a->array.count == a->array.capacity) {
        int cap = a->array.capacity ? a->array.capacity * 2 : 4;
        VntValue **items = realloc(a->array.items, sizeof(*items) * cap);
        if (!items) {
            fprintf(stderr, "Runtime error: out of memory.\n");
            exit(1);
        }
        a->array.items = items;
        a->array.capacity = cap;
    }
    a->array.items[a->array.count++] = item;
    return a;
}

VntValue *vnt_array_get(VntValue *a, VntValue *index) {
    if (!a || a->type != VNT_ARRAY || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: array indexing requires an array and integer index.\n");
        exit(1);
    }
    int i = index->integer;
    if (i < 0 || i >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    return a->array.items[i];
}

VntValue *vnt_array_set(VntValue *a, VntValue *index, VntValue *value) {
    if (!a || a->type != VNT_ARRAY || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: array assignment requires an array and integer index.\n");
        exit(1);
    }
    int i = index->integer;
    if (i < 0 || i >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    a->array.items[i] = value;
    return value;
}

VntValue *vnt_add(VntValue *a, VntValue *b) {
    if (a->type == VNT_STRING && b->type == VNT_STRING) {
        size_t la = strlen(a->string), lb = strlen(b->string);
        char *s = malloc(la + lb + 1);
        if (!s) { fprintf(stderr, "Runtime error: out of memory.\n"); exit(1); }
        memcpy(s, a->string, la);
        memcpy(s + la, b->string, lb + 1);
        VntValue *v = vnt_string(s);
        free(s);
        return v;
    }
    if (a->type != VNT_INT || b->type != VNT_INT) {
        fprintf(stderr, "Runtime error: arithmetic requires integers.\n");
        exit(1);
    }
    return vnt_int(a->integer + b->integer);
}

static void require_int(VntValue *a, VntValue *b) {
    if (!a || !b || a->type != VNT_INT || b->type != VNT_INT) {
        fprintf(stderr, "Runtime error: arithmetic requires integers.\n");
        exit(1);
    }
}

VntValue *vnt_sub(VntValue *a, VntValue *b) { require_int(a,b); return vnt_int(a->integer-b->integer); }
VntValue *vnt_mul(VntValue *a, VntValue *b) { require_int(a,b); return vnt_int(a->integer*b->integer); }

VntValue *vnt_div(VntValue *a, VntValue *b) {
    require_int(a,b);
    if (!b->integer) { fprintf(stderr, "Runtime error: division by zero.\n"); exit(1); }
    return vnt_int(a->integer / b->integer);
}

VntValue *vnt_mod(VntValue *a, VntValue *b) {
    require_int(a,b);
    if (!b->integer) { fprintf(stderr, "Runtime error: modulo by zero.\n"); exit(1); }
    return vnt_int(a->integer % b->integer);
}

VntValue *vnt_neg(VntValue *a) {
    if (!a || a->type != VNT_INT) { fprintf(stderr, "Runtime error: unary '-' requires an integer.\n"); exit(1); }
    return vnt_int(-a->integer);
}

static int equal_value(VntValue *a, VntValue *b) {
    if (a->type != b->type) return 0;
    switch (a->type) {
        case VNT_INT: return a->integer == b->integer;
        case VNT_BOOL: return a->boolean == b->boolean;
        case VNT_STRING: return strcmp(a->string, b->string) == 0;
        case VNT_ARRAY:
            if (a->array.count != b->array.count) return 0;
            for (int i=0;i<a->array.count;i++) if (!equal_value(a->array.items[i], b->array.items[i])) return 0;
            return 1;
    }
    return 0;
}

VntValue *vnt_eq(VntValue *a, VntValue *b) { return vnt_bool(equal_value(a,b)); }
VntValue *vnt_ne(VntValue *a, VntValue *b) { return vnt_bool(!equal_value(a,b)); }

static int order(VntValue *a, VntValue *b, int op) {
    if (!a || !b || a->type != VNT_INT || b->type != VNT_INT) {
        fprintf(stderr, "Runtime error: ordered comparisons require integers.\n");
        exit(1);
    }
    if (op == 0) return a->integer > b->integer;
    if (op == 1) return a->integer < b->integer;
    if (op == 2) return a->integer >= b->integer;
    return a->integer <= b->integer;
}

VntValue *vnt_gt(VntValue *a,VntValue *b){return vnt_bool(order(a,b,0));}
VntValue *vnt_lt(VntValue *a,VntValue *b){return vnt_bool(order(a,b,1));}
VntValue *vnt_ge(VntValue *a,VntValue *b){return vnt_bool(order(a,b,2));}
VntValue *vnt_le(VntValue *a,VntValue *b){return vnt_bool(order(a,b,3));}

VntValue *vnt_not(VntValue *a) {
    if (!a || a->type != VNT_BOOL) { fprintf(stderr, "Runtime error: '!' requires a boolean.\n"); exit(1); }
    return vnt_bool(!a->boolean);
}

int vnt_truth(VntValue *a) {
    if (!a || a->type != VNT_BOOL) { fprintf(stderr, "Runtime error: condition must be boolean.\n"); exit(1); }
    return a->boolean;
}

void vnt_print(VntValue *v) {
    if (!v) { printf("null\n"); return; }
    switch (v->type) {
        case VNT_INT: printf("%d\n", v->integer); break;
        case VNT_BOOL: printf("%s\n", v->boolean ? "true" : "false"); break;
        case VNT_STRING: printf("%s\n", v->string); break;
        case VNT_ARRAY:
            printf("[");
            for (int i=0;i<v->array.count;i++) {
                if (i) printf(", ");
                VntValue *x=v->array.items[i];
                if (x->type==VNT_INT) printf("%d",x->integer);
                else if (x->type==VNT_BOOL) printf("%s",x->boolean?"true":"false");
                else if (x->type==VNT_STRING) printf("\"%s\"",x->string);
                else printf("[array]");
            }
            printf("]\n");
            break;
    }
}

VntValue *vnt_len(VntValue *v) {
    if (v->type == VNT_STRING) return vnt_int((int)strlen(v->string));
    if (v->type == VNT_ARRAY) return vnt_int(v->array.count);
    fprintf(stderr, "Runtime error: len() requires a string or array.\n");
    exit(1);
}

VntValue *vnt_input(VntValue *prompt) {
    if (!prompt || prompt->type != VNT_STRING) {
        fprintf(stderr, "Runtime error: input() requires a string prompt.\n");
        exit(1);
    }
    fputs(prompt->string, stdout);
    fflush(stdout);
    char buffer[4096];
    if (!fgets(buffer,sizeof(buffer),stdin)) {
        fprintf(stderr, "Runtime error: failed to read input.\n");
        exit(1);
    }
    buffer[strcspn(buffer,"\r\n")] = 0;
    return vnt_string(buffer);
}

VntValue *vnt_range(VntValue *a, VntValue *b, VntValue *c, int argc) {
    int start=0,end=0,step=1;
    if (argc==1) { if(a->type!=VNT_INT) goto bad; end=a->integer; }
    else if(argc==2) { if(a->type!=VNT_INT||b->type!=VNT_INT) goto bad; start=a->integer; end=b->integer; }
    else { if(a->type!=VNT_INT||b->type!=VNT_INT||c->type!=VNT_INT) goto bad; start=a->integer; end=b->integer; step=c->integer; }
    if (!step) { fprintf(stderr,"Runtime error: range() step cannot be zero.\n"); exit(1); }
    VntValue *r=vnt_array_new();
    if(step>0) for(int i=start;i<end;i+=step) vnt_array_push(r,vnt_int(i));
    else for(int i=start;i>end;i+=step) vnt_array_push(r,vnt_int(i));
    return r;
bad:
    fprintf(stderr,"Runtime error: range() requires integer arguments.\n"); exit(1);
}
