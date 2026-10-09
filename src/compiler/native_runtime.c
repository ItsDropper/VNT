#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

typedef enum {
    VNT_NULL,
    VNT_INT,
    VNT_FLOAT,
    VNT_BOOL,
    VNT_STRING,
    VNT_ARRAY,
    VNT_OBJECT,
    VNT_REFERENCE
} VntType;

typedef struct VntValue VntValue;

typedef struct {
    char *key;
    VntValue *value;
} VntField;

struct VntValue {
    VntType type;
    union {
        int integer;
        double floating;
        int boolean;
        char *string;
        struct {
            VntValue **items;
            int count;
            int capacity;
        } array;
        struct {
            VntField *fields;
            int count;
            int capacity;
            char *type_name;
        } object;
        struct {
            VntValue **slot;
        } reference;
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

VntValue *vnt_string_get(VntValue *s, VntValue *index);

static void require_number(VntValue *a, VntValue *b) {
    if (!a || !b ||
        (a->type != VNT_INT && a->type != VNT_FLOAT) ||
        (b->type != VNT_INT && b->type != VNT_FLOAT)) {
        fprintf(stderr, "Runtime error: arithmetic requires numbers.\n");
        exit(1);
    }
}

static double number_value(VntValue *v) {
    return v->type == VNT_FLOAT ? v->floating : (double)v->integer;
}

VntValue *vnt_null(void) { return alloc_value(VNT_NULL); }

VntValue *vnt_ref(VntValue **slot) {
    if (!slot) {
        fprintf(stderr, "Runtime error: cannot reference a null slot.\n");
        exit(1);
    }
    VntValue *v = alloc_value(VNT_REFERENCE);
    v->reference.slot = slot;
    return v;
}

VntValue *vnt_deref(VntValue *ref) {
    if (!ref || ref->type != VNT_REFERENCE || !ref->reference.slot) {
        fprintf(stderr, "Runtime error: dereference requires a reference.\n");
        exit(1);
    }
    return *ref->reference.slot;
}

VntValue *vnt_ref_set(VntValue *ref, VntValue *value) {
    if (!ref || ref->type != VNT_REFERENCE || !ref->reference.slot) {
        fprintf(stderr, "Runtime error: assignment requires a reference.\n");
        exit(1);
    }
    *ref->reference.slot = value;
    return value;
}

VntValue *vnt_int(int x) {
    VntValue *v = alloc_value(VNT_INT);
    v->integer = x;
    return v;
}

VntValue *vnt_float(double x) {
    VntValue *v = alloc_value(VNT_FLOAT);
    v->floating = x;
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
    if (a && a->type == VNT_STRING)
        return vnt_string_get(a, index);
    if (!a || a->type != VNT_ARRAY || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: indexing requires an array/string and integer index.\n");
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

VntValue *vnt_string_get(VntValue *s, VntValue *index) {
    if (!s || s->type != VNT_STRING || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: string indexing requires a string and integer index.\n");
        exit(1);
    }
    int i = index->integer;
    int len = (int)strlen(s->string);
    if (i < 0 || i >= len) {
        fprintf(stderr, "Runtime error: string index out of bounds.\n");
        exit(1);
    }
    char out[2] = { s->string[i], '\0' };
    return vnt_string(out);
}

static char *value_to_concat_string(VntValue *v) {
    char buffer[64];
    const char *text = NULL;
    if (!v) return strdup("null");
    switch (v->type) {
        case VNT_STRING: text = v->string; break;
        case VNT_INT: snprintf(buffer, sizeof(buffer), "%d", v->integer); text = buffer; break;
        case VNT_FLOAT: snprintf(buffer, sizeof(buffer), "%.15g", v->floating); text = buffer; break;
        case VNT_BOOL: text = v->boolean ? "true" : "false"; break;
        default: return NULL;
    }
    return strdup(text ? text : "");
}

VntValue *vnt_add(VntValue *a, VntValue *b) {
    if (a && b && (a->type == VNT_STRING || b->type == VNT_STRING)) {
        char *left = value_to_concat_string(a);
        char *right = value_to_concat_string(b);
        if (!left || !right) {
            free(left);
            free(right);
            fprintf(stderr, "Runtime error: string concatenation supports strings, numbers, and booleans.\n");
            exit(1);
        }
        size_t la = strlen(left), lb = strlen(right);
        char *joined = malloc(la + lb + 1);
        if (!joined) {
            free(left);
            free(right);
            fprintf(stderr, "Runtime error: out of memory.\n");
            exit(1);
        }
        memcpy(joined, left, la);
        memcpy(joined + la, right, lb + 1);
        VntValue *result = vnt_string(joined);
        free(joined);
        free(left);
        free(right);
        return result;
    }
    require_number(a, b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) + number_value(b));
    return vnt_int(a->integer + b->integer);
}
VntValue *vnt_sub(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) - number_value(b));
    return vnt_int(a->integer - b->integer);
}

VntValue *vnt_mul(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) * number_value(b));
    return vnt_int(a->integer * b->integer);
}

