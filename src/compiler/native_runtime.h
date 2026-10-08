#ifndef VNT_NATIVE_RUNTIME_H
#define VNT_NATIVE_RUNTIME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

typedef enum { V_INVALID, V_STRING, V_INTEGER, V_BOOLEAN, V_ARRAY } VntType;
typedef struct VntValue VntValue;

struct VntValue {
    VntType type;
    union {
        char *string;
        int32_t integer;
        int boolean;
        struct {
            VntValue *items;
            int count;
            int capacity;
        } array;
    };
};

static void vnt_error(const char *message) {
    fprintf(stderr, "Runtime error: %s\n", message);
    exit(1);
}

static VntValue vnt_invalid(void) {
    VntValue v = {0};
    return v;
}

static VntValue vnt_int(int32_t x) {
    VntValue v = {0}; v.type = V_INTEGER; v.integer = x; return v;
}

static VntValue vnt_bool(int x) {
    VntValue v = {0}; v.type = V_BOOLEAN; v.boolean = !!x; return v;
}

static VntValue vnt_string(const char *s) {
    VntValue v = {0};
    size_t n = strlen(s ? s : "");
    v.type = V_STRING;
    v.string = malloc(n + 1);
    if (!v.string) vnt_error("out of memory.");
    memcpy(v.string, s ? s : "", n + 1);
    return v;
}

static VntValue vnt_array(void) {
    VntValue v = {0};
    v.type = V_ARRAY;
    return v;
}

static void vnt_append(VntValue *a, VntValue value) {
    if (a->array.count == a->array.capacity) {
        int cap = a->array.capacity ? a->array.capacity * 2 : 8;
        VntValue *items = realloc(a->array.items, sizeof(VntValue) * cap);
        if (!items) vnt_error("out of memory.");
        a->array.items = items;
        a->array.capacity = cap;
    }
    a->array.items[a->array.count++] = value;
}

static VntValue vnt_array_from(VntValue *items, int count) {
    VntValue result = vnt_array();
    for (int i = 0; i < count; ++i) vnt_append(&result, items[i]);
    return result;
}

static VntValue vnt_get(VntValue array, VntValue index) {
    if (array.type != V_ARRAY || index.type != V_INTEGER)
        vnt_error("indexing requires an array and integer index.");
    if (index.integer < 0 || index.integer >= array.array.count)
        vnt_error("array index out of bounds.");
    return array.array.items[index.integer];
}

static void vnt_set(VntValue *array, VntValue index, VntValue value) {
    if (array->type != V_ARRAY || index.type != V_INTEGER)
        vnt_error("indexing requires an array and integer index.");
    if (index.integer < 0 || index.integer >= array->array.count)
        vnt_error("array index out of bounds.");
    array->array.items[index.integer] = value;
}

static int vnt_truth(VntValue value) {
    if (value.type != V_BOOLEAN)
        vnt_error("condition requires a boolean.");
    return value.boolean;
}

static VntValue vnt_neg(VntValue value) {
    if (value.type != V_INTEGER) vnt_error("unary '-' requires an integer.");
    if (value.integer == INT32_MIN) vnt_error("integer overflow in unary '-'.");
    return vnt_int(-value.integer);
}

static VntValue vnt_add(VntValue a, VntValue b) {
    if (a.type == V_STRING && b.type == V_STRING) {
        size_t x = strlen(a.string ? a.string : "");
        size_t y = strlen(b.string ? b.string : "");
        char *s = malloc(x + y + 1);
        if (!s) vnt_error("out of memory.");
        memcpy(s, a.string ? a.string : "", x);
        memcpy(s + x, b.string ? b.string : "", y + 1);
        VntValue result = vnt_string(s);
        free(s);
        return result;
    }
    if (a.type != V_INTEGER || b.type != V_INTEGER)
        vnt_error("arithmetic requires integers.");
    return vnt_int((int32_t)((int64_t)a.integer + b.integer));
}

static VntValue vnt_sub(VntValue a, VntValue b) {
    if (a.type != V_INTEGER || b.type != V_INTEGER)
        vnt_error("arithmetic requires integers.");
    return vnt_int((int32_t)((int64_t)a.integer - b.integer));
}

static VntValue vnt_mul(VntValue a, VntValue b) {
    if (a.type != V_INTEGER || b.type != V_INTEGER)
        vnt_error("arithmetic requires integers.");
    return vnt_int((int32_t)((int64_t)a.integer * b.integer));
}

static VntValue vnt_div(VntValue a, VntValue b) {
    if (a.type != V_INTEGER || b.type != V_INTEGER)
        vnt_error("arithmetic requires integers.");
    if (b.integer == 0) vnt_error("division by zero.");
    if (a.integer == INT32_MIN && b.integer == -1)
        return vnt_int(INT32_MIN);
    return vnt_int(a.integer / b.integer);
}

