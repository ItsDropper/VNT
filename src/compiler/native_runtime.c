#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#endif
#ifdef _WIN32
#include <direct.h>
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

/*
 * Values currently live for the lifetime of a compiled program. Stable slabs
 * avoid one heap allocation per integer, boolean, and arithmetic result.
 */
#define VNT_VALUE_SLAB_CAPACITY 1024
typedef struct VntValueSlab {
    struct VntValueSlab *next;
    size_t used;
    VntValue values[VNT_VALUE_SLAB_CAPACITY];
} VntValueSlab;

static VntValueSlab *value_slabs;

static VntValue *alloc_value(VntType type) {
    VntValueSlab *slab = value_slabs;
    if (!slab || slab->used == VNT_VALUE_SLAB_CAPACITY) {
        VntValueSlab *next = calloc(1, sizeof(*next));
        if (!next) {
            fprintf(stderr, "Runtime error: out of memory.\n");
            exit(1);
        }
        next->next = value_slabs;
        value_slabs = next;
        slab = next;
    }
    VntValue *v = &slab->values[slab->used++];
    /* Slabs are calloc-initialized and each slot is used only once. */
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

/* Error exits used by generated native integer fast paths on overflow. */
void vnt_int_add_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in addition.\n");
    exit(1);
}
void vnt_int_sub_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in subtraction.\n");
    exit(1);
}
void vnt_int_mul_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in multiplication.\n");
    exit(1);
}
void vnt_int_neg_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in unary '-'.\n");
    exit(1);
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

VntValue *vnt_array_get_int(VntValue *a, int index) {
    if (!a || a->type != VNT_ARRAY) {
        fprintf(stderr, "Runtime error: indexing requires an array.\n");
        exit(1);
    }
    if (index < 0 || index >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    return a->array.items[index];
}

VntValue *vnt_array_set_int(VntValue *a, int index, VntValue *value) {
    if (!a || a->type != VNT_ARRAY) {
        fprintf(stderr, "Runtime error: array assignment requires an array.\n");
        exit(1);
    }
    if (index < 0 || index >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    a->array.items[index] = value;
    return value;
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
    int64_t result = (int64_t)a->integer + (int64_t)b->integer;
    if (result < INT_MIN || result > INT_MAX) {
        fprintf(stderr, "Runtime error: integer overflow in addition.\n");
        exit(1);
    }
    return vnt_int((int)result);
}
VntValue *vnt_sub(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) - number_value(b));
    int64_t result = (int64_t)a->integer - (int64_t)b->integer;
    if (result < INT_MIN || result > INT_MAX) {
        fprintf(stderr, "Runtime error: integer overflow in subtraction.\n");
        exit(1);
    }
    return vnt_int((int)result);
}

VntValue *vnt_mul(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) * number_value(b));
    int64_t result = (int64_t)a->integer * (int64_t)b->integer;
    if (result < INT_MIN || result > INT_MAX) {
        fprintf(stderr, "Runtime error: integer overflow in multiplication.\n");
        exit(1);
    }
    return vnt_int((int)result);
}

VntValue *vnt_div(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (number_value(b) == 0.0) {
        fprintf(stderr, "Runtime error: division by zero.\n");
        exit(1);
    }
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) / number_value(b));
    if (a->integer == INT_MIN && b->integer == -1) {
        fprintf(stderr, "Runtime error: integer overflow in division.\n");
        exit(1);
    }
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
    if (a->integer == INT_MIN && b->integer == -1) return vnt_int(0);
    return vnt_int(a->integer % b->integer);
}