VntValue *vnt_div(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (number_value(b) == 0.0) {
        fprintf(stderr, "Runtime error: division by zero.\n");
        exit(1);
    }
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) / number_value(b));
    return vnt_int(a->integer / b->integer);
}

VntValue *vnt_mod(VntValue *a, VntValue *b) {
    if (!a || !b || a->type != VNT_INT || b->type != VNT_INT) {
        fprintf(stderr, "Runtime error: modulo requires integers.\n");
        exit(1);
    }
    if (!b->integer) {
        fprintf(stderr, "Runtime error: modulo by zero.\n");
        exit(1);
    }
    return vnt_int(a->integer % b->integer);
}

VntValue *vnt_neg(VntValue *a) {
    require_number(a, a);
    return a->type == VNT_FLOAT ? vnt_float(-a->floating) : vnt_int(-a->integer);
}

static int equal_value(VntValue *a, VntValue *b) {
    if (a->type != b->type) {
        if ((a->type == VNT_INT || a->type == VNT_FLOAT) &&
            (b->type == VNT_INT || b->type == VNT_FLOAT))
            return number_value(a) == number_value(b);
        return 0;
    }
    switch (a->type) {
        case VNT_NULL: return 1;
        case VNT_INT: return a->integer == b->integer;
        case VNT_FLOAT: return a->floating == b->floating;
        case VNT_BOOL: return a->boolean == b->boolean;
        case VNT_STRING: return strcmp(a->string, b->string) == 0;
        case VNT_ARRAY:
            if (a->array.count != b->array.count) return 0;
            for (int i=0;i<a->array.count;i++)
                if (!equal_value(a->array.items[i], b->array.items[i])) return 0;
            return 1;
        case VNT_OBJECT:
            return a == b;
    }
    return 0;
}

VntValue *vnt_eq(VntValue *a, VntValue *b) { return vnt_bool(equal_value(a,b)); }
VntValue *vnt_ne(VntValue *a, VntValue *b) { return vnt_bool(!equal_value(a,b)); }

static int order(VntValue *a, VntValue *b, int op) {
    require_number(a,b);
    double x=number_value(a), y=number_value(b);
    if (op == 0) return x > y;
    if (op == 1) return x < y;
    if (op == 2) return x >= y;
    return x <= y;
}

VntValue *vnt_gt(VntValue *a,VntValue *b){return vnt_bool(order(a,b,0));}
VntValue *vnt_lt(VntValue *a,VntValue *b){return vnt_bool(order(a,b,1));}
VntValue *vnt_ge(VntValue *a,VntValue *b){return vnt_bool(order(a,b,2));}
VntValue *vnt_le(VntValue *a,VntValue *b){return vnt_bool(order(a,b,3));}

VntValue *vnt_not(VntValue *a) {
    if (!a || a->type != VNT_BOOL) {
        fprintf(stderr, "Runtime error: '!' requires a boolean.\n");
        exit(1);
    }
    return vnt_bool(!a->boolean);
}

