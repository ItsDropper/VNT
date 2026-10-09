#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
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
    VNT_PROCESS,
    VNT_REFERENCE
} VntType;

typedef struct VntValue VntValue;
typedef struct VntProcess VntProcess;

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
        VntProcess *process;
        struct {
            VntValue **slot;
        } reference;
    };

};

struct VntProcess {
#ifdef _WIN32
    HANDLE handle;
    HANDLE thread;
    HANDLE stdout_read;
    HANDLE stderr_read;
    DWORD pid;
#else
    pid_t pid;
    int stdout_fd;
    int stderr_fd;
    int wait_status;
#endif
    int done;
    int exit_code;
    char *stdout_data;
    size_t stdout_len;
    char *stderr_data;
    size_t stderr_len;
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
        case VNT_PROCESS:
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
        case VNT_PROCESS: printf("<process>"); break;
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


/* Native process API. Arguments are passed as an array and never interpreted
 * by a shell. Captured output is accumulated until program termination. */
static void process_append(char **dst, size_t *length, const char *data, size_t n) {
    if (!n) return;
    if (n > SIZE_MAX - *length - 1) {
        fprintf(stderr, "Runtime error: process output is too large.\n");
        exit(1);
    }
    char *next = realloc(*dst, *length + n + 1);
    if (!next) {
        fprintf(stderr, "Runtime error: out of memory capturing process output.\n");
        exit(1);
    }
    memcpy(next + *length, data, n);
    *length += n;
    next[*length] = '\0';
}
static VntProcess *process_from_value(VntValue *v, const char *fn) {
    if (!v || v->type != VNT_PROCESS || !v->process) {
        fprintf(stderr, "Runtime error: %s expects a process handle.\n", fn);
        exit(1);
    }
    return v->process;
}
#ifdef _WIN32
static void process_drain_pipe(HANDLE *pipe_handle, char **buffer, size_t *length) {
    if (!*pipe_handle || *pipe_handle == INVALID_HANDLE_VALUE) return;
    for (;;) {
        DWORD available = 0;
        if (!PeekNamedPipe(*pipe_handle, NULL, 0, NULL, &available, NULL)) {
            CloseHandle(*pipe_handle); *pipe_handle = NULL; return;
        }
        if (!available) return;
        char chunk[4096]; DWORD got = 0;
        DWORD want = available < sizeof(chunk) ? available : (DWORD)sizeof(chunk);
        if (!ReadFile(*pipe_handle, chunk, want, &got, NULL) || !got) {
            CloseHandle(*pipe_handle); *pipe_handle = NULL; return;
        }
        process_append(buffer, length, chunk, got);
    }
}
#else
static void process_drain_fd(int *fd, char **buffer, size_t *length) {
    if (*fd < 0) return;
    for (;;) {
        char chunk[4096];
        ssize_t got = read(*fd, chunk, sizeof(chunk));
        if (got > 0) { process_append(buffer, length, chunk, (size_t)got); continue; }
        if (got == 0) { close(*fd); *fd = -1; return; }
        if (errno == EINTR) continue;
        if (errno == EAGAIN || errno == EWOULDBLOCK) return;
        close(*fd); *fd = -1; return;
    }
}
#endif
static void process_refresh(VntProcess *p) {
#ifdef _WIN32
    process_drain_pipe(&p->stdout_read, &p->stdout_data, &p->stdout_len);
    process_drain_pipe(&p->stderr_read, &p->stderr_data, &p->stderr_len);
    if (!p->done && WaitForSingleObject(p->handle, 0) == WAIT_OBJECT_0) {
        DWORD code = 1;
        if (GetExitCodeProcess(p->handle, &code)) p->exit_code = (int)code;
        p->done = 1;
        if (p->thread) { CloseHandle(p->thread); p->thread = NULL; }
        if (p->handle) { CloseHandle(p->handle); p->handle = NULL; }
    }
#else
    process_drain_fd(&p->stdout_fd, &p->stdout_data, &p->stdout_len);
    process_drain_fd(&p->stderr_fd, &p->stderr_data, &p->stderr_len);
    if (!p->done) {
        int status = 0; pid_t result = waitpid(p->pid, &status, WNOHANG);
        if (result == p->pid) {
            p->wait_status = status;
            p->exit_code = WIFEXITED(status) ? WEXITSTATUS(status) :
                           (WIFSIGNALED(status) ? 128 + WTERMSIG(status) : 1);
            p->done = 1;
        }
    }
#endif
#ifdef _WIN32
    process_drain_pipe(&p->stdout_read, &p->stdout_data, &p->stdout_len);
    process_drain_pipe(&p->stderr_read, &p->stderr_data, &p->stderr_len);
#else
    process_drain_fd(&p->stdout_fd, &p->stdout_data, &p->stdout_len);
    process_drain_fd(&p->stderr_fd, &p->stderr_data, &p->stderr_len);
#endif
}
#ifdef _WIN32
static void append_win_arg(char **cmd, size_t *len, size_t *cap, const char *arg) {
    size_t need = strlen(arg) * 2 + 4;
    if (*len + need + 1 > *cap) {
        size_t nc = *cap ? *cap : 128;
        while (nc < *len + need + 1) nc *= 2;
        char *next = realloc(*cmd, nc);
        if (!next) { fprintf(stderr, "Runtime error: out of memory building process command line.\n"); exit(1); }
        *cmd = next; *cap = nc;
    }
    if (*len) (*cmd)[(*len)++] = ' ';
    (*cmd)[(*len)++] = '"';
    size_t slashes = 0;
    for (const char *s = arg; ; ++s) {
        if (*s == '\\') { ++slashes; continue; }
        if (*s == '"') {
            for (size_t i = 0; i < slashes * 2 + 1; ++i) (*cmd)[(*len)++] = '\\';
            (*cmd)[(*len)++] = '"'; slashes = 0; continue;
        }
        if (*s == '\0') {
            for (size_t i = 0; i < slashes * 2; ++i) (*cmd)[(*len)++] = '\\';
            break;
        }
        for (size_t i = 0; i < slashes; ++i) (*cmd)[(*len)++] = '\\';
        slashes = 0; (*cmd)[(*len)++] = *s;
    }
    (*cmd)[(*len)++] = '"'; (*cmd)[*len] = '\0';
}
#endif
VntValue *vnt_process_start(VntValue *executable, VntValue *arguments) {
    const char *exe = vnt_app_string(executable, "process_start()");
    if (!arguments || arguments->type != VNT_ARRAY) {
        fprintf(stderr, "Runtime error: process_start() expects an array of string arguments.\n"); exit(1);
    }
    for (int i = 0; i < arguments->array.count; ++i)
        (void)vnt_app_string(arguments->array.items[i], "process_start() arguments");
    VntProcess *p = calloc(1, sizeof(*p));
    if (!p) { fprintf(stderr, "Runtime error: out of memory starting process.\n"); exit(1); }
#ifdef _WIN32
    /* CreateProcessA does not search PATH when lpApplicationName is explicit.
       Resolve bare executable names first (e.g. "cmd.exe" -> System32\cmd.exe). */
    char resolved_exe[MAX_PATH];
    const char *launch_exe = exe;
    if (!strchr(exe, '\\') && !strchr(exe, '/') && !strchr(exe, ':')) {
        DWORD found = SearchPathA(NULL, exe, NULL, MAX_PATH, resolved_exe, NULL);
        if (found == 0 || found >= MAX_PATH) {
            DWORD error = found == 0 ? GetLastError() : ERROR_INSUFFICIENT_BUFFER;
            free(p);
            fprintf(stderr, "Runtime error: process_start() could not find executable '%s' (Windows error %lu).\n",
                    exe, (unsigned long)error);
            exit(1);
        }
        launch_exe = resolved_exe;
    }
    p->stdout_read = NULL; p->stderr_read = NULL;
    SECURITY_ATTRIBUTES sa; memset(&sa, 0, sizeof(sa)); sa.nLength = sizeof(sa); sa.bInheritHandle = TRUE;
    HANDLE out_write = NULL, err_write = NULL;
    if (!CreatePipe(&p->stdout_read, &out_write, &sa, 0) ||
        !CreatePipe(&p->stderr_read, &err_write, &sa, 0)) {
        if (p->stdout_read) CloseHandle(p->stdout_read);
        if (out_write) CloseHandle(out_write);
        if (p->stderr_read) CloseHandle(p->stderr_read);
        if (err_write) CloseHandle(err_write);
        free(p); fprintf(stderr, "Runtime error: process_start() could not create output pipes.\n"); return vnt_null();
    }
    SetHandleInformation(p->stdout_read, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(p->stderr_read, HANDLE_FLAG_INHERIT, 0);
    char *cmd = NULL; size_t cmd_len = 0, cmd_cap = 0;
    append_win_arg(&cmd, &cmd_len, &cmd_cap, launch_exe);
    for (int i = 0; i < arguments->array.count; ++i)
        append_win_arg(&cmd, &cmd_len, &cmd_cap, arguments->array.items[i]->string);
    STARTUPINFOA si; PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si)); memset(&pi, 0, sizeof(pi));
    si.cb = sizeof(si); si.dwFlags = STARTF_USESTDHANDLES;
    si.hStdInput = GetStdHandle(STD_INPUT_HANDLE); si.hStdOutput = out_write; si.hStdError = err_write;
    BOOL ok = CreateProcessA(launch_exe, cmd, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
    free(cmd); CloseHandle(out_write); CloseHandle(err_write);
    if (!ok) {
        DWORD error = GetLastError();
        CloseHandle(p->stdout_read); CloseHandle(p->stderr_read); free(p);
        fprintf(stderr, "Runtime error: process_start() failed to launch '%s' (Windows error %lu).\n", exe, (unsigned long)error);
        exit(1);
    }
    p->handle = pi.hProcess; p->thread = pi.hThread; p->pid = pi.dwProcessId;
#else
    int out_pipe[2] = {-1,-1}, err_pipe[2] = {-1,-1};
    if (pipe(out_pipe) != 0 || pipe(err_pipe) != 0) {
        if (out_pipe[0] >= 0) close(out_pipe[0]); if (out_pipe[1] >= 0) close(out_pipe[1]);
        if (err_pipe[0] >= 0) close(err_pipe[0]); if (err_pipe[1] >= 0) close(err_pipe[1]);
        free(p); fprintf(stderr, "Runtime error: process_start() could not create output pipes.\n"); return vnt_null();
    }
    pid_t pid = fork();
    if (pid < 0) {
        close(out_pipe[0]); close(out_pipe[1]); close(err_pipe[0]); close(err_pipe[1]); free(p);
        fprintf(stderr, "Runtime error: process_start() could not fork.\n"); return vnt_null();
    }
    if (pid == 0) {
        dup2(out_pipe[1], STDOUT_FILENO); dup2(err_pipe[1], STDERR_FILENO);
        close(out_pipe[0]); close(out_pipe[1]); close(err_pipe[0]); close(err_pipe[1]);
        char **argv = calloc((size_t)arguments->array.count + 2, sizeof(char *));
        if (!argv) _exit(126);
        argv[0] = (char *)exe;
        for (int i = 0; i < arguments->array.count; ++i) argv[i+1] = arguments->array.items[i]->string;
        execvp(exe, argv);
        dprintf(STDERR_FILENO, "VNT process_start: could not execute '%s': %s\n", exe, strerror(errno));
        _exit(127);
    }
    close(out_pipe[1]); close(err_pipe[1]);
    int flags = fcntl(out_pipe[0], F_GETFL, 0); if (flags >= 0) fcntl(out_pipe[0], F_SETFL, flags | O_NONBLOCK);
    flags = fcntl(err_pipe[0], F_GETFL, 0); if (flags >= 0) fcntl(err_pipe[0], F_SETFL, flags | O_NONBLOCK);
    p->pid = pid; p->stdout_fd = out_pipe[0]; p->stderr_fd = err_pipe[0];
#endif
    VntValue *result = alloc_value(VNT_PROCESS); result->process = p; return result;
}
VntValue *vnt_process_poll(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_poll()"); process_refresh(p); return vnt_bool(p->done);
}
VntValue *vnt_process_wait(VntValue *value, VntValue *timeout) {
    VntProcess *p = process_from_value(value, "process_wait()");
    if (!timeout || timeout->type != VNT_INT || timeout->integer < -1) {
        fprintf(stderr, "Runtime error: process_wait() timeout must be -1 or a non-negative integer.\n"); exit(1);
    }
    uint64_t start;
#ifdef _WIN32
    start = GetTickCount64();
#else
    struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts);
    start = (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
#endif
    for (;;) {
        process_refresh(p);
        if (p->done) return vnt_bool(1);
        if (timeout->integer >= 0) {
            uint64_t now;
#ifdef _WIN32
            now = GetTickCount64();
#else
            clock_gettime(CLOCK_MONOTONIC, &ts);
            now = (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
#endif
            if (now - start >= (uint64_t)timeout->integer) return vnt_bool(0);
        }
#ifdef _WIN32
        Sleep(5);
#else
        struct timespec delay = {0,5000000L}; nanosleep(&delay, NULL);
#endif
    }
}
VntValue *vnt_process_pid(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_pid()"); return vnt_int((int)p->pid);
}
VntValue *vnt_process_terminate(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_terminate()"); process_refresh(p);
    if (p->done) return vnt_bool(0);
#ifdef _WIN32
    return vnt_bool(TerminateProcess(p->handle, 1) != 0);
#else
    return vnt_bool(kill(p->pid, SIGTERM) == 0);
#endif
}
VntValue *vnt_process_stdout(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_stdout()"); process_refresh(p);
    return vnt_string(p->stdout_data ? p->stdout_data : "");
}
VntValue *vnt_process_stderr(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_stderr()"); process_refresh(p);
    return vnt_string(p->stderr_data ? p->stderr_data : "");
}
VntValue *vnt_process_exit_code(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_exit_code()"); process_refresh(p);
    return vnt_int(p->done ? p->exit_code : -1);
}


/* Vanta GUI native backend: retained command list + double-buffered WM_PAINT. */
#ifdef _WIN32
enum { VG_TEXT=1, VG_TITLE, VG_PANEL, VG_BUTTON, VG_INPUT, VG_TEXTAREA, VG_CHECKBOX, VG_PROGRESS, VG_SEPARATOR, VG_RECT };
typedef struct {
    int type,x,y,w,h,font_size,radius,hovered,checked,value,maximum,placeholder;
    COLORREF color,background;
    char text[512];
} VntGuiCommand;
typedef struct { int x,y,width,height,multiline,select_all; char text[512]; } VntGuiInputState;
static HWND vnt_gui_hwnd;
static int vnt_gui_width=800, vnt_gui_height=600, vnt_gui_last_key, vnt_gui_text_y=18;
static COLORREF vnt_gui_background=RGB(11,16,32);
static const char *vnt_gui_class_name="VNTNativeWindow";
static int vnt_gui_clicked, vnt_gui_click_x, vnt_gui_click_y, vnt_gui_mouse_x, vnt_gui_mouse_y;
static COLORREF vnt_gui_text_color=RGB(170,182,211), vnt_gui_title_color=RGB(244,247,255);
static COLORREF vnt_gui_panel_background=RGB(21,30,51), vnt_gui_button_background=RGB(49,92,232);
static COLORREF vnt_gui_button_hover=RGB(80,120,255), vnt_gui_button_text=RGB(255,255,255);
static COLORREF vnt_gui_input_background=RGB(10,15,29), vnt_gui_accent=RGB(49,92,232);
static int vnt_gui_font_size=16, vnt_gui_title_font_size=30, vnt_gui_button_font_size=15;
static int vnt_gui_button_padding=10, vnt_gui_button_radius=9;
static VntGuiCommand vnt_gui_commands[4096];
static int vnt_gui_command_count;
static VntGuiInputState vnt_gui_inputs[16];
static int vnt_gui_input_count, vnt_gui_focused_input=-1;

static int vnt_gui_require_int(VntValue *v,const char *fn) {
    if(!v || v->type!=VNT_INT) { fprintf(stderr,"Runtime error: %s expects integer arguments.\n",fn); exit(1); }
    return v->integer;
}
static COLORREF vnt_gui_color(int c) { return RGB((c>>16)&255,(c>>8)&255,c&255); }
static char *vnt_gui_trim(char *s) {
    while(*s==' '||*s=='\t'||*s=='\r'||*s=='\n') s++;
    size_t n=strlen(s); while(n&&(s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\r'||s[n-1]=='\n')) s[--n]=0;
    return s;
}
static int vnt_gui_parse_color(const char *s,COLORREF *out) {
    if(!s||s[0]!='#'||strlen(s)!=7) return 0;
    char *end=NULL; unsigned long rgb=strtoul(s+1,&end,16);
    if(!end||*end) return 0; *out=RGB((rgb>>16)&255,(rgb>>8)&255,rgb&255); return 1;
}
static void vnt_gui_apply_css_rule(const char *css,const char *selector) {
    size_t slen=strlen(selector); const char *p=css;
    while((p=strstr(p,selector))!=NULL) {
        if(p!=css&&(isalnum((unsigned char)p[-1])||p[-1]=='_'||p[-1]=='-')) { p+=slen; continue; }
        const char *q=p+slen; while(*q==' '||*q=='\t'||*q=='\r'||*q=='\n') q++;
        if(*q!='{') { p+=slen; continue; }
        const char *end=strchr(q+1,'}'); if(!end) return;
        size_t len=(size_t)(end-q-1); char *body=(char*)malloc(len+1); if(!body)return;
        memcpy(body,q+1,len); body[len]=0; char *save=NULL;
        for(char *decl=strtok_s(body,";",&save);decl;decl=strtok_s(NULL,";",&save)) {
            char *colon=strchr(decl,':'); if(!colon)continue; *colon=0;
            char *key=vnt_gui_trim(decl), *value=vnt_gui_trim(colon+1); COLORREF c;
            if(!strcmp(selector,"window")&&!strcmp(key,"background-color")&&vnt_gui_parse_color(value,&c))vnt_gui_background=c;
            else if(!strcmp(selector,"label")&&!strcmp(key,"color")&&vnt_gui_parse_color(value,&c))vnt_gui_text_color=c;
            else if(!strcmp(selector,"label")&&!strcmp(key,"font-size")){int n=atoi(value);if(n>=8&&n<=72)vnt_gui_font_size=n;}
            else if(!strcmp(selector,"title")&&!strcmp(key,"color")&&vnt_gui_parse_color(value,&c))vnt_gui_title_color=c;
            else if(!strcmp(selector,"title")&&!strcmp(key,"font-size")){int n=atoi(value);if(n>=12&&n<=72)vnt_gui_title_font_size=n;}
            else if(!strcmp(selector,"panel")&&!strcmp(key,"background-color")&&vnt_gui_parse_color(value,&c))vnt_gui_panel_background=c;
            else if((!strcmp(selector,"button")||!strcmp(selector,"button:hover"))&&!strcmp(key,"background-color")&&vnt_gui_parse_color(value,&c)){if(!strcmp(selector,"button:hover"))vnt_gui_button_hover=c;else vnt_gui_button_background=c;}
            else if(!strcmp(selector,"button")&&!strcmp(key,"color")&&vnt_gui_parse_color(value,&c))vnt_gui_button_text=c;
            else if(!strcmp(selector,"button")&&!strcmp(key,"font-size")){int n=atoi(value);if(n>=8&&n<=48)vnt_gui_button_font_size=n;}
            else if(!strcmp(selector,"button")&&!strcmp(key,"padding")){int n=atoi(value);if(n>=0&&n<=48)vnt_gui_button_padding=n;}
            else if(!strcmp(selector,"button")&&!strcmp(key,"border-radius")){int n=atoi(value);if(n>=0&&n<=32)vnt_gui_button_radius=n;}
        }
        free(body); return;
    }
}
static VntGuiCommand *vnt_gui_add(int type,int x,int y,int w,int h,const char *text) {
    if(vnt_gui_command_count>=4096) return NULL;
    VntGuiCommand *c=&vnt_gui_commands[vnt_gui_command_count++]; memset(c,0,sizeof(*c));
    c->type=type;c->x=x;c->y=y;c->w=w;c->h=h;
    if(text) snprintf(c->text,sizeof(c->text),"%s",text);
    return c;
}
static void vnt_gui_render(HDC dc) {
    RECT client; GetClientRect(vnt_gui_hwnd,&client);
    HBRUSH bg=CreateSolidBrush(vnt_gui_background); FillRect(dc,&client,bg); DeleteObject(bg);
    SetBkMode(dc,TRANSPARENT);
    for(int i=0;i<vnt_gui_command_count;i++) {
        VntGuiCommand *c=&vnt_gui_commands[i]; RECT r={c->x,c->y,c->x+c->w,c->y+c->h};
        COLORREF fill=c->background, ink=c->color;
        if(c->type==VG_TEXT||c->type==VG_TITLE) {
            HFONT font=CreateFontA(-c->font_size,0,0,0,c->type==VG_TITLE?FW_SEMIBOLD:FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Segoe UI");
            HGDIOBJ old=font?SelectObject(dc,font):NULL; SetTextColor(dc,ink);
            TextOutA(dc,c->x,c->y,c->text,(int)strlen(c->text));
            if(old)SelectObject(dc,old);if(font)DeleteObject(font);
        } else if(c->type==VG_PANEL||c->type==VG_BUTTON||c->type==VG_INPUT||c->type==VG_TEXTAREA||c->type==VG_CHECKBOX) {
            if(c->type==VG_BUTTON) fill=c->hovered?vnt_gui_button_hover:vnt_gui_button_background;
            if(c->type==VG_INPUT||c->type==VG_TEXTAREA) fill=vnt_gui_input_background;
            if(c->type==VG_CHECKBOX) fill=c->checked?vnt_gui_accent:vnt_gui_input_background;
            HBRUSH brush=CreateSolidBrush(fill); HPEN pen=CreatePen(PS_SOLID,1,fill);
            HGDIOBJ ob=brush?SelectObject(dc,brush):NULL, op=pen?SelectObject(dc,pen):NULL;
            if(c->type==VG_PANEL||c->type==VG_BUTTON||c->type==VG_TEXTAREA) RoundRect(dc,r.left,r.top,r.right,r.bottom,c->radius*2,c->radius*2);
            else Rectangle(dc,r.left,r.top,r.right,r.bottom);
            if(op)SelectObject(dc,op);if(ob)SelectObject(dc,ob);if(pen)DeleteObject(pen);if(brush)DeleteObject(brush);
            if(c->type==VG_CHECKBOX) {
                RECT tr={c->x+28,c->y-1,c->x+c->w,c->y+c->h+1};SetTextColor(dc,vnt_gui_text_color);DrawTextA(dc,c->text,-1,&tr,DT_LEFT|DT_VCENTER|DT_SINGLELINE);
                if(c->checked){HPEN cp=CreatePen(PS_SOLID,2,RGB(255,255,255));HGDIOBJ op2=SelectObject(dc,cp);MoveToEx(dc,c->x+5,c->y+c->h/2, NULL);LineTo(dc,c->x+10,c->y+c->h-5);LineTo(dc,c->x+19,c->y+5);SelectObject(dc,op2);DeleteObject(cp);}
            } else {
                HFONT font=CreateFontA(-c->font_size,0,0,0,c->type==VG_BUTTON?FW_SEMIBOLD:FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Segoe UI");
                HGDIOBJ old=font?SelectObject(dc,font):NULL;
                SetTextColor(dc,c->type==VG_BUTTON?vnt_gui_button_text:vnt_gui_text_color);
                if(c->placeholder) SetTextColor(dc,RGB(110,125,153));
                RECT tr={r.left+10,r.top+(c->type==VG_TEXTAREA?8:0),r.right-8,r.bottom-6};
                if(c->type==VG_TEXTAREA) DrawTextA(dc,c->text,-1,&tr,DT_LEFT|DT_TOP|DT_WORDBREAK|DT_NOPREFIX);
                else DrawTextA(dc,c->text,-1,&tr,DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
                if(c->type==VG_INPUT&&vnt_gui_focused_input>=0&&vnt_gui_inputs[vnt_gui_focused_input].x==c->x&&vnt_gui_inputs[vnt_gui_focused_input].y==c->y&&!c->placeholder) { SIZE sz={0};GetTextExtentPoint32A(dc,c->text,(int)strlen(c->text),&sz);MoveToEx(dc,r.left+10+sz.cx,r.top+7,NULL);LineTo(dc,r.left+10+sz.cx,r.bottom-7); }
                if(old)SelectObject(dc,old);if(font)DeleteObject(font);
            }
        } else if(c->type==VG_PROGRESS) {
            HBRUSH track=CreateSolidBrush(vnt_gui_input_background);FillRect(dc,&r,track);DeleteObject(track);
            int max=c->maximum>0?c->maximum:1;int width=(int)((long long)c->w*c->value/max);if(width<0)width=0;if(width>c->w)width=c->w;
            RECT fillr={c->x,c->y,c->x+width,c->y+c->h};HBRUSH fillb=CreateSolidBrush(vnt_gui_accent);FillRect(dc,&fillr,fillb);DeleteObject(fillb);
        } else if(c->type==VG_SEPARATOR) {
            HPEN pen=CreatePen(PS_SOLID,1,vnt_gui_panel_background);HGDIOBJ old=SelectObject(dc,pen);MoveToEx(dc,c->x,c->y,NULL);LineTo(dc,c->x+c->w,c->y);SelectObject(dc,old);DeleteObject(pen);
        } else if(c->type==VG_RECT) {
            HBRUSH brush=CreateSolidBrush(c->background);FillRect(dc,&r,brush);DeleteObject(brush);
        }
    }
}
static LRESULT CALLBACK vnt_gui_wndproc(HWND hwnd,UINT msg,WPARAM wp,LPARAM lp) {
    switch(msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;HDC target=BeginPaint(hwnd,&ps);RECT r;GetClientRect(hwnd,&r);
            int w=r.right-r.left,h=r.bottom-r.top;
            if(target&&w>0&&h>0){HDC memory=CreateCompatibleDC(target);HBITMAP bitmap=CreateCompatibleBitmap(target,w,h);
                if(memory&&bitmap){HGDIOBJ old=SelectObject(memory,bitmap);vnt_gui_render(memory);BitBlt(target,0,0,w,h,memory,0,0,SRCCOPY);SelectObject(memory,old);}
                if(bitmap)DeleteObject(bitmap);if(memory)DeleteDC(memory);
            }
            EndPaint(hwnd,&ps);return 0;
        }
        case WM_ERASEBKGND:return 1;
        case WM_KEYDOWN:
            vnt_gui_last_key=(int)wp;
            if(wp=='A'&&(GetKeyState(VK_CONTROL)&0x8000)&&vnt_gui_focused_input>=0&&vnt_gui_focused_input<vnt_gui_input_count) vnt_gui_inputs[vnt_gui_focused_input].select_all=1;
            return 0;
        case WM_CHAR:
            if(vnt_gui_focused_input>=0&&vnt_gui_focused_input<vnt_gui_input_count){
                VntGuiInputState *state=&vnt_gui_inputs[vnt_gui_focused_input];char *s=state->text;size_t n=strlen(s);
                if(state->select_all){s[0]=0;n=0;state->select_all=0;}
                if(wp==VK_BACK){if(n)s[n-1]=0;}
                else if((wp=='\r'||wp=='\n')&&state->multiline){if(n<sizeof(state->text)-1){s[n]='\n';s[n+1]=0;}}
                else if(wp>=32&&wp<127&&n<sizeof(state->text)-1){s[n]=(char)wp;s[n+1]=0;}
                InvalidateRect(hwnd,NULL,FALSE);
            }
            return 0;
        case WM_MOUSEMOVE:{int oldx=vnt_gui_mouse_x,oldy=vnt_gui_mouse_y;vnt_gui_mouse_x=(short)LOWORD(lp);vnt_gui_mouse_y=(short)HIWORD(lp);if(oldx!=vnt_gui_mouse_x||oldy!=vnt_gui_mouse_y)InvalidateRect(hwnd,NULL,FALSE);return 0;}
        case WM_LBUTTONUP:vnt_gui_click_x=(short)LOWORD(lp);vnt_gui_click_y=(short)HIWORD(lp);vnt_gui_clicked=1;return 0;
        case WM_DESTROY:if(hwnd==vnt_gui_hwnd)vnt_gui_hwnd=NULL;PostQuitMessage(0);return 0;
        default:return DefWindowProcA(hwnd,msg,wp,lp);
    }
}
VntValue *vnt_gui_css(VntValue *path) {
    const char *filename=vnt_app_string(path,"gui_css()");FILE *file=fopen(filename,"rb");if(!file)return vnt_bool(0);
    char *css=(char*)malloc(65536);if(!css){fclose(file);return vnt_bool(0);}
    size_t n=fread(css,1,65535,file);fclose(file);css[n]=0;
    if(n==65535){free(css);fprintf(stderr,"Runtime error: stylesheet exceeds 65535 bytes.\n");return vnt_bool(0);}
    vnt_gui_apply_css_rule(css,"window");vnt_gui_apply_css_rule(css,"label");vnt_gui_apply_css_rule(css,"title");
    vnt_gui_apply_css_rule(css,"panel");vnt_gui_apply_css_rule(css,"button");vnt_gui_apply_css_rule(css,"button:hover");free(css);
    return vnt_bool(1);
}
VntValue *vnt_gui_size(VntValue *width,VntValue *height) {
    int w=vnt_gui_require_int(width,"gui_size()"),h=vnt_gui_require_int(height,"gui_size()");
    if(w<160||h<120||w>8192||h>8192){fprintf(stderr,"Runtime error: gui_size() dimensions must be between 160x120 and 8192x8192.\n");return vnt_bool(0);}
    vnt_gui_width=w;vnt_gui_height=h;return vnt_bool(1);
}
VntValue *vnt_gui_open(VntValue *title) {
    const char *caption=vnt_app_string(title,"gui_open()");if(vnt_gui_hwnd)return vnt_bool(1);
    HINSTANCE instance=GetModuleHandleA(NULL);WNDCLASSEXA wc;memset(&wc,0,sizeof(wc));
    wc.cbSize=sizeof(wc);wc.lpfnWndProc=vnt_gui_wndproc;wc.hInstance=instance;wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=NULL;wc.lpszClassName=vnt_gui_class_name;
    if(!RegisterClassExA(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return vnt_bool(0);
    RECT rect={0,0,vnt_gui_width,vnt_gui_height};AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
    vnt_gui_text_y=18;vnt_gui_last_key=0;vnt_gui_clicked=0;vnt_gui_command_count=0;vnt_gui_input_count=0;vnt_gui_focused_input=-1;
    vnt_gui_hwnd=CreateWindowExA(0,vnt_gui_class_name,caption,WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,rect.right-rect.left,rect.bottom-rect.top,NULL,NULL,instance,NULL);
    if(!vnt_gui_hwnd)return vnt_bool(0);ShowWindow(vnt_gui_hwnd,SW_SHOW);UpdateWindow(vnt_gui_hwnd);return vnt_bool(1);
}
static VntValue *vnt_gui_draw_text(const char *text,int x,int y,int title) {
    VntGuiCommand *c=vnt_gui_add(title?VG_TITLE:VG_TEXT,x,y,0,0,text);if(!c)return vnt_bool(0);
    c->font_size=title?vnt_gui_title_font_size:vnt_gui_font_size;c->color=title?vnt_gui_title_color:vnt_gui_text_color;return vnt_bool(1);
}
VntValue *vnt_gui_text(VntValue *text){const char *s=vnt_app_string(text,"gui_text()");int y=vnt_gui_text_y;vnt_gui_text_y+=vnt_gui_font_size+8;RECT r;if(vnt_gui_hwnd&&GetClientRect(vnt_gui_hwnd,&r)&&vnt_gui_text_y>r.bottom-20)vnt_gui_text_y=18;return vnt_gui_draw_text(s,18,y,0);}
VntValue *vnt_gui_text_at(VntValue *text,VntValue *x,VntValue *y){return vnt_gui_draw_text(vnt_app_string(text,"gui_text_at()"),vnt_gui_require_int(x,"gui_text_at()"),vnt_gui_require_int(y,"gui_text_at()"),0);}
VntValue *vnt_gui_title(VntValue *text,VntValue *x,VntValue *y){return vnt_gui_draw_text(vnt_app_string(text,"gui_title()"),vnt_gui_require_int(x,"gui_title()"),vnt_gui_require_int(y,"gui_title()"),1);}
VntValue *vnt_gui_panel(VntValue *x,VntValue *y,VntValue *w,VntValue *h){
    int xx=vnt_gui_require_int(x,"gui_panel()"),yy=vnt_gui_require_int(y,"gui_panel()"),ww=vnt_gui_require_int(w,"gui_panel()"),hh=vnt_gui_require_int(h,"gui_panel()");
    VntGuiCommand *c=vnt_gui_add(VG_PANEL,xx,yy,ww,hh,NULL);if(!c)return vnt_bool(0);c->background=vnt_gui_panel_background;c->radius=12;return vnt_bool(1);
}
static int vnt_gui_input_state(int x,int y,int width,int height,int multiline,int create){
    for(int i=0;i<vnt_gui_input_count;i++) if(vnt_gui_inputs[i].x==x&&vnt_gui_inputs[i].y==y){vnt_gui_inputs[i].width=width;vnt_gui_inputs[i].height=height;vnt_gui_inputs[i].multiline=multiline;return i;}
    if(!create||vnt_gui_input_count>=16)return -1;
    int i=vnt_gui_input_count++;memset(&vnt_gui_inputs[i],0,sizeof(vnt_gui_inputs[i]));
    vnt_gui_inputs[i].x=x;vnt_gui_inputs[i].y=y;vnt_gui_inputs[i].width=width;vnt_gui_inputs[i].height=height;vnt_gui_inputs[i].multiline=multiline;return i;
}
static VntValue *vnt_gui_button_draw(const char *text,int xx,int yy,int w,int h){
    if(w<24)w=24;if(h<24)h=24;RECT r={xx,yy,xx+w,yy+h};
    int hover=vnt_gui_mouse_x>=r.left&&vnt_gui_mouse_x<r.right&&vnt_gui_mouse_y>=r.top&&vnt_gui_mouse_y<r.bottom;
    VntGuiCommand *c=vnt_gui_add(VG_BUTTON,xx,yy,w,h,text);if(c){c->font_size=vnt_gui_button_font_size;c->radius=vnt_gui_button_radius;c->hovered=hover;}
    return vnt_bool(vnt_gui_clicked&&vnt_gui_click_x>=r.left&&vnt_gui_click_x<r.right&&vnt_gui_click_y>=r.top&&vnt_gui_click_y<r.bottom);
}
VntValue *vnt_gui_button(VntValue *label,VntValue *x,VntValue *y){
    const char *text=vnt_app_string(label,"gui_button()");int xx=vnt_gui_require_int(x,"gui_button()"),yy=vnt_gui_require_int(y,"gui_button()");
    int w=(int)(strlen(text)*vnt_gui_button_font_size*0.62)+vnt_gui_button_padding*2+16,h=vnt_gui_button_font_size+vnt_gui_button_padding*2+2;
    return vnt_gui_button_draw(text,xx,yy,w,h);
}
VntValue *vnt_gui_button_sized(VntValue *label,VntValue *x,VntValue *y,VntValue *width,VntValue *height){
    return vnt_gui_button_draw(vnt_app_string(label,"gui_button_sized()"),vnt_gui_require_int(x,"gui_button_sized()"),vnt_gui_require_int(y,"gui_button_sized()"),vnt_gui_require_int(width,"gui_button_sized()"),vnt_gui_require_int(height,"gui_button_sized()"));
}
VntValue *vnt_gui_input(VntValue *label,VntValue *x,VntValue *y,VntValue *width){
    const char *placeholder=vnt_app_string(label,"gui_input()");int xx=vnt_gui_require_int(x,"gui_input()"),yy=vnt_gui_require_int(y,"gui_input()"),ww=vnt_gui_require_int(width,"gui_input()");
    int index=vnt_gui_input_state(xx,yy,ww,36,0,1);
    if(index>=0){if(vnt_gui_clicked&&vnt_gui_click_x>=xx&&vnt_gui_click_x<xx+ww&&vnt_gui_click_y>=yy&&vnt_gui_click_y<yy+36)vnt_gui_focused_input=index;
        int empty=!vnt_gui_inputs[index].text[0];VntGuiCommand *c=vnt_gui_add(VG_INPUT,xx,yy,ww,36,empty?placeholder:vnt_gui_inputs[index].text);if(c){c->font_size=vnt_gui_font_size;c->radius=7;c->placeholder=empty;}
        return vnt_string(vnt_gui_inputs[index].text);}
    return vnt_string("");
}
VntValue *vnt_gui_textarea(VntValue *label,VntValue *x,VntValue *y,VntValue *width,VntValue *height){
    const char *placeholder=vnt_app_string(label,"gui_textarea()");int xx=vnt_gui_require_int(x,"gui_textarea()"),yy=vnt_gui_require_int(y,"gui_textarea()"),ww=vnt_gui_require_int(width,"gui_textarea()"),hh=vnt_gui_require_int(height,"gui_textarea()");
    int index=vnt_gui_input_state(xx,yy,ww,hh,1,1);
    if(index>=0){if(vnt_gui_clicked&&vnt_gui_click_x>=xx&&vnt_gui_click_x<xx+ww&&vnt_gui_click_y>=yy&&vnt_gui_click_y<yy+hh)vnt_gui_focused_input=index;
        int empty=!vnt_gui_inputs[index].text[0];VntGuiCommand *c=vnt_gui_add(VG_TEXTAREA,xx,yy,ww,hh,empty?placeholder:vnt_gui_inputs[index].text);if(c){c->font_size=vnt_gui_font_size;c->radius=8;c->placeholder=empty;}
        return vnt_string(vnt_gui_inputs[index].text);}
    return vnt_string("");
}
VntValue *vnt_gui_input_set(VntValue *text,VntValue *x,VntValue *y){
    const char *value=vnt_app_string(text,"gui_input_set()");int xx=vnt_gui_require_int(x,"gui_input_set()"),yy=vnt_gui_require_int(y,"gui_input_set()");
    int index=vnt_gui_input_state(xx,yy,0,0,0,1);if(index<0)return vnt_bool(0);
    snprintf(vnt_gui_inputs[index].text,sizeof(vnt_gui_inputs[index].text),"%s",value);vnt_gui_inputs[index].select_all=0;return vnt_bool(1);
}
VntValue *vnt_gui_panel_color(VntValue *x,VntValue *y,VntValue *w,VntValue *h,VntValue *color){
    int xx=vnt_gui_require_int(x,"gui_panel_color()"),yy=vnt_gui_require_int(y,"gui_panel_color()"),ww=vnt_gui_require_int(w,"gui_panel_color()"),hh=vnt_gui_require_int(h,"gui_panel_color()");
    VntGuiCommand *c=vnt_gui_add(VG_PANEL,xx,yy,ww,hh,NULL);if(!c)return vnt_bool(0);c->background=vnt_gui_color(vnt_gui_require_int(color,"gui_panel_color()"));c->radius=12;return vnt_bool(1);
}
VntValue *vnt_gui_text_style(VntValue *text,VntValue *x,VntValue *y,VntValue *color,VntValue *size){
    const char *s=vnt_app_string(text,"gui_text_style()");int xx=vnt_gui_require_int(x,"gui_text_style()"),yy=vnt_gui_require_int(y,"gui_text_style()"),font=vnt_gui_require_int(size,"gui_text_style()");
    if(font<8||font>72)font=16;VntGuiCommand *c=vnt_gui_add(VG_TEXT,xx,yy,0,0,s);if(!c)return vnt_bool(0);c->font_size=font;c->color=vnt_gui_color(vnt_gui_require_int(color,"gui_text_style()"));return vnt_bool(1);
}

VntValue *vnt_gui_checkbox(VntValue *label,VntValue *x,VntValue *y,VntValue *checked){
    const char *text=vnt_app_string(label,"gui_checkbox()");int xx=vnt_gui_require_int(x,"gui_checkbox()"),yy=vnt_gui_require_int(y,"gui_checkbox()");
    int on=checked&&(checked->type==VNT_BOOL?checked->boolean:checked->type==VNT_INT?checked->integer:0);
    RECT r={xx,yy,xx+24,yy+24};if(vnt_gui_clicked&&vnt_gui_click_x>=xx&&vnt_gui_click_x<xx+250&&vnt_gui_click_y>=yy&&vnt_gui_click_y<yy+28)on=!on;
    VntGuiCommand *c=vnt_gui_add(VG_CHECKBOX,xx,yy,24,24,text);if(c)c->checked=on;
    return vnt_bool(on);
}
VntValue *vnt_gui_progress(VntValue *value,VntValue *maximum,VntValue *x,VntValue *y){
    int v=vnt_gui_require_int(value,"gui_progress()"),m=vnt_gui_require_int(maximum,"gui_progress()"),xx=vnt_gui_require_int(x,"gui_progress()"),yy=vnt_gui_require_int(y,"gui_progress()");
    VntGuiCommand *c=vnt_gui_add(VG_PROGRESS,xx,yy,260,10,NULL);if(!c)return vnt_bool(0);c->value=v;c->maximum=m;return vnt_bool(1);
}
VntValue *vnt_gui_separator(VntValue *x,VntValue *y,VntValue *width){
    int xx=vnt_gui_require_int(x,"gui_separator()"),yy=vnt_gui_require_int(y,"gui_separator()"),ww=vnt_gui_require_int(width,"gui_separator()");
    VntGuiCommand *c=vnt_gui_add(VG_SEPARATOR,xx,yy,ww,1,NULL);return vnt_bool(c!=NULL);
}
VntValue *vnt_gui_fill(VntValue *color){vnt_gui_background=vnt_gui_color(vnt_gui_require_int(color,"gui_fill()"));return vnt_bool(1);}
VntValue *vnt_gui_rect(VntValue *color){VntGuiCommand *c=vnt_gui_add(VG_RECT,32,64,160,80,NULL);if(!c)return vnt_bool(0);c->background=vnt_gui_color(vnt_gui_require_int(color,"gui_rect()"));return vnt_bool(1);}
VntValue *vnt_gui_poll(void){
    if(!vnt_gui_hwnd)return vnt_bool(0);MSG msg;
    vnt_gui_clicked=0;
    while(PeekMessageA(&msg,NULL,0,0,PM_REMOVE)){if(msg.message==WM_QUIT){vnt_gui_hwnd=NULL;return vnt_bool(0);}TranslateMessage(&msg);DispatchMessageA(&msg);}
    vnt_gui_command_count=0;vnt_gui_text_y=18;
    return vnt_bool(vnt_gui_hwnd!=NULL);
}
VntValue *vnt_gui_present(void){if(!vnt_gui_hwnd)return vnt_bool(0);InvalidateRect(vnt_gui_hwnd,NULL,FALSE);UpdateWindow(vnt_gui_hwnd);return vnt_bool(1);}
VntValue *vnt_gui_key(void){int key=vnt_gui_last_key;vnt_gui_last_key=0;return vnt_int(key);}
VntValue *vnt_gui_close(void){if(vnt_gui_hwnd)DestroyWindow(vnt_gui_hwnd);vnt_gui_hwnd=NULL;vnt_gui_command_count=0;return vnt_bool(1);}
#else
VntValue *vnt_gui_css(VntValue *path) { (void)path;fprintf(stderr,"Runtime error: native GUI is currently supported on Windows only.\n");return vnt_bool(0); }
VntValue *vnt_gui_size(VntValue *width,VntValue *height) { (void)width;(void)height;fprintf(stderr,"Runtime error: native GUI is currently supported on Windows only.\n");return vnt_bool(0); }
VntValue *vnt_gui_open(VntValue *title) { (void)title;fprintf(stderr,"Runtime error: native GUI is currently supported on Windows only.\n");return vnt_bool(0); }
VntValue *vnt_gui_text(VntValue *text) { (void)text;return vnt_bool(0); }
VntValue *vnt_gui_text_at(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VntValue *vnt_gui_title(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VntValue *vnt_gui_panel(VntValue *x,VntValue *y,VntValue *w,VntValue *h) { (void)x;(void)y;(void)w;(void)h;return vnt_bool(0); }
VntValue *vnt_gui_fill(VntValue *color) { (void)color;return vnt_bool(0); }
VntValue *vnt_gui_rect(VntValue *color) { (void)color;return vnt_bool(0); }
VntValue *vnt_gui_button(VntValue *label,VntValue *x,VntValue *y) { (void)label;(void)x;(void)y;return vnt_bool(0); }
VntValue *vnt_gui_button_sized(VntValue *label,VntValue *x,VntValue *y,VntValue *w,VntValue *h) { (void)label;(void)x;(void)y;(void)w;(void)h;return vnt_bool(0); }
VntValue *vnt_gui_input(VntValue *label,VntValue *x,VntValue *y,VntValue *w) { (void)label;(void)x;(void)y;(void)w;return vnt_string(""); }
VntValue *vnt_gui_textarea(VntValue *label,VntValue *x,VntValue *y,VntValue *w,VntValue *h) { (void)label;(void)x;(void)y;(void)w;(void)h;return vnt_string(""); }
VntValue *vnt_gui_input_set(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VntValue *vnt_gui_panel_color(VntValue *x,VntValue *y,VntValue *w,VntValue *h,VntValue *c) { (void)x;(void)y;(void)w;(void)h;(void)c;return vnt_bool(0); }
VntValue *vnt_gui_text_style(VntValue *t,VntValue *x,VntValue *y,VntValue *c,VntValue *s) { (void)t;(void)x;(void)y;(void)c;(void)s;return vnt_bool(0); }
VntValue *vnt_gui_checkbox(VntValue *label,VntValue *x,VntValue *y,VntValue *checked) { (void)label;(void)x;(void)y;(void)checked;return vnt_bool(0); }
VntValue *vnt_gui_progress(VntValue *v,VntValue *m,VntValue *x,VntValue *y) { (void)v;(void)m;(void)x;(void)y;return vnt_bool(0); }
VntValue *vnt_gui_separator(VntValue *x,VntValue *y,VntValue *w) { (void)x;(void)y;(void)w;return vnt_bool(0); }
VntValue *vnt_gui_present(void) { return vnt_bool(0); }
VntValue *vnt_gui_poll(void) { return vnt_bool(0); }
VntValue *vnt_gui_key(void) { return vnt_int(0); }
VntValue *vnt_gui_close(void) { return vnt_bool(1); }
#endif