VntValue *vnt_neg(VntValue *a) {
    require_number(a, a);
    if (a->type == VNT_FLOAT) return vnt_float(-a->floating);
    if (a->integer == INT_MIN) {
        fprintf(stderr, "Runtime error: integer overflow in negation.\n");
        exit(1);
    }
    return vnt_int(-a->integer);
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


/* Native application API: portable file, environment, and timing calls. */
static const char *vnt_app_string(VntValue *v, const char *fn) {
    if (!v || v->type != VNT_STRING || !v->string) {
        fprintf(stderr, "Runtime error: %s expects a string argument.\n", fn);
        exit(1);
    }
    return v->string;
}
VntValue *vnt_fs_exists(VntValue *path) {
    const char *p = vnt_app_string(path, "fs_exists()");
#ifdef _WIN32
    return vnt_bool(GetFileAttributesA(p) != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st; return vnt_bool(stat(p, &st) == 0);
#endif
}
VntValue *vnt_fs_read(VntValue *path) {
    const char *p = vnt_app_string(path, "fs_read()");
    FILE *f = fopen(p, "rb");
    if (!f) { fprintf(stderr, "Runtime error: fs_read() could not open '%s'.\n", p); exit(1); }
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); fprintf(stderr, "Runtime error: fs_read() seek failed.\n"); exit(1); }
    long n = ftell(f);
    if (n < 0 || fseek(f, 0, SEEK_SET) != 0) { fclose(f); fprintf(stderr, "Runtime error: fs_read() size query failed.\n"); exit(1); }
    if ((unsigned long)n > (size_t)-1 - 1) { fclose(f); fprintf(stderr, "Runtime error: fs_read() file too large.\n"); exit(1); }
    char *buf = malloc((size_t)n + 1);
    if (!buf) { fclose(f); fprintf(stderr, "Runtime error: out of memory.\n"); exit(1); }
    size_t got = fread(buf, 1, (size_t)n, f);
    int bad = ferror(f) || got != (size_t)n; fclose(f);
    if (bad) { free(buf); fprintf(stderr, "Runtime error: fs_read() failed.\n"); exit(1); }
    buf[n] = '\0';
    VntValue *result = vnt_string(buf); free(buf); return result;
}
static VntValue *vnt_fs_write_mode(VntValue *path, VntValue *contents, const char *mode, const char *fn) {
    const char *p = vnt_app_string(path, fn), *data = vnt_app_string(contents, fn);
    FILE *f = fopen(p, mode);
    if (!f) { fprintf(stderr, "Runtime error: %s could not open file.\n", fn); return vnt_bool(0); }
    size_t n = strlen(data), written = fwrite(data, 1, n, f);
    int bad = written != n || ferror(f); if (fclose(f) != 0) bad = 1;
    return vnt_bool(!bad);
}
VntValue *vnt_fs_write(VntValue *p, VntValue *s) { return vnt_fs_write_mode(p, s, "wb", "fs_write()"); }
VntValue *vnt_fs_append(VntValue *p, VntValue *s) { return vnt_fs_write_mode(p, s, "ab", "fs_append()"); }
VntValue *vnt_fs_delete(VntValue *path) { return vnt_bool(remove(vnt_app_string(path, "fs_delete()")) == 0); }
VntValue *vnt_dir_create(VntValue *path) {
    const char *p = vnt_app_string(path, "dir_create()");
#ifdef _WIN32
    return vnt_bool(_mkdir(p) == 0);
#else
    return vnt_bool(mkdir(p, 0777) == 0);
#endif
}
VntValue *vnt_cwd(void) {
    size_t cap = 256; char *buf = NULL;
    for (;;) {
        char *next = realloc(buf, cap);
        if (!next) { free(buf); fprintf(stderr, "Runtime error: out of memory in cwd().\n"); exit(1); }
        buf = next;
#ifdef _WIN32
        if (_getcwd(buf, (int)cap)) break;
#else
        if (getcwd(buf, cap)) break;
#endif
        if (errno != ERANGE || cap >= 1048576) { free(buf); fprintf(stderr, "Runtime error: cwd() failed.\n"); exit(1); }
        cap *= 2;
    }
    VntValue *out = vnt_string(buf); free(buf); return out;
}
VntValue *vnt_env_get(VntValue *name) {
    const char *v = getenv(vnt_app_string(name, "env_get()")); return v ? vnt_string(v) : vnt_null();
}
VntValue *vnt_env_set(VntValue *name, VntValue *value) {
    const char *k = vnt_app_string(name, "env_set()"), *v = vnt_app_string(value, "env_set()");
#ifdef _WIN32
    return vnt_bool(_putenv_s(k, v) == 0);
#else
    return vnt_bool(setenv(k, v, 1) == 0);
#endif
}
VntValue *vnt_time_ms(void) {
#ifdef _WIN32
    return vnt_float((double)GetTickCount64());
#else
    struct timeval tv; if (gettimeofday(&tv, NULL) != 0) return vnt_float(-1.0);
    return vnt_float((double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0);
#endif
}
VntValue *vnt_sleep_ms(VntValue *value) {
    if (!value || value->type != VNT_INT || value->integer < 0) {
        fprintf(stderr, "Runtime error: sleep_ms() expects a non-negative integer.\n"); exit(1);
    }
#ifdef _WIN32
    Sleep((DWORD)value->integer);
#else
    struct timespec req = { value->integer / 1000, (long)(value->integer % 1000) * 1000000L };
    while (nanosleep(&req, &req) != 0) if (errno != EINTR) return vnt_null();
#endif
    return vnt_null();
}