int vnt_truth(VntValue *a) {
    if (!a || a->type != VNT_BOOL) {
        fprintf(stderr, "Runtime error: condition must be boolean.\n");
        exit(1);
    }
    return a->boolean;
}

static void print_value(VntValue *v) {
    if (!v) { printf("null"); return; }
    switch (v->type) {
        case VNT_NULL: printf("null"); break;
        case VNT_INT: printf("%d", v->integer); break;
        case VNT_FLOAT: printf("%.15g", v->floating); break;
        case VNT_BOOL: printf("%s", v->boolean ? "true" : "false"); break;
        case VNT_STRING: printf("%s", v->string); break;
        case VNT_ARRAY:
            printf("[");
            for (int i=0;i<v->array.count;i++) {
                if (i) printf(", ");
                print_value(v->array.items[i]);
            }
            printf("]");
            break;
        case VNT_OBJECT: {
            if (v->object.type_name && v->object.type_name[0])
                printf("%s{", v->object.type_name);
            else
                printf("{");
            for (int i=0;i<v->object.count;i++) {
                if (i) printf(", ");
                printf("%s: ", v->object.fields[i].key);
                print_value(v->object.fields[i].value);
            }
            printf("}");
            break;
        }
        case VNT_REFERENCE: printf("<reference>"); break;
    }
}

void vnt_print(VntValue *v) {
    print_value(v);
    putchar('\n');
}