static VntValue vnt_mod_value(VntValue a, VntValue b) {
    if (a.type != V_INTEGER || b.type != V_INTEGER)
        vnt_error("modulo requires integers.");
    if (b.integer == 0) vnt_error("modulo by zero.");
    if (a.integer == INT32_MIN && b.integer == -1)
        return vnt_int(0);
    return vnt_int(a.integer % b.integer);
}

static int vnt_equal(VntValue a, VntValue b) {
    if (a.type != b.type) return 0;
    switch (a.type) {
        case V_INTEGER: return a.integer == b.integer;
        case V_BOOLEAN: return a.boolean == b.boolean;
        case V_STRING:
            return strcmp(a.string ? a.string : "",
                          b.string ? b.string : "") == 0;
        case V_ARRAY:
            if (a.array.count != b.array.count) return 0;
            for (int i = 0; i < a.array.count; ++i)
                if (!vnt_equal(a.array.items[i], b.array.items[i])) return 0;
            return 1;
        default: return 0;
    }
}

static VntValue vnt_eq(VntValue a, VntValue b) { return vnt_bool(vnt_equal(a,b)); }
static VntValue vnt_ne(VntValue a, VntValue b) { return vnt_bool(!vnt_equal(a,b)); }

static VntValue vnt_compare(VntValue a, VntValue b, int op) {
    if (a.type != V_INTEGER || b.type != V_INTEGER)
        vnt_error("ordered comparisons require integers.");
    int r = op == 0 ? a.integer > b.integer :
            op == 1 ? a.integer < b.integer :
            op == 2 ? a.integer >= b.integer :
                      a.integer <= b.integer;
    return vnt_bool(r);
}

static VntValue vnt_gt(VntValue a,VntValue b){return vnt_compare(a,b,0);}
static VntValue vnt_lt(VntValue a,VntValue b){return vnt_compare(a,b,1);}
static VntValue vnt_ge(VntValue a,VntValue b){return vnt_compare(a,b,2);}
static VntValue vnt_le(VntValue a,VntValue b){return vnt_compare(a,b,3);}

static void vnt_print_inner(VntValue value) {
    switch (value.type) {
        case V_STRING: printf("%s", value.string ? value.string : ""); break;
        case V_INTEGER: printf("%d", value.integer); break;
        case V_BOOLEAN: printf("%s", value.boolean ? "true" : "false"); break;
        case V_ARRAY:
            putchar('[');
            for (int i=0;i<value.array.count;i++) {
                if (i) printf(", ");
                vnt_print_inner(value.array.items[i]);
            }
            putchar(']');
            break;
        default: break;
    }
}

static void vnt_print(VntValue value) {
    vnt_print_inner(value);
    putchar('\n');
}

static VntValue vnt_len(VntValue value) {
    if (value.type == V_STRING)
        return vnt_int((int32_t)strlen(value.string ? value.string : ""));
    if (value.type == V_ARRAY)
        return vnt_int(value.array.count);
    vnt_error("len() requires a string or array.");
    return vnt_invalid();
}

static VntValue vnt_mod(VntValue a, VntValue b) {
    return vnt_mod_value(a,b);
}

static VntValue vnt_input(VntValue prompt) {
    if (prompt.type != V_STRING)
        vnt_error("input() requires a string prompt.");
    printf("%s", prompt.string ? prompt.string : "");
    fflush(stdout);
    char buffer[4096];
    if (!fgets(buffer, sizeof(buffer), stdin))
        vnt_error("failed to read input.");
    buffer[strcspn(buffer, "\r\n")] = '\0';
    return vnt_string(buffer);
}

static VntValue vnt_range1(VntValue end) {
    if (end.type != V_INTEGER) vnt_error("range() requires integer arguments.");
    VntValue result = vnt_array();
    for (int64_t i=0;i<end.integer;i++) vnt_append(&result,vnt_int((int32_t)i));
    return result;
}

static VntValue vnt_range2(VntValue start,VntValue end) {
    if (start.type != V_INTEGER || end.type != V_INTEGER)
        vnt_error("range() requires integer arguments.");
    VntValue result=vnt_array();
    for (int64_t i=start.integer;i<end.integer;i++) vnt_append(&result,vnt_int((int32_t)i));
    return result;
}

static VntValue vnt_range3(VntValue start,VntValue end,VntValue step) {
    if (start.type != V_INTEGER || end.type != V_INTEGER || step.type != V_INTEGER)
        vnt_error("range() requires integer arguments.");
    if (step.integer == 0) vnt_error("range() step cannot be zero.");
    VntValue result=vnt_array();
    if (step.integer > 0)
        for (int64_t i=start.integer;i<end.integer;i+=step.integer)
            vnt_append(&result,vnt_int((int32_t)i));
    else
        for (int64_t i=start.integer;i>end.integer;i+=step.integer)
            vnt_append(&result,vnt_int((int32_t)i));
    return result;
}

#endif