VntValue *vnt_len(VntValue *v) {
    if (v->type == VNT_STRING) return vnt_int((int)strlen(v->string));
    if (v->type == VNT_ARRAY) return vnt_int(v->array.count);
    if (v->type == VNT_OBJECT) return vnt_int(v->object.count);
    fprintf(stderr, "Runtime error: len() requires a string, array, or object.\n");
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

VntValue *vnt_object_new(void) {
    return alloc_value(VNT_OBJECT);
}

VntValue *vnt_struct_new(const char *name) {
    VntValue *v = alloc_value(VNT_OBJECT);
    v->object.type_name = strdup(name ? name : "");
    if (!v->object.type_name) {
        fprintf(stderr, "Runtime error: out of memory.\n");
        exit(1);
    }
    return v;
}

VntValue *vnt_object_get(VntValue *object, const char *key) {
    if (!object || object->type != VNT_OBJECT) {
        fprintf(stderr, "Runtime error: member access requires an object.\n");
        exit(1);
    }
    for (int i=0;i<object->object.count;i++)
        if (!strcmp(object->object.fields[i].key,key))
            return object->object.fields[i].value;
    return vnt_null();
}

VntValue *vnt_object_set(VntValue *object, const char *key, VntValue *value) {
    if (!object || object->type != VNT_OBJECT) {
        fprintf(stderr, "Runtime error: member assignment requires an object.\n");
        exit(1);
    }
    for (int i=0;i<object->object.count;i++) {
        if (!strcmp(object->object.fields[i].key,key)) {
            object->object.fields[i].value=value;
            return value;
        }
    }
    if (object->object.count == object->object.capacity) {
        int cap=object->object.capacity ? object->object.capacity*2 : 4;
        VntField *fields=realloc(object->object.fields,sizeof(*fields)*cap);
        if (!fields) { fprintf(stderr,"Runtime error: out of memory.\n"); exit(1); }
        object->object.fields=fields;
        object->object.capacity=cap;
    }
    object->object.fields[object->object.count].key=strdup(key);
    if (!object->object.fields[object->object.count].key) {
        fprintf(stderr,"Runtime error: out of memory.\n"); exit(1);
    }
    object->object.fields[object->object.count++].value=value;
    return value;
}

VntValue *vnt_sqrt(VntValue *x) { require_number(x,x); return vnt_float(sqrt(number_value(x))); }
VntValue *vnt_sin(VntValue *x) { require_number(x,x); return vnt_float(sin(number_value(x))); }
VntValue *vnt_cos(VntValue *x) { require_number(x,x); return vnt_float(cos(number_value(x))); }
VntValue *vnt_tan(VntValue *x) { require_number(x,x); return vnt_float(tan(number_value(x))); }
VntValue *vnt_abs(VntValue *x) { require_number(x,x); return x->type==VNT_FLOAT ? vnt_float(fabs(x->floating)) : vnt_int(abs(x->integer)); }
VntValue *vnt_floor(VntValue *x) { require_number(x,x); return vnt_float(floor(number_value(x))); }
VntValue *vnt_ceil(VntValue *x) { require_number(x,x); return vnt_float(ceil(number_value(x))); }
VntValue *vnt_min(VntValue *a,VntValue *b) { require_number(a,b); return number_value(a)<number_value(b)?a:b; }
VntValue *vnt_max(VntValue *a,VntValue *b) { require_number(a,b); return number_value(a)>number_value(b)?a:b; }
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


static intptr_t ffi_arg(VntValue *v) {
    if (!v || v->type != VNT_INT) {
        fprintf(stderr, "Runtime error: ffi_int() arguments must be integers.\n");
        exit(1);
    }
    return (intptr_t)v->integer;
}

static const char *ffi_string(VntValue *v, const char *what) {
    if (!v || v->type != VNT_STRING) {
        fprintf(stderr, "Runtime error: ffi_int() %s must be a string.\n", what);
        exit(1);
    }
    return v->string;
}

VntValue *vnt_ffi_int(
    VntValue *library,
    VntValue *symbol,
    VntValue *a0,
    VntValue *a1,
    VntValue *a2,
    VntValue *a3,
    VntValue *a4,
    VntValue *a5,
    int argc
) {
    const char *library_name = ffi_string(library, "library");
    const char *symbol_name = ffi_string(symbol, "symbol");
#ifdef _WIN32
    HMODULE module = LoadLibraryA(library_name);
    if (!module) {
        fprintf(stderr, "Runtime error: ffi_int() could not load '%s'.\n", library_name);
        exit(1);
    }
    FARPROC proc = GetProcAddress(module, symbol_name);
    if (!proc) {
        fprintf(stderr, "Runtime error: ffi_int() could not find '%s'.\n", symbol_name);
        exit(1);
    }
#else
    void *module = dlopen(library_name, RTLD_LAZY);
    if (!module) {
        fprintf(stderr, "Runtime error: ffi_int() could not load '%s'.\n", library_name);
        exit(1);
    }
    void *proc = dlsym(module, symbol_name);
    if (!proc) {
        fprintf(stderr, "Runtime error: ffi_int() could not find '%s'.\n", symbol_name);
        exit(1);
    }
#endif

    intptr_t args[6] = {0};
    VntValue *values[6] = {a0, a1, a2, a3, a4, a5};
    if (argc < 0 || argc > 6) {
        fprintf(stderr, "Runtime error: ffi_int() supports 0-6 arguments.\n");
        exit(1);
    }
    for (int i = 0; i < argc; i++) args[i] = ffi_arg(values[i]);

    intptr_t result = 0;
    switch (argc) {
        case 0: result = ((intptr_t (*)(void))proc)(); break;
        case 1: result = ((intptr_t (*)(intptr_t))proc)(args[0]); break;
        case 2: result = ((intptr_t (*)(intptr_t, intptr_t))proc)(args[0], args[1]); break;
        case 3: result = ((intptr_t (*)(intptr_t, intptr_t, intptr_t))proc)(args[0], args[1], args[2]); break;
        case 4: result = ((intptr_t (*)(intptr_t, intptr_t, intptr_t, intptr_t))proc)(args[0], args[1], args[2], args[3]); break;
        case 5: result = ((intptr_t (*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t))proc)(args[0], args[1], args[2], args[3], args[4]); break;
        case 6: result = ((intptr_t (*)(intptr_t, intptr_t, intptr_t, intptr_t, intptr_t, intptr_t))proc)(args[0], args[1], args[2], args[3], args[4], args[5]); break;
    }
    return vnt_int((int)result);
}


void vnt_int_div_zero(void) {
    fprintf(stderr, "Runtime error: division or modulo by zero.\n");
    exit(1);
}
