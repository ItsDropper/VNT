#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <errno.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

#if defined(_WIN32)
/* Windows x64 already uses the ABI emitted by the native backend. */
#define VNT_NATIVE_ABI
#elif defined(__x86_64__) && defined(__GNUC__)
/* The current x86-64 backend emits Microsoft x64 call sequences on every host.
   Keep the runtime boundary compatible on ELF/SysV hosts without changing
   libc calls made inside this C runtime. */
#define VNT_NATIVE_ABI __attribute__((ms_abi))
#else
#define VNT_NATIVE_ABI
#endif

#ifdef _WIN32
#include <direct.h>
#include <io.h>

/*
 * Diagnostic crash log for compiled native VNT applications.
 * The log is written beside the process working directory as vnt_crash.log.
 * This handler intentionally avoids heap allocation and symbol APIs so it
 * remains useful for access violations in the runtime itself.
 */
static LONG WINAPI vnt_unhandled_exception(EXCEPTION_POINTERS *info) {
    FILE *f = fopen("vnt_crash.log", "a");
    if (f) {
        DWORD code = 0;
        void *address = NULL;
        if (info && info->ExceptionRecord) {
            code = info->ExceptionRecord->ExceptionCode;
            address = info->ExceptionRecord->ExceptionAddress;
        }
        fprintf(f, "VNT native crash report\n");
        fprintf(f, "Process ID: %lu\nThread ID: %lu\n",
                (unsigned long)GetCurrentProcessId(),
                (unsigned long)GetCurrentThreadId());
        fprintf(f, "Exception code: 0x%08lX\nException address: %p\n",
                (unsigned long)code, address);
        char module[MAX_PATH] = {0};
        HMODULE image_base = GetModuleHandleA(NULL);
        if (GetModuleFileNameA(NULL, module, MAX_PATH))
            fprintf(f, "Executable: %s\n", module);
        fprintf(f, "Module base: %p\n", (void *)image_base);
        if (image_base && address)
            fprintf(f, "Fault RVA: 0x%llX\n",
                    (unsigned long long)((uintptr_t)address - (uintptr_t)image_base));
#if defined(_M_X64) || defined(__x86_64__)
        if (info && info->ContextRecord) {
            CONTEXT *x = info->ContextRecord;
            fprintf(f, "RIP=%016llX RSP=%016llX RBP=%016llX\n",
                    (unsigned long long)x->Rip, (unsigned long long)x->Rsp,
                    (unsigned long long)x->Rbp);
            fprintf(f, "RAX=%016llX RBX=%016llX RCX=%016llX RDX=%016llX\n",
                    (unsigned long long)x->Rax, (unsigned long long)x->Rbx,
                    (unsigned long long)x->Rcx, (unsigned long long)x->Rdx);
            fprintf(f, "RSI=%016llX RDI=%016llX R8 =%016llX R9 =%016llX\n",
                    (unsigned long long)x->Rsi, (unsigned long long)x->Rdi,
                    (unsigned long long)x->R8, (unsigned long long)x->R9);
            fprintf(f, "R10=%016llX R11=%016llX R12=%016llX R13=%016llX\n",
                    (unsigned long long)x->R10, (unsigned long long)x->R11,
                    (unsigned long long)x->R12, (unsigned long long)x->R13);
            fprintf(f, "R14=%016llX R15=%016llX\n",
                    (unsigned long long)x->R14, (unsigned long long)x->R15);
        }
#elif defined(_M_IX86) || defined(__i386__)
        if (info && info->ContextRecord) {
            CONTEXT *x = info->ContextRecord;
            fprintf(f, "EIP=%08lX ESP=%08lX EBP=%08lX\n",
                    (unsigned long)x->Eip, (unsigned long)x->Esp,
                    (unsigned long)x->Ebp);
            fprintf(f, "EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX\n",
                    (unsigned long)x->Eax, (unsigned long)x->Ebx,
                    (unsigned long)x->Ecx, (unsigned long)x->Edx);
        }
#endif
        if (info && info->ExceptionRecord) {
            fprintf(f, "Exception flags: 0x%08lX\n",
                    (unsigned long)info->ExceptionRecord->ExceptionFlags);
            if (info->ExceptionRecord->NumberParameters > 0)
                fprintf(f, "Exception detail[0]: 0x%llX\n",
                        (unsigned long long)info->ExceptionRecord->ExceptionInformation[0]);
            if (info->ExceptionRecord->NumberParameters > 1)
                fprintf(f, "Exception detail[1]: 0x%llX\n",
                        (unsigned long long)info->ExceptionRecord->ExceptionInformation[1]);
        }
        fflush(f);
        fclose(f);
    }
    return EXCEPTION_EXECUTE_HANDLER;
}

static LONG CALLBACK vnt_first_chance_exception(EXCEPTION_POINTERS *info) {
    if (!info || !info->ExceptionRecord) return EXCEPTION_CONTINUE_SEARCH;
    DWORD code = info->ExceptionRecord->ExceptionCode;
    /* Log likely fatal native faults; leave normal exception handling intact. */
    if (code != EXCEPTION_ACCESS_VIOLATION &&
        code != EXCEPTION_ILLEGAL_INSTRUCTION &&
        code != EXCEPTION_STACK_OVERFLOW &&
        code != EXCEPTION_ARRAY_BOUNDS_EXCEEDED &&
        code != EXCEPTION_INT_DIVIDE_BY_ZERO)
        return EXCEPTION_CONTINUE_SEARCH;

    FILE *f = fopen("vnt_crash.log", "a");
    if (f) {
        void *fault_address = info->ExceptionRecord->ExceptionAddress;
        HMODULE image_base = GetModuleHandleA(NULL);
        fprintf(f, "First-chance exception: 0x%08lX at %p; thread=%lu\n",
                (unsigned long)code, fault_address,
                (unsigned long)GetCurrentThreadId());
        fprintf(f, "Module base: %p\n", (void *)image_base);
        if (image_base && fault_address)
            fprintf(f, "Fault RVA: 0x%llX\n",
                    (unsigned long long)((uintptr_t)fault_address - (uintptr_t)image_base));
        if (info->ExceptionRecord->NumberParameters > 0)
            fprintf(f, "Exception detail[0]: 0x%llX\n",
                    (unsigned long long)info->ExceptionRecord->ExceptionInformation[0]);
        if (info->ExceptionRecord->NumberParameters > 1)
            fprintf(f, "Exception detail[1]: 0x%llX\n",
                    (unsigned long long)info->ExceptionRecord->ExceptionInformation[1]);
#if defined(_M_X64) || defined(__x86_64__)
        if (info->ContextRecord) {
            CONTEXT *x = info->ContextRecord;
            fprintf(f, "RIP=%016llX RSP=%016llX RBP=%016llX RCX=%016llX RDX=%016llX R8=%016llX R9=%016llX\n",
                    (unsigned long long)x->Rip, (unsigned long long)x->Rsp,
                    (unsigned long long)x->Rbp, (unsigned long long)x->Rcx,
                    (unsigned long long)x->Rdx, (unsigned long long)x->R8,
                    (unsigned long long)x->R9);
        }
#endif
        fflush(f);
        fclose(f);
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static VNT_NATIVE_ABI void vnt_install_crash_handler(void) {
    FILE *f = fopen("vnt_crash.log", "w");
    if (f) {
        fprintf(f, "VNT crash logger initialized. PID=%lu\n",
                (unsigned long)GetCurrentProcessId());
        fclose(f);
    }
    AddVectoredExceptionHandler(1, vnt_first_chance_exception);
    SetUnhandledExceptionFilter(vnt_unhandled_exception);
}
#if defined(__GNUC__)
__attribute__((constructor))
#endif
static VNT_NATIVE_ABI void vnt_runtime_initialize(void) {
    vnt_install_crash_handler();
}
#endif

typedef enum {
    VNT_NULL,
    VNT_INT,
    VNT_FLOAT,
    VNT_BOOL,
    VNT_STRING,
    VNT_ARRAY,
    VNT_BYTES,
    VNT_OBJECT,
    VNT_PROCESS,
    VNT_REFERENCE,
    VNT_POINTER
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
        int64_t integer;
        double floating;
        int boolean;
        char *string;
        struct {
            VntValue **items;
            int count;
            int capacity;
        } array;
        struct {
            unsigned char *data;
            size_t length;
        } bytes;
        struct {
            VntField *fields;
            int count;
            int capacity;
            char *type_name;
        } object;
        VntProcess *process;
        void *pointer;
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

VNT_NATIVE_ABI VntValue * vnt_string_get(VntValue *s, VntValue *index);

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

VNT_NATIVE_ABI VntValue * vnt_null(void) { return alloc_value(VNT_NULL); }

VNT_NATIVE_ABI VntValue * vnt_ref(VntValue **slot) {
    if (!slot) {
        fprintf(stderr, "Runtime error: cannot reference a null slot.\n");
        exit(1);
    }
    VntValue *v = alloc_value(VNT_REFERENCE);
    v->reference.slot = slot;
    return v;
}

VNT_NATIVE_ABI VntValue * vnt_deref(VntValue *ref) {
    if (!ref || ref->type != VNT_REFERENCE || !ref->reference.slot) {
        fprintf(stderr, "Runtime error: dereference requires a reference.\n");
        exit(1);
    }
    return *ref->reference.slot;
}

VNT_NATIVE_ABI VntValue * vnt_ref_set(VntValue *ref, VntValue *value) {
    if (!ref || ref->type != VNT_REFERENCE || !ref->reference.slot) {
        fprintf(stderr, "Runtime error: assignment requires a reference.\n");
        exit(1);
    }
    *ref->reference.slot = value;
    return value;
}

VNT_NATIVE_ABI VntValue * vnt_int(int64_t x) {
    VntValue *v = alloc_value(VNT_INT);
    v->integer = x;
    return v;
}

/* Error exits used by generated native integer fast paths on overflow. */
VNT_NATIVE_ABI void vnt_int_add_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in addition.\n");
    exit(1);
}
VNT_NATIVE_ABI void vnt_int_sub_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in subtraction.\n");
    exit(1);
}
VNT_NATIVE_ABI void vnt_int_mul_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in multiplication.\n");
    exit(1);
}
VNT_NATIVE_ABI void vnt_int_neg_overflow(void) {
    fprintf(stderr, "Runtime error: integer overflow in unary '-'.\n");
    exit(1);
}

VNT_NATIVE_ABI VntValue * vnt_float(double x) {
    VntValue *v = alloc_value(VNT_FLOAT);
    v->floating = x;
    return v;
}

VNT_NATIVE_ABI VntValue * vnt_bool(int x) {
    VntValue *v = alloc_value(VNT_BOOL);
    v->boolean = x ? 1 : 0;
    return v;
}

VNT_NATIVE_ABI VntValue * vnt_string(const char *s) {
    VntValue *v = alloc_value(VNT_STRING);
    v->string = strdup(s ? s : "");
    if (!v->string) {
        fprintf(stderr, "Runtime error: out of memory.\n");
        exit(1);
    }
    return v;
}

VNT_NATIVE_ABI VntValue * vnt_array_new(void) {
    return alloc_value(VNT_ARRAY);
}

VNT_NATIVE_ABI VntValue * vnt_bytes_new(const unsigned char *data, size_t length) {
    VntValue *v = alloc_value(VNT_BYTES);
    v->bytes.data = (unsigned char *)malloc(length ? length : 1);
    if (!v->bytes.data) {
        fprintf(stderr, "Runtime error: out of memory.\n");
        exit(1);
    }
    if (length && data) memcpy(v->bytes.data, data, length);
    else if (length) memset(v->bytes.data, 0, length);
    v->bytes.length = length;
    return v;
}


VNT_NATIVE_ABI VntValue * vnt_array_push(VntValue *a, VntValue *item) {
    if (!a || a->type != VNT_ARRAY) {
        fprintf(stderr, "Runtime error: expected an array.\n");
        exit(1);
    }
    if (a->array.count == a->array.capacity) {
        if (a->array.capacity > INT_MAX / 2) {
            fprintf(stderr, "Runtime error: array capacity limit exceeded.\n");
            exit(1);
        }
        int cap = a->array.capacity ? a->array.capacity * 2 : 4;
        if ((size_t)cap > SIZE_MAX / sizeof(*a->array.items)) {
            fprintf(stderr, "Runtime error: array allocation size overflow.\n");
            exit(1);
        }
        VntValue **items = realloc(a->array.items, sizeof(*items) * (size_t)cap);
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

VNT_NATIVE_ABI VntValue * vnt_array_get_int(VntValue *a, int64_t index) {
    if (a && a->type == VNT_BYTES) {
        if (index < 0 || (uint64_t)index >= a->bytes.length) {
            fprintf(stderr, "Runtime error: byte buffer index out of bounds.\n");
            exit(1);
        }
        return vnt_int((int64_t)a->bytes.data[(size_t)index]);
    }
    if (!a || a->type != VNT_ARRAY) {
        fprintf(stderr, "Runtime error: indexing requires an array or byte buffer.\n");
        exit(1);
    }
    if (index < 0 || index >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    return a->array.items[index];
}

VNT_NATIVE_ABI VntValue * vnt_array_set_int(VntValue *a, int64_t index, VntValue *value) {
    if (a && a->type == VNT_BYTES) {
        if (index < 0 || (size_t)index >= a->bytes.length || !value ||
            value->type != VNT_INT || value->integer < 0 || value->integer > 255) {
            fprintf(stderr, "Runtime error: byte buffer assignment requires an in-range index and byte integer.\n");
            exit(1);
        }
        a->bytes.data[(size_t)index] = (unsigned char)value->integer;
        return value;
    }
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

VNT_NATIVE_ABI VntValue * vnt_array_get(VntValue *a, VntValue *index) {
    if (a && a->type == VNT_BYTES) {
        if (!index || index->type != VNT_INT) {
            fprintf(stderr, "Runtime error: byte buffer indexing requires an integer index.\n");
            exit(1);
        }
        return vnt_array_get_int(a, index->integer);
    }
    if (a && a->type == VNT_STRING)
        return vnt_string_get(a, index);
    if (!a || a->type != VNT_ARRAY || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: indexing requires an array/string and integer index.\n");
        exit(1);
    }
    int64_t i = index->integer;
    if (i < 0 || i >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    return a->array.items[(int)i];
}

VNT_NATIVE_ABI VntValue * vnt_array_set(VntValue *a, VntValue *index, VntValue *value) {
    if (a && a->type == VNT_BYTES) {
        if (!index || index->type != VNT_INT) {
            fprintf(stderr, "Runtime error: byte buffer assignment requires an integer index.\n");
            exit(1);
        }
        return vnt_array_set_int(a, index->integer, value);
    }
    if (!a || a->type != VNT_ARRAY || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: array assignment requires an array and integer index.\n");
        exit(1);
    }
    int64_t i = index->integer;
    if (i < 0 || i >= a->array.count) {
        fprintf(stderr, "Runtime error: array index out of bounds.\n");
        exit(1);
    }
    a->array.items[(int)i] = value;
    return value;
}

VNT_NATIVE_ABI VntValue * vnt_string_get(VntValue *s, VntValue *index) {
    if (!s || s->type != VNT_STRING || !index || index->type != VNT_INT) {
        fprintf(stderr, "Runtime error: string indexing requires a string and integer index.\n");
        exit(1);
    }
    int64_t i = index->integer;
    size_t len = strlen(s->string);
    if (i < 0 || (uint64_t)i >= len) {
        fprintf(stderr, "Runtime error: string index out of bounds.\n");
        exit(1);
    }
    char out[2] = { s->string[(size_t)i], '\0' };
    return vnt_string(out);
}

static char *value_to_concat_string(VntValue *v) {
    char buffer[64];
    const char *text = NULL;
    if (!v) return strdup("null");
    switch (v->type) {
        case VNT_STRING: text = v->string; break;
        case VNT_INT: snprintf(buffer, sizeof(buffer), "%lld", (long long)v->integer); text = buffer; break;
        case VNT_FLOAT: snprintf(buffer, sizeof(buffer), "%.15g", v->floating); text = buffer; break;
        case VNT_BOOL: text = v->boolean ? "true" : "false"; break;
        default: return NULL;
    }
    return strdup(text ? text : "");
}

VNT_NATIVE_ABI VntValue * vnt_add(VntValue *a, VntValue *b) {
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
        if (la > SIZE_MAX - lb - 1) {
            free(left);
            free(right);
            fprintf(stderr, "Runtime error: string concatenation size overflow.\n");
            exit(1);
        }
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
    int64_t result;
    if (__builtin_add_overflow(a->integer, b->integer, &result)) {
        fprintf(stderr, "Runtime error: signed 64-bit integer overflow in addition.\n");
        exit(1);
    }
    return vnt_int(result);
}
VNT_NATIVE_ABI VntValue * vnt_sub(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) - number_value(b));
    int64_t result;
    if (__builtin_sub_overflow(a->integer, b->integer, &result)) {
        fprintf(stderr, "Runtime error: signed 64-bit integer overflow in subtraction.\n");
        exit(1);
    }
    return vnt_int(result);
}

VNT_NATIVE_ABI VntValue * vnt_mul(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) * number_value(b));
    int64_t result;
    if (__builtin_mul_overflow(a->integer, b->integer, &result)) {
        fprintf(stderr, "Runtime error: signed 64-bit integer overflow in multiplication.\n");
        exit(1);
    }
    return vnt_int(result);
}

VNT_NATIVE_ABI VntValue * vnt_div(VntValue *a, VntValue *b) {
    require_number(a,b);
    if (number_value(b) == 0.0) {
        fprintf(stderr, "Runtime error: division by zero.\n");
        exit(1);
    }
    if (a->type == VNT_FLOAT || b->type == VNT_FLOAT)
        return vnt_float(number_value(a) / number_value(b));
    if (a->integer == INT64_MIN && b->integer == -1) {
        fprintf(stderr, "Runtime error: integer overflow in division.\n");
        exit(1);
    }
    return vnt_int(a->integer / b->integer);
}

VNT_NATIVE_ABI VntValue * vnt_mod(VntValue *a, VntValue *b) {
    if (!a || !b || a->type != VNT_INT || b->type != VNT_INT) {
        fprintf(stderr, "Runtime error: modulo requires integers.\n");
        exit(1);
    }
    if (!b->integer) {
        fprintf(stderr, "Runtime error: modulo by zero.\n");
        exit(1);
    }
    if (a->integer == INT64_MIN && b->integer == -1) {
        fprintf(stderr, "Runtime error: integer overflow in modulo.\n");
        exit(1);
    }
    return vnt_int(a->integer % b->integer);
}

VNT_NATIVE_ABI VntValue * vnt_neg(VntValue *a) {
    require_number(a, a);
    if (a->type == VNT_FLOAT) return vnt_float(-a->floating);
    if (a->integer == INT64_MIN) {
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
        case VNT_BYTES:
            return a->bytes.length == b->bytes.length &&
                   (!a->bytes.length || memcmp(a->bytes.data, b->bytes.data, a->bytes.length) == 0);
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

VNT_NATIVE_ABI VntValue * vnt_eq(VntValue *a, VntValue *b) { return vnt_bool(equal_value(a,b)); }
VNT_NATIVE_ABI VntValue * vnt_ne(VntValue *a, VntValue *b) { return vnt_bool(!equal_value(a,b)); }

static int order(VntValue *a, VntValue *b, int op) {
    require_number(a,b);
    if (a->type == VNT_INT && b->type == VNT_INT) {
        if (op == 0) return a->integer > b->integer;
        if (op == 1) return a->integer < b->integer;
        if (op == 2) return a->integer >= b->integer;
        return a->integer <= b->integer;
    }
    double x=number_value(a), y=number_value(b);
    if (op == 0) return x > y;
    if (op == 1) return x < y;
    if (op == 2) return x >= y;
    return x <= y;
}

VNT_NATIVE_ABI VntValue * vnt_gt(VntValue *a,VntValue *b){return vnt_bool(order(a,b,0));}
VNT_NATIVE_ABI VntValue * vnt_lt(VntValue *a,VntValue *b){return vnt_bool(order(a,b,1));}
VNT_NATIVE_ABI VntValue * vnt_ge(VntValue *a,VntValue *b){return vnt_bool(order(a,b,2));}
VNT_NATIVE_ABI VntValue * vnt_le(VntValue *a,VntValue *b){return vnt_bool(order(a,b,3));}

VNT_NATIVE_ABI VntValue * vnt_not(VntValue *a) {
    if (!a || a->type != VNT_BOOL) {
        fprintf(stderr, "Runtime error: '!' requires a boolean.\n");
        exit(1);
    }
    return vnt_bool(!a->boolean);
}

VNT_NATIVE_ABI int vnt_truth(VntValue *a) {
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
        case VNT_INT: printf("%lld", (long long)v->integer); break;
        case VNT_FLOAT: printf("%.15g", v->floating); break;
        case VNT_BOOL: printf("%s", v->boolean ? "true" : "false"); break;
        case VNT_STRING: printf("%s", v->string); break;
        case VNT_BYTES:
            printf("[");
            for (size_t i = 0; i < v->bytes.length; ++i) {
                if (i) printf(", ");
                printf("%u", (unsigned int)v->bytes.data[i]);
            }
            printf("]");
            break;
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

VNT_NATIVE_ABI void vnt_print(VntValue *v) {
    print_value(v);
    putchar('\n');
    fflush(stdout);
}

/* UTF-8 helpers count Unicode scalar encodings, not grapheme clusters. Invalid
   byte sequences are treated as one-byte units so slicing always progresses. */
static VNT_NATIVE_ABI const char *vnt_app_string(VntValue *v,const char *fn);
static size_t vnt_utf8_unit(const unsigned char *p) {
    if (p[0] < 0x80) return 1;
    if (p[0] >= 0xC2 && p[0] <= 0xDF &&
        p[1] && (p[1] & 0xC0) == 0x80) return 2;
    if (p[0] >= 0xE0 && p[0] <= 0xEF &&
        p[1] && p[2] &&
        (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80 &&
        !(p[0] == 0xE0 && p[1] < 0xA0) &&
        !(p[0] == 0xED && p[1] >= 0xA0)) return 3;
    if (p[0] >= 0xF0 && p[0] <= 0xF4 &&
        p[1] && p[2] && p[3] &&
        (p[1] & 0xC0) == 0x80 && (p[2] & 0xC0) == 0x80 &&
        (p[3] & 0xC0) == 0x80 &&
        !(p[0] == 0xF0 && p[1] < 0x90) &&
        !(p[0] == 0xF4 && p[1] >= 0x90)) return 4;
    return 1;
}
VNT_NATIVE_ABI VntValue *vnt_utf8_len(VntValue *value) {
    const unsigned char *p=(const unsigned char *)vnt_app_string(value,"utf8_len()");
    int64_t count=0;
    while (*p) { size_t n=vnt_utf8_unit(p); p+=n; if(count==INT64_MAX) return vnt_int(count); count++; }
    return vnt_int(count);
}
VNT_NATIVE_ABI VntValue *vnt_utf8_slice(VntValue *value,VntValue *start_value,VntValue *count_value) {
    const unsigned char *source=(const unsigned char *)vnt_app_string(value,"utf8_slice()");
    if(!start_value||start_value->type!=VNT_INT||!count_value||count_value->type!=VNT_INT||
       start_value->integer<0||count_value->integer<0) {
        fprintf(stderr,"Runtime error: utf8_slice() expects non-negative integer start and count.\\n"); exit(1);
    }
    size_t begin=0,end=0,index=0,byte_begin=0,byte_end=0;
    size_t start=(size_t)start_value->integer, count=(size_t)count_value->integer;
    while(source[begin] && index<start) { begin+=vnt_utf8_unit(source+begin); index++; }
    if(index<start) return vnt_string("");
    byte_begin=begin; index=0;
    while(source[end] && index<count) { end+=vnt_utf8_unit(source+end); index++; }
    /* The second walk begins at byte zero; use a code-point target to avoid
       assuming that the requested slice ends before the string does. */
    end=byte_begin;
    while(source[end] && index>0) { end+=vnt_utf8_unit(source+end); index--; }
    byte_end=end;
    char *out=(char *)malloc(byte_end-byte_begin+1);
    if(!out){fprintf(stderr,"Runtime error: out of memory in utf8_slice().\\n");exit(1);}
    memcpy(out,source+byte_begin,byte_end-byte_begin);out[byte_end-byte_begin]=0;
    VntValue *result=vnt_string(out);free(out);return result;
}

VNT_NATIVE_ABI VntValue * vnt_len(VntValue *v) {
    if (v->type == VNT_STRING) return vnt_int((int64_t)strlen(v->string));
    if (v->type == VNT_ARRAY) return vnt_int(v->array.count);
    if (v->type == VNT_BYTES) {
        if (v->bytes.length > (uint64_t)INT64_MAX) {
            fprintf(stderr, "Runtime error: byte buffer is too large for len().\n");
            exit(1);
        }
        return vnt_int((int64_t)v->bytes.length);
    }
    if (v->type == VNT_OBJECT) return vnt_int(v->object.count);
    fprintf(stderr, "Runtime error: len() requires a string, array, or object.\n");
    exit(1);
}

VNT_NATIVE_ABI VntValue * vnt_input(VntValue *prompt) {
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

/* Exit the current native VNT program with a conventional process status. */
VNT_NATIVE_ABI VntValue * vnt_process_exit(VntValue *code) {
    if (!code || code->type != VNT_INT) {
        fprintf(stderr, "Runtime error: process_exit() requires an integer status.\n");
        exit(1);
    }
    int status = (int)code->integer;
    if (status < 0 || status > 255) {
        fprintf(stderr, "Runtime error: process_exit() status must be between 0 and 255.\n");
        exit(1);
    }
    exit(status);
    return vnt_null();
}

VNT_NATIVE_ABI VntValue * vnt_object_new(void) {
    return alloc_value(VNT_OBJECT);
}

VNT_NATIVE_ABI VntValue * vnt_struct_new(const char *name) {
    VntValue *v = alloc_value(VNT_OBJECT);
    v->object.type_name = strdup(name ? name : "");
    if (!v->object.type_name) {
        fprintf(stderr, "Runtime error: out of memory.\n");
        exit(1);
    }
    return v;
}

VNT_NATIVE_ABI VntValue * vnt_object_get(VntValue *object, const char *key) {
    if (!object || object->type != VNT_OBJECT || !key) {
        fprintf(stderr, "Runtime error: member access requires an object and non-null key.\n");
        exit(1);
    }
    for (int i=0;i<object->object.count;i++)
        if (!strcmp(object->object.fields[i].key,key))
            return object->object.fields[i].value;
    return vnt_null();
}

VNT_NATIVE_ABI VntValue * vnt_object_set(VntValue *object, const char *key, VntValue *value) {
    if (!object || object->type != VNT_OBJECT || !key) {
        fprintf(stderr, "Runtime error: member assignment requires an object and non-null key.\n");
        exit(1);
    }
    for (int i=0;i<object->object.count;i++) {
        if (!strcmp(object->object.fields[i].key,key)) {
            object->object.fields[i].value=value;
            return value;
        }
    }
    if (object->object.count == object->object.capacity) {
        if (object->object.capacity > INT_MAX / 2) {
            fprintf(stderr, "Runtime error: object capacity limit exceeded.\n");
            exit(1);
        }
        int cap=object->object.capacity ? object->object.capacity*2 : 4;
        if ((size_t)cap > SIZE_MAX / sizeof(*object->object.fields)) {
            fprintf(stderr, "Runtime error: object allocation size overflow.\n");
            exit(1);
        }
        VntField *fields=realloc(object->object.fields,sizeof(*fields)*(size_t)cap);
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

VNT_NATIVE_ABI VntValue * vnt_sqrt(VntValue *x) { require_number(x,x); return vnt_float(sqrt(number_value(x))); }
VNT_NATIVE_ABI VntValue * vnt_sin(VntValue *x) { require_number(x,x); return vnt_float(sin(number_value(x))); }
VNT_NATIVE_ABI VntValue * vnt_cos(VntValue *x) { require_number(x,x); return vnt_float(cos(number_value(x))); }
VNT_NATIVE_ABI VntValue * vnt_tan(VntValue *x) { require_number(x,x); return vnt_float(tan(number_value(x))); }
VNT_NATIVE_ABI VntValue * vnt_abs(VntValue *x) { require_number(x,x); if (x->type==VNT_FLOAT) return vnt_float(fabs(x->floating)); if (x->integer==INT64_MIN) { fprintf(stderr,"Runtime error: integer overflow in abs().\n"); exit(1); } return vnt_int(x->integer<0?-x->integer:x->integer); }
VNT_NATIVE_ABI VntValue * vnt_floor(VntValue *x) { require_number(x,x); return vnt_float(floor(number_value(x))); }
VNT_NATIVE_ABI VntValue * vnt_ceil(VntValue *x) { require_number(x,x); return vnt_float(ceil(number_value(x))); }
VNT_NATIVE_ABI VntValue * vnt_min(VntValue *a,VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_INT && b->type == VNT_INT)
        return a->integer < b->integer ? a : b;
    return number_value(a) < number_value(b) ? a : b;
}
VNT_NATIVE_ABI VntValue * vnt_max(VntValue *a,VntValue *b) {
    require_number(a,b);
    if (a->type == VNT_INT && b->type == VNT_INT)
        return a->integer > b->integer ? a : b;
    return number_value(a) > number_value(b) ? a : b;
}
VNT_NATIVE_ABI VntValue * vnt_range(VntValue *a, VntValue *b, VntValue *c, int argc) {
    int64_t start=0,end=0,step=1;
    if (argc==1) { if(a->type!=VNT_INT) goto bad; end=a->integer; }
    else if(argc==2) { if(a->type!=VNT_INT||b->type!=VNT_INT) goto bad; start=a->integer; end=b->integer; }
    else { if(a->type!=VNT_INT||b->type!=VNT_INT||c->type!=VNT_INT) goto bad; start=a->integer; end=b->integer; step=c->integer; }
    if (!step) { fprintf(stderr,"Runtime error: range() step cannot be zero.\n"); exit(1); }
    VntValue *r=vnt_array_new();
    if(step>0) for(int64_t i=start;i<end;) { vnt_array_push(r,vnt_int(i)); if (i > INT64_MAX-step) break; i+=step; }
    else for(int64_t i=start;i>end;) { vnt_array_push(r,vnt_int(i)); if (i < INT64_MIN-step) break; i+=step; }
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

VNT_NATIVE_ABI VntValue * vnt_ffi_int(
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
    return vnt_int((int64_t)result);
}


VNT_NATIVE_ABI void vnt_int_div_zero(void) {
    fprintf(stderr, "Runtime error: division or modulo by zero.\n");
    exit(1);
}


/* Native application API: portable file, environment, and timing calls. */
static VNT_NATIVE_ABI const char * vnt_app_string(VntValue *v, const char *fn) {
    if (!v || v->type != VNT_STRING || !v->string) {
        fprintf(stderr, "Runtime error: %s expects a string argument.\n", fn);
        exit(1);
    }
    return v->string;
}
VNT_NATIVE_ABI VntValue * vnt_fs_exists(VntValue *path) {
    const char *p = vnt_app_string(path, "fs_exists()");
#ifdef _WIN32
    return vnt_bool(GetFileAttributesA(p) != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st; return vnt_bool(stat(p, &st) == 0);
#endif
}
VNT_NATIVE_ABI VntValue * vnt_fs_read(VntValue *path) {
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
static VNT_NATIVE_ABI VntValue * vnt_fs_write_mode(VntValue *path, VntValue *contents, const char *mode, const char *fn) {
    const char *p = vnt_app_string(path, fn), *data = vnt_app_string(contents, fn);
    FILE *f = fopen(p, mode);
    if (!f) { fprintf(stderr, "Runtime error: %s could not open file.\n", fn); return vnt_bool(0); }
    size_t n = strlen(data), written = fwrite(data, 1, n, f);
    int bad = written != n || ferror(f); if (fclose(f) != 0) bad = 1;
    return vnt_bool(!bad);
}
/* Atomic text replacement: write a sibling temporary file, flush it, then
   replace the destination. This is atomic on filesystems that provide atomic
   same-directory rename; it does not promise crash-proof directory metadata. */
#ifdef _WIN32
static volatile LONG64 vnt_atomic_write_counter;
static unsigned long long vnt_next_atomic_write_sequence(void) {
    return (unsigned long long)InterlockedIncrement64(&vnt_atomic_write_counter);
}
#else
static unsigned long long vnt_atomic_write_counter;
static unsigned long long vnt_next_atomic_write_sequence(void) {
    return __atomic_add_fetch(&vnt_atomic_write_counter, 1ULL, __ATOMIC_RELAXED);
}
#endif
VNT_NATIVE_ABI VntValue *vnt_fs_write_atomic(VntValue *path_value,VntValue *contents_value) {
    const char *path=vnt_app_string(path_value,"fs_write_atomic()");
    const char *contents=vnt_app_string(contents_value,"fs_write_atomic()");
    char *temporary=NULL; FILE *file=NULL;
    for(int attempt=0;attempt<32;attempt++) {
        unsigned long long sequence=vnt_next_atomic_write_sequence();
#ifdef _WIN32
        unsigned long process_id=(unsigned long)GetCurrentProcessId();
#else
        unsigned long process_id=(unsigned long)getpid();
#endif
        size_t needed=strlen(path)+80;
        temporary=(char *)malloc(needed);
        if(!temporary){fprintf(stderr,"Runtime error: out of memory in fs_write_atomic().\n");exit(1);}
        snprintf(temporary,needed,"%s.vnt-tmp-%lu-%llu",path,process_id,sequence);
        file=fopen(temporary,"wbx");
        if(file) break;
        free(temporary);temporary=NULL;
        if(errno!=EEXIST) return vnt_bool(0);
    }
    if(!file){free(temporary);return vnt_bool(0);}
    size_t length=strlen(contents),written=fwrite(contents,1,length,file);
    int failed=written!=length || ferror(file);
    if(!failed && fflush(file)!=0) failed=1;
#ifdef _WIN32
    if(!failed && _commit(_fileno(file))!=0) failed=1;
#else
    if(!failed && fsync(fileno(file))!=0) failed=1;
#endif
    if(fclose(file)!=0) failed=1;
    if(!failed) {
#ifdef _WIN32
        if(!MoveFileExA(temporary,path,MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)) failed=1;
#else
        if(rename(temporary,path)!=0) failed=1;
#endif
    }
    if(failed) remove(temporary);
    free(temporary);
    return vnt_bool(!failed);
}

VNT_NATIVE_ABI VntValue * vnt_fs_write(VntValue *p, VntValue *s) { return vnt_fs_write_mode(p, s, "wb", "fs_write()"); }
VNT_NATIVE_ABI VntValue * vnt_fs_append(VntValue *p, VntValue *s) { return vnt_fs_write_mode(p, s, "ab", "fs_append()"); }
VNT_NATIVE_ABI VntValue * vnt_fs_delete(VntValue *path) { return vnt_bool(remove(vnt_app_string(path, "fs_delete()")) == 0); }
VNT_NATIVE_ABI VntValue * vnt_fs_rename(VntValue *from, VntValue *to) {
    return vnt_bool(rename(vnt_app_string(from, "fs_rename()"),
                           vnt_app_string(to, "fs_rename()")) == 0);
}
VNT_NATIVE_ABI VntValue * vnt_fs_is_file(VntValue *path) {
    const char *p = vnt_app_string(path, "fs_is_file()");
#ifdef _WIN32
    DWORD a = GetFileAttributesA(p);
    return vnt_bool(a != INVALID_FILE_ATTRIBUTES && !(a & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return vnt_bool(stat(p, &st) == 0 && S_ISREG(st.st_mode));
#endif
}
VNT_NATIVE_ABI VntValue * vnt_fs_is_dir(VntValue *path) {
    const char *p = vnt_app_string(path, "fs_is_dir()");
#ifdef _WIN32
    DWORD a = GetFileAttributesA(p);
    return vnt_bool(a != INVALID_FILE_ATTRIBUTES && (a & FILE_ATTRIBUTE_DIRECTORY));
#else
    struct stat st;
    return vnt_bool(stat(p, &st) == 0 && S_ISDIR(st.st_mode));
#endif
}
VNT_NATIVE_ABI VntValue * vnt_fs_list(VntValue *path) {
    const char *p = vnt_app_string(path, "fs_list()");
    VntValue *result = vnt_array_new();
#ifdef _WIN32
    size_t n = strlen(p);
    char *pattern = malloc(n + 3);
    if (!pattern) { fprintf(stderr, "Runtime error: out of memory in fs_list().\n"); exit(1); }
    snprintf(pattern, n + 3, "%s%s*", p,
             n && (p[n-1] == '/' || p[n-1] == '\\') ? "" : "\\");
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA(pattern, &data);
    free(pattern);
    if (h == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Runtime error: fs_list() could not list '%s'.\n", p);
        exit(1);
    }
    do {
        if (strcmp(data.cFileName, ".") && strcmp(data.cFileName, ".."))
            vnt_array_push(result, vnt_string(data.cFileName));
    } while (FindNextFileA(h, &data));
    FindClose(h);
#else
    DIR *dir = opendir(p);
    if (!dir) {
        fprintf(stderr, "Runtime error: fs_list() could not list '%s'.\n", p);
        exit(1);
    }
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
            vnt_array_push(result, vnt_string(entry->d_name));
    }
    closedir(dir);
#endif
    return result;
}
VNT_NATIVE_ABI VntValue * vnt_io_write(VntValue *value) {
    const char *s = vnt_app_string(value, "io_write()");
    size_t n = strlen(s);
    return vnt_bool(fwrite(s, 1, n, stdout) == n && fflush(stdout) == 0);
}
VNT_NATIVE_ABI VntValue * vnt_io_write_error(VntValue *value) {
    const char *s = vnt_app_string(value, "io_write_error()");
    size_t n = strlen(s);
    return vnt_bool(fwrite(s, 1, n, stderr) == n && fflush(stderr) == 0);
}
VNT_NATIVE_ABI VntValue * vnt_dir_create(VntValue *path) {
    const char *p = vnt_app_string(path, "dir_create()");
#ifdef _WIN32
    return vnt_bool(_mkdir(p) == 0);
#else
    return vnt_bool(mkdir(p, 0777) == 0);
#endif
}
VNT_NATIVE_ABI VntValue * vnt_cwd(void) {
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
VNT_NATIVE_ABI VntValue * vnt_env_get(VntValue *name) {
    const char *v = getenv(vnt_app_string(name, "env_get()")); return v ? vnt_string(v) : vnt_null();
}
VNT_NATIVE_ABI VntValue * vnt_env_has(VntValue *name) {
    return vnt_bool(getenv(vnt_app_string(name, "env_has()")) != NULL);
}
VNT_NATIVE_ABI VntValue * vnt_env_set(VntValue *name, VntValue *value) {
    const char *k = vnt_app_string(name, "env_set()"), *v = vnt_app_string(value, "env_set()");
#ifdef _WIN32
    return vnt_bool(_putenv_s(k, v) == 0);
#else
    return vnt_bool(setenv(k, v, 1) == 0);
#endif
}
VNT_NATIVE_ABI VntValue * vnt_time_ms(void) {
#ifdef _WIN32
    return vnt_float((double)GetTickCount64());
#else
    struct timeval tv; if (gettimeofday(&tv, NULL) != 0) return vnt_float(-1.0);
    return vnt_float((double)tv.tv_sec * 1000.0 + (double)tv.tv_usec / 1000.0);
#endif
}
/* Convert Unix epoch milliseconds using the host C runtime's timezone rules.
   Returned fields: [year, month, day, hour, minute, second, weekday, yearday, is_dst].
   Local conversion uses only the machine's configured timezone, not arbitrary IANA zones. */
static VntValue *vnt_time_parts(VntValue *value,int utc,const char *fn) {
    if(!value||value->type!=VNT_INT){fprintf(stderr,"Runtime error: %s expects epoch milliseconds as an integer.\\n",fn);exit(1);}
    int64_t ms=value->integer,seconds=ms/1000;
    if(ms<0 && ms%1000)seconds--;
    time_t stamp=(time_t)seconds;
    if((int64_t)stamp!=seconds)return vnt_array_new();
    struct tm parts;
    struct tm *converted=utc?gmtime(&stamp):localtime(&stamp);
    if(!converted)return vnt_array_new();
    parts=*converted;
    VntValue *result=vnt_array_new();
    vnt_array_push(result,vnt_int((int64_t)parts.tm_year+1900));
    vnt_array_push(result,vnt_int((int64_t)parts.tm_mon+1));
    vnt_array_push(result,vnt_int(parts.tm_mday));
    vnt_array_push(result,vnt_int(parts.tm_hour));
    vnt_array_push(result,vnt_int(parts.tm_min));
    vnt_array_push(result,vnt_int(parts.tm_sec));
    vnt_array_push(result,vnt_int(parts.tm_wday));
    vnt_array_push(result,vnt_int((int64_t)parts.tm_yday+1));
    vnt_array_push(result,vnt_int(parts.tm_isdst>0?1:0));
    return result;
}
VNT_NATIVE_ABI VntValue *vnt_time_local_parts(VntValue *value){return vnt_time_parts(value,0,"time_local_parts()");}
VNT_NATIVE_ABI VntValue *vnt_time_utc_parts(VntValue *value){return vnt_time_parts(value,1,"time_utc_parts()");}

VNT_NATIVE_ABI VntValue * vnt_sleep_ms(VntValue *value) {
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
    size_t arg_len = strlen(arg);
    if (arg_len > (SIZE_MAX - 4) / 2) {
        fprintf(stderr, "Runtime error: process argument is too large to quote.\n");
        exit(1);
    }
    size_t need = arg_len * 2 + 4;
    if (*len > SIZE_MAX - need - 1) {
        fprintf(stderr, "Runtime error: process command line is too large.\n");
        exit(1);
    }
    size_t required = *len + need + 1;
    if (required > *cap) {
        size_t nc = *cap ? *cap : 128;
        while (nc < required) {
            if (nc > SIZE_MAX / 2) {
                nc = required;
                break;
            }
            nc *= 2;
        }
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
VNT_NATIVE_ABI VntValue * vnt_process_start(VntValue *executable, VntValue *arguments) {
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
VNT_NATIVE_ABI VntValue * vnt_process_poll(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_poll()"); process_refresh(p); return vnt_bool(p->done);
}
VNT_NATIVE_ABI VntValue * vnt_process_wait(VntValue *value, VntValue *timeout) {
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
VNT_NATIVE_ABI VntValue * vnt_process_pid(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_pid()"); return vnt_int((int)p->pid);
}
VNT_NATIVE_ABI VntValue * vnt_process_terminate(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_terminate()"); process_refresh(p);
    if (p->done) return vnt_bool(0);
#ifdef _WIN32
    return vnt_bool(TerminateProcess(p->handle, 1) != 0);
#else
    return vnt_bool(kill(p->pid, SIGTERM) == 0);
#endif
}
VNT_NATIVE_ABI VntValue * vnt_process_stdout(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_stdout()"); process_refresh(p);
    return vnt_string(p->stdout_data ? p->stdout_data : "");
}
VNT_NATIVE_ABI VntValue * vnt_process_stderr(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_stderr()"); process_refresh(p);
    return vnt_string(p->stderr_data ? p->stderr_data : "");
}
VNT_NATIVE_ABI VntValue * vnt_process_exit_code(VntValue *value) {
    VntProcess *p = process_from_value(value, "process_exit_code()"); process_refresh(p);
    return vnt_int(p->done ? p->exit_code : -1);
}

/* Visible launcher for interactive desktop applications and scripts. */
VNT_NATIVE_ABI VntValue * vnt_process_launch(VntValue *path_value) {
    const char *path = vnt_app_string(path_value, "process_launch()");
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    if (attrs == INVALID_FILE_ATTRIBUTES || (attrs & FILE_ATTRIBUTE_DIRECTORY)) return vnt_bool(0);
    const char *ext = strrchr(path, '.');
    int is_script = ext && (!_stricmp(ext, ".bat") || !_stricmp(ext, ".cmd"));
    STARTUPINFOA si; PROCESS_INFORMATION pi;
    memset(&si, 0, sizeof(si)); memset(&pi, 0, sizeof(pi)); si.cb = sizeof(si);
    char *command = NULL; const char *application = path;
    if (is_script) {
        char system_dir[MAX_PATH]; UINT n = GetSystemDirectoryA(system_dir, MAX_PATH);
        if (!n || n >= MAX_PATH) return vnt_bool(0);
        size_t exe_len = strlen(system_dir) + sizeof("\\cmd.exe");
        char *cmd_exe = malloc(exe_len); if (!cmd_exe) return vnt_bool(0);
        snprintf(cmd_exe, exe_len, "%s\\cmd.exe", system_dir);
        size_t cap = strlen(cmd_exe) + strlen(path) * 2 + 32;
        command = malloc(cap); if (!command) { free(cmd_exe); return vnt_bool(0); }
        snprintf(command, cap, "\"%s\" /c \"\"%s\"\"", cmd_exe, path);
        application = cmd_exe;
    } else {
        size_t cap = strlen(path) * 2 + 4; command = malloc(cap);
        if (!command) return vnt_bool(0); snprintf(command, cap, "\"%s\"", path);
    }
    BOOL ok = CreateProcessA(application, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    free(command); if (is_script) free((void *)application);
    if (!ok) return vnt_bool(0);
    CloseHandle(pi.hThread); CloseHandle(pi.hProcess); return vnt_bool(1);
#else
    struct stat st; if (stat(path, &st) != 0 || S_ISDIR(st.st_mode)) return vnt_bool(0);
    pid_t pid = fork(); if (pid < 0) return vnt_bool(0);
    if (pid == 0) { setsid(); execl(path, path, (char *)NULL); _exit(127); }
    return vnt_bool(1);
#endif
}


/* Dependency-free SHA-256 for integrity checks and digest-based identifiers.
   This is hashing, not encryption or password-storage policy. */
static uint32_t vnt_sha_rotr(uint32_t x,unsigned n){return (x>>n)|(x<<(32-n));}
static void vnt_sha256_block(uint32_t h[8],const unsigned char block[64]) {
    static const uint32_t k[64]={
      0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
      0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
      0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
      0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
      0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
      0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
      0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
      0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
    };
    uint32_t w[64];
    for(int i=0;i<16;i++)w[i]=((uint32_t)block[i*4]<<24)|((uint32_t)block[i*4+1]<<16)|((uint32_t)block[i*4+2]<<8)|block[i*4+3];
    for(int i=16;i<64;i++){
        uint32_t s0=vnt_sha_rotr(w[i-15],7)^vnt_sha_rotr(w[i-15],18)^(w[i-15]>>3);
        uint32_t s1=vnt_sha_rotr(w[i-2],17)^vnt_sha_rotr(w[i-2],19)^(w[i-2]>>10);
        w[i]=w[i-16]+s0+w[i-7]+s1;
    }
    uint32_t a=h[0],b=h[1],c=h[2],d=h[3],e=h[4],f=h[5],g=h[6],z=h[7];
    for(int i=0;i<64;i++){
        uint32_t s1=vnt_sha_rotr(e,6)^vnt_sha_rotr(e,11)^vnt_sha_rotr(e,25);
        uint32_t ch=(e&f)^(~e&g),t1=z+s1+ch+k[i]+w[i];
        uint32_t s0=vnt_sha_rotr(a,2)^vnt_sha_rotr(a,13)^vnt_sha_rotr(a,22);
        uint32_t maj=(a&b)^(a&c)^(b&c),t2=s0+maj;
        z=g;g=f;f=e;e=d+t1;d=c;c=b;b=a;a=t1+t2;
    }
    h[0]+=a;h[1]+=b;h[2]+=c;h[3]+=d;h[4]+=e;h[5]+=f;h[6]+=g;h[7]+=z;
}
VNT_NATIVE_ABI VntValue *vnt_sha256(VntValue *input) {
    const unsigned char *data=(const unsigned char *)vnt_app_string(input,"sha256()");
    size_t n=strlen((const char *)data);
    if(n>(SIZE_MAX-72)){fprintf(stderr,"Runtime error: input too large for sha256().\\n");exit(1);}
    size_t padded=((n+9+63)/64)*64;
    unsigned char *buffer=(unsigned char *)calloc(padded,1);
    if(!buffer){fprintf(stderr,"Runtime error: out of memory in sha256().\\n");exit(1);}
    memcpy(buffer,data,n);buffer[n]=0x80;
    uint64_t bits=(uint64_t)n*8;
    for(int i=0;i<8;i++)buffer[padded-1-i]=(unsigned char)(bits>>(i*8));
    uint32_t h[8]={0x6a09e667,0xbb67ae85,0x3c6ef372,0xa54ff53a,0x510e527f,0x9b05688c,0x1f83d9ab,0x5be0cd19};
    for(size_t i=0;i<padded;i+=64)vnt_sha256_block(h,buffer+i);
    free(buffer);
    char hex[65];
    for(int i=0;i<8;i++)snprintf(hex+i*8,9,"%08x",h[i]);
    hex[64]=0;return vnt_string(hex);
}
VNT_NATIVE_ABI VntValue *vnt_constant_time_equal(VntValue *left,VntValue *right) {
    const unsigned char *a=(const unsigned char *)vnt_app_string(left,"constant_time_equal()");
    const unsigned char *b=(const unsigned char *)vnt_app_string(right,"constant_time_equal()");
    size_t na=strlen((const char *)a),nb=strlen((const char *)b),n=na>nb?na:nb;
    size_t diff=na^nb;
    for(size_t i=0;i<n;i++)diff|=(i<na?a[i]:0)^(i<nb?b[i]:0);
    return vnt_bool(diff==0);
}

/* Decimal-string arbitrary-precision signed integer arithmetic. */
typedef struct { int sign; size_t length; char *digits; } VntBigInt;
static void vnt_bigint_fail(void) {
    fprintf(stderr,"Runtime error: bigint operations require signed decimal integer strings.\\n"); exit(1);
}
static VntBigInt vnt_bigint_parse(VntValue *value,const char *fn) {
    const char *text=vnt_app_string(value,fn);
    VntBigInt n={1,0,NULL};
    if(*text=='-'||*text=='+'){if(*text=='-')n.sign=-1;text++;}
    const char *begin=text;
    while(*text>='0'&&*text<='9')text++;
    if(text==begin||*text) vnt_bigint_fail();
    while(begin[0]=='0' && begin[1]) begin++;
    n.length=(size_t)(text-begin);
    n.digits=(char *)malloc(n.length+1);
    if(!n.digits){fprintf(stderr,"Runtime error: out of memory in bigint operation.\\n");exit(1);}
    memcpy(n.digits,begin,n.length);n.digits[n.length]=0;
    if(n.length==1&&n.digits[0]=='0')n.sign=0;
    return n;
}
static int vnt_bigint_magcmp(const VntBigInt *a,const VntBigInt *b) {
    if(a->length!=b->length)return a->length<b->length?-1:1;
    int c=memcmp(a->digits,b->digits,a->length);
    return c<0?-1:c>0?1:0;
}
static char *vnt_bigint_format(int sign,const char *digits,size_t length) {
    while(length>1&&*digits=='0'){digits++;length--;}
    int zero=length==1&&digits[0]=='0';
    size_t prefix=(sign<0&&!zero)?1:0;
    if(length>SIZE_MAX-prefix-1) {
        fprintf(stderr,"Runtime error: bigint result size overflow.\n");
        exit(1);
    }
    char *out=(char *)malloc(prefix+length+1);
    if(!out){fprintf(stderr,"Runtime error: out of memory formatting bigint.\\n");exit(1);}
    if(prefix)out[0]='-';
    memcpy(out+prefix,digits,length);out[prefix+length]=0;return out;
}
static char *vnt_bigint_addmag(const VntBigInt *a,const VntBigInt *b,size_t *outlen) {
    size_t largest=a->length>b->length?a->length:b->length;
    if(largest>SIZE_MAX-2) {
        fprintf(stderr,"Runtime error: bigint addition size overflow.\n");
        exit(1);
    }
    size_t n=largest+1;
    char *out=(char *)malloc(n+1);if(!out){fprintf(stderr,"Runtime error: out of memory.\\n");exit(1);}
    int carry=0;
    for(size_t k=0;k<n-1;k++){
        int da=k<a->length?a->digits[a->length-1-k]-'0':0;
        int db=k<b->length?b->digits[b->length-1-k]-'0':0;
        int sum=da+db+carry;out[n-1-k]=(char)('0'+sum%10);carry=sum/10;
    }
    out[0]=(char)('0'+carry);
    size_t start=carry?0:1;*outlen=n-start;
    memmove(out,out+start,*outlen);out[*outlen]=0;return out;
}
static char *vnt_bigint_submag(const VntBigInt *large,const VntBigInt *small,size_t *outlen) {
    char *out=(char *)malloc(large->length+1);if(!out){fprintf(stderr,"Runtime error: out of memory.\\n");exit(1);}
    int borrow=0;
    for(size_t k=0;k<large->length;k++){
        int da=large->digits[large->length-1-k]-'0';
        int db=k<small->length?small->digits[small->length-1-k]-'0':0;
        int v=da-db-borrow;if(v<0){v+=10;borrow=1;}else borrow=0;
        out[large->length-1-k]=(char)('0'+v);
    }
    size_t start=0;while(start+1<large->length&&out[start]=='0')start++;
    *outlen=large->length-start;memmove(out,out+start,*outlen);out[*outlen]=0;return out;
}
static VntValue *vnt_bigint_binary(VntValue *av,VntValue *bv,int operation) {
    VntBigInt a=vnt_bigint_parse(av,"bigint operation"),b=vnt_bigint_parse(bv,"bigint operation");
    char *digits=NULL;size_t length=0;int sign=1;
    if(operation==3) {
        int cmp=a.sign==b.sign?(a.sign==0?0:vnt_bigint_magcmp(&a,&b)*(a.sign<0?-1:1)):(a.sign<b.sign?-1:1);
        VntValue *r=vnt_int(cmp<0?-1:cmp>0?1:0);free(a.digits);free(b.digits);return r;
    }
    if(operation==2) {
        if(a.sign==0||b.sign==0){digits=strdup("0");length=1;sign=0;}
        else {
            if(a.length>SIZE_MAX-b.length) {
                fprintf(stderr,"Runtime error: bigint multiplication size overflow.\n");
                exit(1);
            }
            length=a.length+b.length;
            if(length>SIZE_MAX/sizeof(int)-1) {
                fprintf(stderr,"Runtime error: bigint multiplication allocation size overflow.\n");
                exit(1);
            }
            int *acc=(int *)calloc(length+1,sizeof(int));
            digits=(char *)malloc(length+1);
            if(!acc||!digits){free(acc);free(digits);fprintf(stderr,"Runtime error: out of memory multiplying bigint.\\n");exit(1);}
            for(size_t i=a.length;i>0;i--) {
                int carry=0,da=a.digits[i-1]-'0';
                for(size_t j=b.length;j>0;j--) {
                    size_t pos=i+j-1;
                    int sum=acc[pos]+da*(b.digits[j-1]-'0')+carry;
                    acc[pos]=sum%10;carry=sum/10;
                }
                acc[i-1]+=carry;
            }
            for(size_t i=0;i<length;i++)digits[i]=(char)('0'+acc[i]);
            digits[length]=0;free(acc);sign=a.sign*b.sign;
        }
    } else {
        int bsign=operation==1?-b.sign:b.sign;
        if(a.sign==0){digits=strdup(b.digits);length=b.length;sign=bsign;}
        else if(bsign==0){digits=strdup(a.digits);length=a.length;sign=a.sign;}
        else if(a.sign==bsign){digits=vnt_bigint_addmag(&a,&b,&length);sign=a.sign;}
        else {
            int cmp=vnt_bigint_magcmp(&a,&b);
            if(cmp==0){digits=strdup("0");length=1;sign=0;}
            else if(cmp>0){digits=vnt_bigint_submag(&a,&b,&length);sign=a.sign;}
            else {digits=vnt_bigint_submag(&b,&a,&length);sign=bsign;}
        }
    }
    if(!digits){fprintf(stderr,"Runtime error: out of memory in bigint operation.\\n");exit(1);}
    char *formatted=vnt_bigint_format(sign,digits,length);
    VntValue *result=vnt_string(formatted);
    free(formatted);free(digits);free(a.digits);free(b.digits);return result;
}
VNT_NATIVE_ABI VntValue *vnt_bigint_add(VntValue *a,VntValue *b){return vnt_bigint_binary(a,b,0);}
VNT_NATIVE_ABI VntValue *vnt_bigint_sub(VntValue *a,VntValue *b){return vnt_bigint_binary(a,b,1);}
VNT_NATIVE_ABI VntValue *vnt_bigint_mul(VntValue *a,VntValue *b){return vnt_bigint_binary(a,b,2);}
VNT_NATIVE_ABI VntValue *vnt_bigint_compare(VntValue *a,VntValue *b){return vnt_bigint_binary(a,b,3);}

/* Read a scalar field from the Nth JSON object/key occurrence. This is a
   deliberately small accessor for API responses, not a full JSON DOM. */
static VNT_NATIVE_ABI int vnt_json_read_hex4(const char **p, uint32_t *value) {
    uint32_t result=0;
    for(int i=0;i<4;i++) {
        if(!(*p)[i])return 0;
        unsigned char ch=(unsigned char)(*p)[i];
        uint32_t digit;
        if(ch>='0'&&ch<='9')digit=(uint32_t)(ch-'0');
        else if(ch>='a'&&ch<='f')digit=(uint32_t)(ch-'a'+10);
        else if(ch>='A'&&ch<='F')digit=(uint32_t)(ch-'A'+10);
        else return 0;
        result=(result<<4)|digit;
    }
    *p+=4;
    *value=result;
    return 1;
}
static VNT_NATIVE_ABI int vnt_json_append_codepoint(char *out,size_t cap,size_t *n,uint32_t cp) {
    unsigned char bytes[4];
    size_t count;
    if(cp<=0x7f){bytes[0]=(unsigned char)cp;count=1;}
    else if(cp<=0x7ff){bytes[0]=(unsigned char)(0xc0|(cp>>6));bytes[1]=(unsigned char)(0x80|(cp&0x3f));count=2;}
    else if(cp<=0xffff && !(cp>=0xd800&&cp<=0xdfff)){bytes[0]=(unsigned char)(0xe0|(cp>>12));bytes[1]=(unsigned char)(0x80|((cp>>6)&0x3f));bytes[2]=(unsigned char)(0x80|(cp&0x3f));count=3;}
    else if(cp<=0x10ffff){bytes[0]=(unsigned char)(0xf0|(cp>>18));bytes[1]=(unsigned char)(0x80|((cp>>12)&0x3f));bytes[2]=(unsigned char)(0x80|((cp>>6)&0x3f));bytes[3]=(unsigned char)(0x80|(cp&0x3f));count=4;}
    else return 0;
    if(*n+count>=cap)return 0;
    memcpy(out+*n,bytes,count);
    *n+=count;
    out[*n]=0;
    return 1;
}
static VNT_NATIVE_ABI void vnt_json_copy_string(const char *p, char *out, size_t cap) {
    size_t n=0;
    if(cap==0)return;
    out[0]=0;
    if(*p=='"')p++;
    while(*p && *p!='"' && n+1<cap) {
        unsigned char ch=(unsigned char)*p++;
        if(ch=='\\' && *p) {
            ch=(unsigned char)*p++;
            switch(ch) {
                case '"': case '\\': case '/': break;
                case 'n': ch='\n'; break;
                case 'r': ch='\r'; break;
                case 't': ch='\t'; break;
                case 'b': ch=' '; break;
                case 'f': ch=' '; break;
                case 'u': {
                    uint32_t cp;
                    if(!vnt_json_read_hex4(&p,&cp)) { ch='?'; break; }
                    if(cp>=0xd800&&cp<=0xdbff) {
                        const char *next=p;
                        uint32_t low;
                        if(next[0]=='\\'&&next[1]=='u') {
                            next+=2;
                            if(vnt_json_read_hex4(&next,&low)&&low>=0xdc00&&low<=0xdfff) {
                                cp=0x10000+((cp-0xd800)<<10)+(low-0xdc00);
                                p=next;
                            } else cp=0xfffd;
                        } else cp=0xfffd;
                    } else if(cp>=0xdc00&&cp<=0xdfff) cp=0xfffd;
                    if(!vnt_json_append_codepoint(out,cap,&n,cp)) return;
                    continue;
                }
                default: break;
            }
        }
        if(n+1>=cap)break;
        out[n++]=(char)ch;
        out[n]=0;
    }
}
static VNT_NATIVE_ABI void vnt_json_extract_value(const char *json,const char *key,int64_t wanted,char *out,size_t cap) {
    int64_t found=0;
    out[0]=0;
    if(!json||!key||wanted<0||cap<2)return;
    const char *p=json;
    while(*p) {
        if(*p!='"'){p++;continue;}
        const char *start=++p;
        int escaped=0;
        while(*p) {
            if(escaped){escaped=0;p++;continue;}
            if(*p=='\\'){escaped=1;p++;continue;}
            if(*p=='"')break;
            p++;
        }
        if(!*p)break;
        size_t key_len=(size_t)(p-start);
        const char *after=p+1;
        while(*after==' '||*after=='\t'||*after=='\r'||*after=='\n')after++;
        if(*after==':') {
            char *decoded_key=(char*)malloc(key_len+1);
            if(!decoded_key) {
                fprintf(stderr,"Runtime error: json_value_at() could not allocate a key buffer.\n");
                exit(1);
            }
            vnt_json_copy_string(start-1,decoded_key,key_len+1);
            int matches=strcmp(decoded_key,key)==0;
            free(decoded_key);
            if(matches && found++==wanted) {
                after++;
                while(*after==' '||*after=='\t'||*after=='\r'||*after=='\n')after++;
                if(*after=='"')vnt_json_copy_string(after,out,cap);
                else {
                    size_t n=0;
                    while(after[n] && after[n]!=',' && after[n]!='}' && after[n]!=']' && n+1<cap)n++;
                    while(n>0 && (after[n-1]==' '||after[n-1]=='\t'||after[n-1]=='\r'||after[n-1]=='\n'))n--;
                    memcpy(out,after,n);out[n]=0;
                }
                return;
            }
        }
        p++;
    }
}
VNT_NATIVE_ABI VntValue * vnt_json_value_at(VntValue *json_value,VntValue *key_value,VntValue *index_value) {
    const char *json=vnt_app_string(json_value,"json_value_at()");
    const char *key=vnt_app_string(key_value,"json_value_at()");
    if(!index_value || index_value->type!=VNT_INT) {
        fprintf(stderr,"Runtime error: json_value_at() expects an integer occurrence index.\n");
        exit(1);
    }
    size_t capacity=strlen(json)+1;
    char *out=(char*)malloc(capacity);
    if(!out) {
        fprintf(stderr,"Runtime error: json_value_at() could not allocate its result buffer.\n");
        exit(1);
    }
    vnt_json_extract_value(json,key,index_value->integer,out,capacity);
    VntValue *result=vnt_string(out);
    free(out);
    return result;
}

/* Vanta GUI native backend: retained command list + double-buffered WM_PAINT. */
#ifdef _WIN32
enum { VG_TEXT=1, VG_TITLE, VG_PANEL, VG_BUTTON, VG_INPUT, VG_TEXTAREA, VG_CHECKBOX, VG_PROGRESS, VG_SEPARATOR, VG_RECT };
typedef struct {
    int type,x,y,w,h,font_size,radius,hovered,checked,value,maximum,placeholder;
    COLORREF color,background;
    char text[4096];
} VntGuiCommand;
typedef struct { int x,y,width,height,multiline,select_all,seen_this_frame; HWND edit_hwnd; char text[4096]; } VntGuiInputState;
static HWND vnt_gui_hwnd;
static int vnt_gui_width=800, vnt_gui_height=600, vnt_gui_last_key, vnt_gui_text_y=18;
static COLORREF vnt_gui_background=RGB(11,16,32);
static const char *vnt_gui_class_name="VNTNativeWindow";
static int vnt_gui_clicked, vnt_gui_click_x, vnt_gui_click_y, vnt_gui_mouse_x, vnt_gui_mouse_y;
static COLORREF vnt_gui_text_color=RGB(170,182,211), vnt_gui_title_color=RGB(244,247,255);
static COLORREF vnt_gui_panel_background=RGB(21,30,51), vnt_gui_button_background=RGB(49,92,232);
static COLORREF vnt_gui_button_hover=RGB(80,120,255), vnt_gui_button_text=RGB(255,255,255);
static COLORREF vnt_gui_input_background=RGB(10,15,29), vnt_gui_input_text=RGB(220,231,255), vnt_gui_input_border=RGB(49,92,232);
static COLORREF vnt_gui_textarea_background=RGB(10,15,29), vnt_gui_textarea_text=RGB(220,231,255), vnt_gui_textarea_border=RGB(49,92,232);
static HBRUSH vnt_gui_input_brush, vnt_gui_textarea_brush;
static int vnt_gui_input_font_size=14, vnt_gui_textarea_font_size=14;
static COLORREF vnt_gui_accent=RGB(49,92,232);
static int vnt_gui_font_size=16, vnt_gui_title_font_size=30, vnt_gui_button_font_size=15;
static int vnt_gui_button_padding=10, vnt_gui_button_radius=9;
static VntGuiCommand vnt_gui_commands[2048];
static int vnt_gui_command_count;
static VntGuiInputState vnt_gui_inputs[16];
static int vnt_gui_input_count, vnt_gui_focused_input=-1;

static VNT_NATIVE_ABI int vnt_gui_require_int(VntValue *v,const char *fn) {
    if(!v || v->type!=VNT_INT || v->integer < INT_MIN || v->integer > INT_MAX) {
        fprintf(stderr,"Runtime error: %s expects an integer in the platform UI range.\n",fn);
        exit(1);
    }
    return (int)v->integer;
}
static COLORREF vnt_gui_color(int c) { return RGB((c>>16)&255,(c>>8)&255,c&255); }
static VNT_NATIVE_ABI char * vnt_gui_trim(char *s) {
    while(*s==' '||*s=='\t'||*s=='\r'||*s=='\n') s++;
    size_t n=strlen(s); while(n&&(s[n-1]==' '||s[n-1]=='\t'||s[n-1]=='\r'||s[n-1]=='\n')) s[--n]=0;
    return s;
}
static VNT_NATIVE_ABI int vnt_gui_parse_color(const char *s,COLORREF *out) {
    if(!s||s[0]!='#'||strlen(s)!=7) return 0;
    char *end=NULL; unsigned long rgb=strtoul(s+1,&end,16);
    if(!end||*end) return 0; *out=RGB((rgb>>16)&255,(rgb>>8)&255,rgb&255); return 1;
}
static VNT_NATIVE_ABI void vnt_gui_apply_css_rule(const char *css,const char *selector) {
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
            else if(!strcmp(selector,"input")&&!strcmp(key,"background-color")&&vnt_gui_parse_color(value,&c))vnt_gui_input_background=c;
            else if(!strcmp(selector,"input")&&!strcmp(key,"color")&&vnt_gui_parse_color(value,&c))vnt_gui_input_text=c;
            else if(!strcmp(selector,"input")&&!strcmp(key,"border-color")&&vnt_gui_parse_color(value,&c))vnt_gui_input_border=c;
            else if(!strcmp(selector,"input")&&!strcmp(key,"font-size")){int n=atoi(value);if(n>=8&&n<=48)vnt_gui_input_font_size=n;}
            else if(!strcmp(selector,"textarea")&&!strcmp(key,"background-color")&&vnt_gui_parse_color(value,&c))vnt_gui_textarea_background=c;
            else if(!strcmp(selector,"textarea")&&!strcmp(key,"color")&&vnt_gui_parse_color(value,&c))vnt_gui_textarea_text=c;
            else if(!strcmp(selector,"textarea")&&!strcmp(key,"border-color")&&vnt_gui_parse_color(value,&c))vnt_gui_textarea_border=c;
            else if(!strcmp(selector,"textarea")&&!strcmp(key,"font-size")){int n=atoi(value);if(n>=8&&n<=48)vnt_gui_textarea_font_size=n;}
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
    if(vnt_gui_command_count>=2048) return NULL;
    VntGuiCommand *c=&vnt_gui_commands[vnt_gui_command_count++]; memset(c,0,sizeof(*c));
    c->type=type;c->x=x;c->y=y;c->w=w;c->h=h;
    if(text) snprintf(c->text,sizeof(c->text),"%s",text);
    return c;
}
static VNT_NATIVE_ABI void vnt_gui_render(HDC dc) {
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
                if(c->type==VG_INPUT&&vnt_gui_focused_input>=0&&vnt_gui_focused_input<vnt_gui_input_count&&vnt_gui_inputs[vnt_gui_focused_input].x==c->x&&vnt_gui_inputs[vnt_gui_focused_input].y==c->y&&!c->placeholder) { SIZE sz={0};GetTextExtentPoint32A(dc,c->text,(int)strlen(c->text),&sz);MoveToEx(dc,r.left+10+sz.cx,r.top+7,NULL);LineTo(dc,r.left+10+sz.cx,r.bottom-7); }
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
            PAINTSTRUCT ps;
            HDC target=BeginPaint(hwnd,&ps);
            if(target) {
                RECT client;
                if(GetClientRect(hwnd,&client) && client.right>client.left && client.bottom>client.top) {
                    HDC memory=CreateCompatibleDC(target);
                    HBITMAP bitmap=memory?CreateCompatibleBitmap(target,client.right-client.left,client.bottom-client.top):NULL;
                    HGDIOBJ previous=NULL;
                    if(memory && bitmap) previous=SelectObject(memory,bitmap);
                    if(memory && bitmap && previous && previous!=HGDI_ERROR) {
                        vnt_gui_render(memory);
                        BitBlt(target,0,0,client.right-client.left,client.bottom-client.top,memory,0,0,SRCCOPY);
                        SelectObject(memory,previous);
                    } else {
                        vnt_gui_render(target);
                    }
                    if(bitmap) DeleteObject(bitmap);
                    if(memory) DeleteDC(memory);
                }
            }
            EndPaint(hwnd,&ps);
            return 0;
        }
        case WM_CTLCOLOREDIT: {
            HWND control=(HWND)lp;
            for(int i=0;i<vnt_gui_input_count;i++) if(vnt_gui_inputs[i].edit_hwnd==control) {
                int multi=vnt_gui_inputs[i].multiline;
                SetTextColor((HDC)wp,multi?vnt_gui_textarea_text:vnt_gui_input_text);
                SetBkColor((HDC)wp,multi?vnt_gui_textarea_background:vnt_gui_input_background);
                HBRUSH brush=multi?vnt_gui_textarea_brush:vnt_gui_input_brush;
                if(brush)return (LRESULT)brush;
                break;
            }
            return DefWindowProcA(hwnd,msg,wp,lp);
        }
        case WM_ERASEBKGND:return 1;
        case WM_KEYDOWN:
            vnt_gui_last_key=(int)wp;
            return 0;
        case WM_MOUSEMOVE:{int oldx=vnt_gui_mouse_x,oldy=vnt_gui_mouse_y;vnt_gui_mouse_x=(short)LOWORD(lp);vnt_gui_mouse_y=(short)HIWORD(lp);if(oldx!=vnt_gui_mouse_x||oldy!=vnt_gui_mouse_y)InvalidateRect(hwnd,NULL,FALSE);return 0;}
        case WM_LBUTTONDOWN:
            SetFocus(hwnd);
            return 0;
        case WM_LBUTTONUP:
            vnt_gui_click_x=(short)LOWORD(lp);
            vnt_gui_click_y=(short)HIWORD(lp);
            vnt_gui_clicked=1;
            InvalidateRect(hwnd,NULL,FALSE);
            return 0;
        case WM_DESTROY:if(hwnd==vnt_gui_hwnd)vnt_gui_hwnd=NULL;PostQuitMessage(0);return 0;
        default:return DefWindowProcA(hwnd,msg,wp,lp);
    }
}
VNT_NATIVE_ABI VntValue * vnt_gui_css(VntValue *path) {
    const char *filename=vnt_app_string(path,"gui_css()");FILE *file=fopen(filename,"rb");if(!file)return vnt_bool(0);
    char *css=(char*)malloc(65536);if(!css){fclose(file);return vnt_bool(0);}
    size_t n=fread(css,1,65535,file);fclose(file);css[n]=0;
    if(n==65535){free(css);fprintf(stderr,"Runtime error: stylesheet exceeds 65535 bytes.\n");return vnt_bool(0);}
    vnt_gui_apply_css_rule(css,"window");vnt_gui_apply_css_rule(css,"label");vnt_gui_apply_css_rule(css,"title");
    vnt_gui_apply_css_rule(css,"panel");vnt_gui_apply_css_rule(css,"button");vnt_gui_apply_css_rule(css,"button:hover");
    vnt_gui_apply_css_rule(css,"input");vnt_gui_apply_css_rule(css,"textarea");
    if(vnt_gui_input_brush)DeleteObject(vnt_gui_input_brush);
    if(vnt_gui_textarea_brush)DeleteObject(vnt_gui_textarea_brush);
    vnt_gui_input_brush=CreateSolidBrush(vnt_gui_input_background);
    vnt_gui_textarea_brush=CreateSolidBrush(vnt_gui_textarea_background);free(css);
    return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_size(VntValue *width,VntValue *height) {
    int w=vnt_gui_require_int(width,"gui_size()"),h=vnt_gui_require_int(height,"gui_size()");
    if(w<160||h<120||w>8192||h>8192){fprintf(stderr,"Runtime error: gui_size() dimensions must be between 160x120 and 8192x8192.\n");return vnt_bool(0);}
    vnt_gui_width=w;vnt_gui_height=h;return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_open(VntValue *title) {
    const char *caption=vnt_app_string(title,"gui_open()");if(vnt_gui_hwnd)return vnt_bool(1);
    HINSTANCE instance=GetModuleHandleA(NULL);WNDCLASSEXA wc;memset(&wc,0,sizeof(wc));
    wc.cbSize=sizeof(wc);wc.lpfnWndProc=vnt_gui_wndproc;wc.hInstance=instance;wc.hCursor=LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground=NULL;wc.lpszClassName=vnt_gui_class_name;
    if(!RegisterClassExA(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return vnt_bool(0);
    RECT rect={0,0,vnt_gui_width,vnt_gui_height};AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);
    vnt_gui_text_y=18;vnt_gui_last_key=0;vnt_gui_clicked=0;vnt_gui_command_count=0;vnt_gui_input_count=0;vnt_gui_focused_input=-1;
    vnt_gui_hwnd=CreateWindowExA(0,vnt_gui_class_name,caption,WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN,CW_USEDEFAULT,CW_USEDEFAULT,rect.right-rect.left,rect.bottom-rect.top,NULL,NULL,instance,NULL);
    if(!vnt_gui_hwnd)return vnt_bool(0);ShowWindow(vnt_gui_hwnd,SW_SHOW);UpdateWindow(vnt_gui_hwnd);return vnt_bool(1);
}
static VNT_NATIVE_ABI VntValue * vnt_gui_draw_text(const char *text,int x,int y,int title) {
    VntGuiCommand *c=vnt_gui_add(title?VG_TITLE:VG_TEXT,x,y,0,0,text);if(!c)return vnt_bool(0);
    c->font_size=title?vnt_gui_title_font_size:vnt_gui_font_size;c->color=title?vnt_gui_title_color:vnt_gui_text_color;return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_text(VntValue *text){const char *s=vnt_app_string(text,"gui_text()");int y=vnt_gui_text_y;vnt_gui_text_y+=vnt_gui_font_size+8;RECT r;if(vnt_gui_hwnd&&GetClientRect(vnt_gui_hwnd,&r)&&vnt_gui_text_y>r.bottom-20)vnt_gui_text_y=18;return vnt_gui_draw_text(s,18,y,0);}
VNT_NATIVE_ABI VntValue * vnt_gui_text_at(VntValue *text,VntValue *x,VntValue *y){return vnt_gui_draw_text(vnt_app_string(text,"gui_text_at()"),vnt_gui_require_int(x,"gui_text_at()"),vnt_gui_require_int(y,"gui_text_at()"),0);}
VNT_NATIVE_ABI VntValue * vnt_gui_title(VntValue *text,VntValue *x,VntValue *y){return vnt_gui_draw_text(vnt_app_string(text,"gui_title()"),vnt_gui_require_int(x,"gui_title()"),vnt_gui_require_int(y,"gui_title()"),1);}
VNT_NATIVE_ABI VntValue * vnt_gui_panel(VntValue *x,VntValue *y,VntValue *w,VntValue *h){
    int xx=vnt_gui_require_int(x,"gui_panel()"),yy=vnt_gui_require_int(y,"gui_panel()"),ww=vnt_gui_require_int(w,"gui_panel()"),hh=vnt_gui_require_int(h,"gui_panel()");
    VntGuiCommand *c=vnt_gui_add(VG_PANEL,xx,yy,ww,hh,NULL);if(!c)return vnt_bool(0);c->background=vnt_gui_panel_background;c->radius=12;return vnt_bool(1);
}
static VNT_NATIVE_ABI int vnt_gui_input_state(int x,int y,int width,int height,int multiline,int create){
    for(int i=0;i<vnt_gui_input_count;i++) if(vnt_gui_inputs[i].x==x&&vnt_gui_inputs[i].y==y){
        VntGuiInputState *state=&vnt_gui_inputs[i];
        state->seen_this_frame=1;
        state->width=width;state->height=height;state->multiline=multiline;
        if(state->edit_hwnd&&width>0&&height>0) {
            RECT r;
            if(GetWindowRect(state->edit_hwnd,&r)) {
                POINT p={r.left,r.top};ScreenToClient(vnt_gui_hwnd,&p);
                if(p.x!=x||p.y!=y||(r.right-r.left)!=width||(r.bottom-r.top)!=height)
                    SetWindowPos(state->edit_hwnd,NULL,x,y,width,height,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREDRAW);
            }
        }
        return i;
    }
    if(!create||vnt_gui_input_count>=16)return -1;
    int i=vnt_gui_input_count++;memset(&vnt_gui_inputs[i],0,sizeof(vnt_gui_inputs[i]));
    vnt_gui_inputs[i].x=x;vnt_gui_inputs[i].y=y;vnt_gui_inputs[i].width=width;vnt_gui_inputs[i].height=height;vnt_gui_inputs[i].multiline=multiline;vnt_gui_inputs[i].seen_this_frame=1;return i;
}
static VNT_NATIVE_ABI int vnt_gui_native_input(int index,const char *placeholder){
    if(index<0||index>=vnt_gui_input_count||!vnt_gui_hwnd)return 0;
    VntGuiInputState *state=&vnt_gui_inputs[index];
    if(!state->edit_hwnd){
        DWORD style=WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_LEFT;
        if(state->multiline) style|=ES_MULTILINE|ES_AUTOVSCROLL|ES_WANTRETURN|WS_VSCROLL;
        else style|=ES_AUTOHSCROLL;
        state->edit_hwnd=CreateWindowExA(WS_EX_CLIENTEDGE,"EDIT",state->text,style,
            state->x,state->y,state->width,state->height,vnt_gui_hwnd,
            (HMENU)(INT_PTR)(1000+index),GetModuleHandleA(NULL),NULL);
        if(!state->edit_hwnd)return 0;
        HFONT font=CreateFontA(-((state->multiline)?vnt_gui_textarea_font_size:vnt_gui_input_font_size),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"Segoe UI");
        if(font)SendMessageA(state->edit_hwnd,WM_SETFONT,(WPARAM)font,TRUE);
        SendMessageA(state->edit_hwnd,EM_SETLIMITTEXT,(WPARAM)(sizeof(state->text)-1),0);
        if(placeholder&&*placeholder){
            wchar_t cue[512];
            int count=MultiByteToWideChar(CP_UTF8,0,placeholder,-1,cue,(int)(sizeof(cue)/sizeof(cue[0])));
            if(count>0) SendMessageW(state->edit_hwnd,0x1501,TRUE,(LPARAM)cue);
        }
    } else {
        RECT r;
        if(GetWindowRect(state->edit_hwnd,&r)) {
            POINT p={r.left,r.top};ScreenToClient(vnt_gui_hwnd,&p);
            if(p.x!=state->x||p.y!=state->y||(r.right-r.left)!=state->width||(r.bottom-r.top)!=state->height)
                SetWindowPos(state->edit_hwnd,NULL,state->x,state->y,state->width,state->height,SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOREDRAW);
        }
    }
    if(!IsWindowVisible(state->edit_hwnd))ShowWindow(state->edit_hwnd,SW_SHOW);
    return 1;
}
static VNT_NATIVE_ABI void vnt_gui_read_native_input(int index){
    if(index<0||index>=vnt_gui_input_count)return;
    VntGuiInputState *state=&vnt_gui_inputs[index];
    if(!state->edit_hwnd)return;
    GetWindowTextA(state->edit_hwnd,state->text,(int)sizeof(state->text));
    state->text[sizeof(state->text)-1]=0;
}
static VNT_NATIVE_ABI VntValue * vnt_gui_button_draw(const char *text,int xx,int yy,int w,int h){
    if(w<24)w=24;if(h<24)h=24;RECT r={xx,yy,xx+w,yy+h};
    int hover=vnt_gui_mouse_x>=r.left&&vnt_gui_mouse_x<r.right&&vnt_gui_mouse_y>=r.top&&vnt_gui_mouse_y<r.bottom;
    VntGuiCommand *c=vnt_gui_add(VG_BUTTON,xx,yy,w,h,text);if(c){c->font_size=vnt_gui_button_font_size;c->radius=vnt_gui_button_radius;c->hovered=hover;}
    return vnt_bool(vnt_gui_clicked&&vnt_gui_click_x>=r.left&&vnt_gui_click_x<r.right&&vnt_gui_click_y>=r.top&&vnt_gui_click_y<r.bottom);
}
VNT_NATIVE_ABI VntValue * vnt_gui_button(VntValue *label,VntValue *x,VntValue *y){
    const char *text=vnt_app_string(label,"gui_button()");int xx=vnt_gui_require_int(x,"gui_button()"),yy=vnt_gui_require_int(y,"gui_button()");
    int w=(int)(strlen(text)*vnt_gui_button_font_size*0.62)+vnt_gui_button_padding*2+16,h=vnt_gui_button_font_size+vnt_gui_button_padding*2+2;
    return vnt_gui_button_draw(text,xx,yy,w,h);
}
VNT_NATIVE_ABI VntValue * vnt_gui_button_sized(VntValue *label,VntValue *x,VntValue *y,VntValue *width,VntValue *height){
    return vnt_gui_button_draw(vnt_app_string(label,"gui_button_sized()"),vnt_gui_require_int(x,"gui_button_sized()"),vnt_gui_require_int(y,"gui_button_sized()"),vnt_gui_require_int(width,"gui_button_sized()"),vnt_gui_require_int(height,"gui_button_sized()"));
}
VNT_NATIVE_ABI VntValue * vnt_gui_input(VntValue *label,VntValue *x,VntValue *y,VntValue *width){
    const char *placeholder=vnt_app_string(label,"gui_input()");int xx=vnt_gui_require_int(x,"gui_input()"),yy=vnt_gui_require_int(y,"gui_input()"),ww=vnt_gui_require_int(width,"gui_input()");
    int index=vnt_gui_input_state(xx,yy,ww,36,0,1);
    if(index>=0){vnt_gui_native_input(index,placeholder);vnt_gui_read_native_input(index);return vnt_string(vnt_gui_inputs[index].text);}
    return vnt_string("");
}
VNT_NATIVE_ABI VntValue * vnt_gui_textarea(VntValue *label,VntValue *x,VntValue *y,VntValue *width,VntValue *height){
    const char *placeholder=vnt_app_string(label,"gui_textarea()");int xx=vnt_gui_require_int(x,"gui_textarea()"),yy=vnt_gui_require_int(y,"gui_textarea()"),ww=vnt_gui_require_int(width,"gui_textarea()"),hh=vnt_gui_require_int(height,"gui_textarea()");
    int index=vnt_gui_input_state(xx,yy,ww,hh,1,1);
    if(index>=0){vnt_gui_native_input(index,placeholder);vnt_gui_read_native_input(index);return vnt_string(vnt_gui_inputs[index].text);}
    return vnt_string("");
}
VNT_NATIVE_ABI VntValue * vnt_gui_input_set(VntValue *text,VntValue *x,VntValue *y){
    const char *value=vnt_app_string(text,"gui_input_set()");int xx=vnt_gui_require_int(x,"gui_input_set()"),yy=vnt_gui_require_int(y,"gui_input_set()");
    int index=vnt_gui_input_state(xx,yy,0,0,0,1);if(index<0)return vnt_bool(0);
    snprintf(vnt_gui_inputs[index].text,sizeof(vnt_gui_inputs[index].text),"%s",value);vnt_gui_inputs[index].select_all=0;
    if(vnt_gui_inputs[index].edit_hwnd)SetWindowTextA(vnt_gui_inputs[index].edit_hwnd,vnt_gui_inputs[index].text);
    return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_textarea_set(VntValue *text,VntValue *x,VntValue *y){
    const char *value=vnt_app_string(text,"gui_textarea_set()");
    int xx=vnt_gui_require_int(x,"gui_textarea_set()"),yy=vnt_gui_require_int(y,"gui_textarea_set()");
    int index=-1;
    for(int i=0;i<vnt_gui_input_count;i++)
        if(vnt_gui_inputs[i].x==xx&&vnt_gui_inputs[i].y==yy){index=i;break;}
    if(index<0)index=vnt_gui_input_state(xx,yy,1,1,1,1);
    if(index<0)return vnt_bool(0);
    VntGuiInputState *state=&vnt_gui_inputs[index];
    state->multiline=1;
    snprintf(state->text,sizeof(state->text),"%s",value);
    state->select_all=0;
    if(state->edit_hwnd)SetWindowTextA(state->edit_hwnd,state->text);
    return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_panel_color(VntValue *x,VntValue *y,VntValue *w,VntValue *h,VntValue *color){
    int xx=vnt_gui_require_int(x,"gui_panel_color()"),yy=vnt_gui_require_int(y,"gui_panel_color()"),ww=vnt_gui_require_int(w,"gui_panel_color()"),hh=vnt_gui_require_int(h,"gui_panel_color()");
    VntGuiCommand *c=vnt_gui_add(VG_PANEL,xx,yy,ww,hh,NULL);if(!c)return vnt_bool(0);c->background=vnt_gui_color(vnt_gui_require_int(color,"gui_panel_color()"));c->radius=12;return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_text_style(VntValue *text,VntValue *x,VntValue *y,VntValue *color,VntValue *size){
    const char *s=vnt_app_string(text,"gui_text_style()");int xx=vnt_gui_require_int(x,"gui_text_style()"),yy=vnt_gui_require_int(y,"gui_text_style()"),font=vnt_gui_require_int(size,"gui_text_style()");
    if(font<8||font>72)font=16;VntGuiCommand *c=vnt_gui_add(VG_TEXT,xx,yy,0,0,s);if(!c)return vnt_bool(0);c->font_size=font;c->color=vnt_gui_color(vnt_gui_require_int(color,"gui_text_style()"));return vnt_bool(1);
}

VNT_NATIVE_ABI VntValue * vnt_gui_checkbox(VntValue *label,VntValue *x,VntValue *y,VntValue *checked){
    const char *text=vnt_app_string(label,"gui_checkbox()");int xx=vnt_gui_require_int(x,"gui_checkbox()"),yy=vnt_gui_require_int(y,"gui_checkbox()");
    int on=checked&&(checked->type==VNT_BOOL?checked->boolean:checked->type==VNT_INT?checked->integer:0);
    RECT r={xx,yy,xx+24,yy+24};if(vnt_gui_clicked&&vnt_gui_click_x>=xx&&vnt_gui_click_x<xx+250&&vnt_gui_click_y>=yy&&vnt_gui_click_y<yy+28)on=!on;
    VntGuiCommand *c=vnt_gui_add(VG_CHECKBOX,xx,yy,24,24,text);if(c)c->checked=on;
    return vnt_bool(on);
}
VNT_NATIVE_ABI VntValue * vnt_gui_progress(VntValue *value,VntValue *maximum,VntValue *x,VntValue *y){
    int v=vnt_gui_require_int(value,"gui_progress()"),m=vnt_gui_require_int(maximum,"gui_progress()"),xx=vnt_gui_require_int(x,"gui_progress()"),yy=vnt_gui_require_int(y,"gui_progress()");
    VntGuiCommand *c=vnt_gui_add(VG_PROGRESS,xx,yy,260,10,NULL);if(!c)return vnt_bool(0);c->value=v;c->maximum=m;return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_separator(VntValue *x,VntValue *y,VntValue *width){
    int xx=vnt_gui_require_int(x,"gui_separator()"),yy=vnt_gui_require_int(y,"gui_separator()"),ww=vnt_gui_require_int(width,"gui_separator()");
    VntGuiCommand *c=vnt_gui_add(VG_SEPARATOR,xx,yy,ww,1,NULL);return vnt_bool(c!=NULL);
}
VNT_NATIVE_ABI VntValue * vnt_gui_fill(VntValue *color){vnt_gui_background=vnt_gui_color(vnt_gui_require_int(color,"gui_fill()"));return vnt_bool(1);}
VNT_NATIVE_ABI VntValue * vnt_gui_rect(VntValue *color){VntGuiCommand *c=vnt_gui_add(VG_RECT,32,64,160,80,NULL);if(!c)return vnt_bool(0);c->background=vnt_gui_color(vnt_gui_require_int(color,"gui_rect()"));return vnt_bool(1);}
VNT_NATIVE_ABI VntValue * vnt_gui_poll(void){
    if(!vnt_gui_hwnd)return vnt_bool(0);MSG msg;
    vnt_gui_clicked=0;
    /* Keep native edit controls visible between frames to avoid flashing.
       The frame marks active controls; present() hides only controls not used. */
    for(int i=0;i<vnt_gui_input_count;i++)vnt_gui_inputs[i].seen_this_frame=0;
    while(PeekMessageA(&msg,NULL,0,0,PM_REMOVE)){if(msg.message==WM_QUIT){vnt_gui_hwnd=NULL;return vnt_bool(0);}TranslateMessage(&msg);DispatchMessageA(&msg);}
    vnt_gui_command_count=0;vnt_gui_text_y=18;
    return vnt_bool(vnt_gui_hwnd!=NULL);
}
VNT_NATIVE_ABI VntValue * vnt_gui_present(void){
    if(!vnt_gui_hwnd)return vnt_bool(0);
    for(int i=0;i<vnt_gui_input_count;i++) {
        VntGuiInputState *state=&vnt_gui_inputs[i];
        if(state->edit_hwnd && !state->seen_this_frame && IsWindowVisible(state->edit_hwnd)) {
            if(GetFocus()==state->edit_hwnd)SetFocus(vnt_gui_hwnd);
            ShowWindow(state->edit_hwnd,SW_HIDE);
        }
    }
    RedrawWindow(vnt_gui_hwnd,NULL,NULL,RDW_INVALIDATE|RDW_UPDATENOW|RDW_NOCHILDREN);
    return vnt_bool(1);
}
VNT_NATIVE_ABI VntValue * vnt_gui_key(void){int key=vnt_gui_last_key;vnt_gui_last_key=0;return vnt_int(key);}
VNT_NATIVE_ABI VntValue * vnt_gui_close(void){if(vnt_gui_hwnd)DestroyWindow(vnt_gui_hwnd);vnt_gui_hwnd=NULL;vnt_gui_command_count=0;return vnt_bool(1);}
#else
VNT_NATIVE_ABI VntValue * vnt_gui_css(VntValue *path) { (void)path;fprintf(stderr,"Runtime error: native GUI is currently supported on Windows only.\n");return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_size(VntValue *width,VntValue *height) { (void)width;(void)height;fprintf(stderr,"Runtime error: native GUI is currently supported on Windows only.\n");return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_open(VntValue *title) { (void)title;fprintf(stderr,"Runtime error: native GUI is currently supported on Windows only.\n");return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_text(VntValue *text) { (void)text;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_text_at(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_title(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_panel(VntValue *x,VntValue *y,VntValue *w,VntValue *h) { (void)x;(void)y;(void)w;(void)h;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_fill(VntValue *color) { (void)color;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_rect(VntValue *color) { (void)color;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_button(VntValue *label,VntValue *x,VntValue *y) { (void)label;(void)x;(void)y;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_button_sized(VntValue *label,VntValue *x,VntValue *y,VntValue *w,VntValue *h) { (void)label;(void)x;(void)y;(void)w;(void)h;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_input(VntValue *label,VntValue *x,VntValue *y,VntValue *w) { (void)label;(void)x;(void)y;(void)w;return vnt_string(""); }
VNT_NATIVE_ABI VntValue * vnt_gui_textarea(VntValue *label,VntValue *x,VntValue *y,VntValue *w,VntValue *h) { (void)label;(void)x;(void)y;(void)w;(void)h;return vnt_string(""); }
VNT_NATIVE_ABI VntValue * vnt_gui_input_set(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_textarea_set(VntValue *text,VntValue *x,VntValue *y) { (void)text;(void)x;(void)y;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_panel_color(VntValue *x,VntValue *y,VntValue *w,VntValue *h,VntValue *c) { (void)x;(void)y;(void)w;(void)h;(void)c;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_text_style(VntValue *t,VntValue *x,VntValue *y,VntValue *c,VntValue *s) { (void)t;(void)x;(void)y;(void)c;(void)s;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_checkbox(VntValue *label,VntValue *x,VntValue *y,VntValue *checked) { (void)label;(void)x;(void)y;(void)checked;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_progress(VntValue *v,VntValue *m,VntValue *x,VntValue *y) { (void)v;(void)m;(void)x;(void)y;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_separator(VntValue *x,VntValue *y,VntValue *w) { (void)x;(void)y;(void)w;return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_present(void) { return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_poll(void) { return vnt_bool(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_key(void) { return vnt_int(0); }
VNT_NATIVE_ABI VntValue * vnt_gui_close(void) { return vnt_bool(1); }
#endif


/* Native networking bridge. Transport implementation lives in native_net.c. */
extern char *vnt_native_http_request(const char *method, const char *url,
                                     const char *headers, const char *body,
                                     int *status, size_t *response_length);
extern char *vnt_native_http_request_bytes(const char *method, const char *url,
                                     const char *headers, const char *body, size_t body_length,
                                     int *status, size_t *response_length);
extern int vnt_native_http_download_stream(const char *url, const char *destination,
                                           const char *headers, int *status);
extern char *vnt_native_url_encode(const char *text);
extern int vnt_native_http_async_start(const char *method, const char *url, const char *headers, const char *body);
extern int vnt_native_http_async_poll(int handle);
extern int vnt_native_http_async_wait(int handle, unsigned int timeout_ms);
extern int vnt_native_http_async_cancel(int handle);
extern char *vnt_native_http_async_result(int handle, size_t *length);
extern int vnt_native_http_async_status(int handle);
extern int vnt_native_http_async_release(int handle);
extern int vnt_native_http_async_stream_start(const char *method, const char *url, const char *headers, const char *body);
extern unsigned char *vnt_native_http_async_stream_read(int handle, size_t maximum, size_t *out_length);
extern int vnt_native_http_async_stream_done(int handle);

static const char *vnt_net_string(VntValue *value, const char *function_name) {
    if (!value || value->type != VNT_STRING) {
        fprintf(stderr, "Runtime error: %s expects string arguments.\n", function_name);
        return "";
    }
    return value->string ? value->string : "";
}

VNT_NATIVE_ABI VntValue *vnt_net_request(VntValue *method, VntValue *url,
                                         VntValue *headers, VntValue *body) {
    int status = 0;
    size_t response_length = 0;
    char *response = vnt_native_http_request(
        vnt_net_string(method, "net_request()"),
        vnt_net_string(url, "net_request()"),
        vnt_net_string(headers, "net_request()"),
        vnt_net_string(body, "net_request()"), &status, &response_length);
    if (!response) return vnt_string("");
    VntValue *result = vnt_string(response);
    free(response);
    return result;
}

VNT_NATIVE_ABI VntValue *vnt_net_request_bytes(VntValue *method, VntValue *url,
                                               VntValue *headers, VntValue *body) {
    if (!body || (body->type != VNT_ARRAY && body->type != VNT_BYTES)) {
        fprintf(stderr, "Runtime error: net_request_bytes() expects an array of byte integers or a byte buffer as its body.\n");
        return vnt_bytes_new(NULL, 0);
    }
    size_t body_length = body->type == VNT_BYTES ? body->bytes.length : (size_t)body->array.count;
    const unsigned char *body_data = NULL;
    unsigned char *owned_body_data = NULL;
    if (body->type == VNT_BYTES) {
        body_data = body->bytes.data;
    } else {
        owned_body_data = (unsigned char *)malloc(body_length ? body_length : 1);
        if (!owned_body_data) return vnt_bytes_new(NULL, 0);
        for (size_t i = 0; i < body_length; ++i) {
            VntValue *item = body->array.items[i];
            if (!item || item->type != VNT_INT || item->integer < 0 || item->integer > 255) {
                fprintf(stderr, "Runtime error: net_request_bytes() body entries must be integers from 0 to 255.\n");
                free(owned_body_data);
                return vnt_bytes_new(NULL, 0);
            }
            owned_body_data[i] = (unsigned char)item->integer;
        }
        body_data = owned_body_data;
    }
    int status = 0;
    size_t response_length = 0;
    char *response = vnt_native_http_request_bytes(
        vnt_net_string(method, "net_request_bytes()"),
        vnt_net_string(url, "net_request_bytes()"),
        vnt_net_string(headers, "net_request_bytes()"),
        (const char *)body_data, body_length, &status, &response_length);
    free(owned_body_data);
    VntValue *result = vnt_bytes_new((const unsigned char *)response, response ? response_length : 0);
    free(response);
    return result;
}
VNT_NATIVE_ABI VntValue *vnt_net_status(VntValue *method, VntValue *url,
                                        VntValue *headers) {
    int status = 0;
    size_t response_length = 0;
    char *response = vnt_native_http_request(
        vnt_net_string(method, "net_status()"),
        vnt_net_string(url, "net_status()"),
        vnt_net_string(headers, "net_status()"), "", &status, &response_length);
    free(response);
    char code[16];
    snprintf(code, sizeof(code), "%03d", status);
    return vnt_string(code);
}

VNT_NATIVE_ABI VntValue *vnt_net_download(VntValue *url, VntValue *destination,
                                          VntValue *headers) {
    int status = 0;
    int ok = vnt_native_http_download_stream(
        vnt_net_string(url, "net_download()"),
        vnt_net_string(destination, "net_download()"),
        vnt_net_string(headers, "net_download()"), &status);
    return vnt_bool(ok);
}

VNT_NATIVE_ABI VntValue *vnt_net_async_start(VntValue *method, VntValue *url,
                                                    VntValue *headers, VntValue *body) {
    int handle = vnt_native_http_async_start(vnt_net_string(method, "net_async_start()"),
        vnt_net_string(url, "net_async_start()"),
        vnt_net_string(headers, "net_async_start()"),
        vnt_net_string(body, "net_async_start()"));
    return vnt_int(handle);
}
VNT_NATIVE_ABI VntValue *vnt_net_async_poll(VntValue *handle) {
    return vnt_int(vnt_native_http_async_poll(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_async_wait(VntValue *handle, VntValue *timeout) {
    unsigned int ms = timeout && timeout->type == VNT_INT && timeout->integer > 0
        ? (unsigned int)timeout->integer : 0;
    return vnt_bool(vnt_native_http_async_wait(handle && handle->type == VNT_INT ? handle->integer : 0, ms) == 1);
}
VNT_NATIVE_ABI VntValue *vnt_net_async_cancel(VntValue *handle) {
    return vnt_bool(vnt_native_http_async_cancel(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_async_result(VntValue *handle) {
    size_t length = 0;
    char *response = vnt_native_http_async_result(handle && handle->type == VNT_INT ? handle->integer : 0, &length);
    if (!response) return vnt_string("");
    VntValue *result = vnt_string(response);
    free(response);
    return result;
}
VNT_NATIVE_ABI VntValue *vnt_net_async_status(VntValue *handle) {
    return vnt_int(vnt_native_http_async_status(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_async_release(VntValue *handle) {
    return vnt_bool(vnt_native_http_async_release(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_stream_start(VntValue *method, VntValue *url,
                                              VntValue *headers, VntValue *body) {
    int handle = vnt_native_http_async_stream_start(vnt_net_string(method, "net_stream_start()"),
        vnt_net_string(url, "net_stream_start()"),
        vnt_net_string(headers, "net_stream_start()"),
        vnt_net_string(body, "net_stream_start()"));
    return vnt_int(handle);
}
VNT_NATIVE_ABI VntValue *vnt_net_stream_read(VntValue *handle, VntValue *maximum) {
    size_t length = 0;
    size_t amount = maximum && maximum->type == VNT_INT && maximum->integer > 0
        ? (size_t)maximum->integer : 65536u;
    if (amount > 65536u) amount = 65536u;
    unsigned char *data = vnt_native_http_async_stream_read(
        handle && handle->type == VNT_INT ? handle->integer : 0, amount, &length);
    VntValue *result = vnt_bytes_new(data, data ? length : 0);
    free(data);
    return result;
}
VNT_NATIVE_ABI VntValue *vnt_net_stream_done(VntValue *handle) {
    return vnt_bool(vnt_native_http_async_stream_done(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_stream_status(VntValue *handle) {
    return vnt_int(vnt_native_http_async_status(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_stream_cancel(VntValue *handle) {
    return vnt_bool(vnt_native_http_async_cancel(handle && handle->type == VNT_INT ? handle->integer : 0));
}
VNT_NATIVE_ABI VntValue *vnt_net_stream_release(VntValue *handle) {
    return vnt_bool(vnt_native_http_async_release(handle && handle->type == VNT_INT ? handle->integer : 0));
}

VNT_NATIVE_ABI VntValue *vnt_net_url_encode(VntValue *text) {
    char *encoded = vnt_native_url_encode(vnt_net_string(text, "net_url_encode()"));
    if (!encoded) return vnt_string("");
    VntValue *result = vnt_string(encoded);
    free(encoded);
    return result;
}


/*
 * Raw memory primitives for low-level VNT code.
 * These APIs intentionally do not prove allocation bounds or pointer lifetime.
 * Callers must only use pointers returned by mem_alloc/mem_realloc or otherwise
 * obtained from a valid native allocation/mapping, and must respect its size.
 */
static VntValue *vnt_pointer_value(void *pointer) {
    VntValue *value = alloc_value(VNT_POINTER);
    value->pointer = pointer;
    return value;
}

static void *vnt_require_pointer(VntValue *value) {
    if (!value || value->type != VNT_POINTER) {
        fprintf(stderr, "Runtime error: memory operation requires ptr.\n");
        exit(1);
    }
    return value->pointer;
}

static size_t vnt_require_memory_size(VntValue *value, int allow_zero) {
    if (!value || value->type != VNT_INT || value->integer < 0 ||
        (!allow_zero && value->integer == 0)) {
        fprintf(stderr, "Runtime error: memory size/offset must be a valid non-negative int.\n");
        exit(1);
    }
    return (size_t)value->integer;
}

static void *vnt_memory_address(VntValue *pointer, VntValue *offset,
                                size_t width) {
    uintptr_t base = (uintptr_t)vnt_require_pointer(pointer);
    size_t displacement = vnt_require_memory_size(offset, 1);
    if (!base || displacement > UINTPTR_MAX - base ||
        width > UINTPTR_MAX - (base + displacement)) {
        fprintf(stderr, "Runtime error: null pointer or address overflow.\n");
        exit(1);
    }
    return (void *)(base + displacement);
}

VNT_NATIVE_ABI VntValue *vnt_mem_alloc(VntValue *size_value) {
    size_t size = vnt_require_memory_size(size_value, 0);
    void *memory = calloc(1, size);
    return vnt_pointer_value(memory);
}

VNT_NATIVE_ABI VntValue *vnt_mem_free(VntValue *pointer) {
    free(vnt_require_pointer(pointer));
    pointer->pointer = NULL;
    return vnt_bool(1);
}

VNT_NATIVE_ABI VntValue *vnt_mem_realloc(VntValue *pointer, VntValue *size_value) {
    void *old_memory = vnt_require_pointer(pointer);
    size_t size = vnt_require_memory_size(size_value, 0);
    void *memory = realloc(old_memory, size);
    /* C realloc preserves old_memory on failure; expose null without losing it. */
    return vnt_pointer_value(memory);
}

VNT_NATIVE_ABI VntValue *vnt_mem_ptr_add(VntValue *pointer, VntValue *offset_value) {
    uintptr_t base = (uintptr_t)vnt_require_pointer(pointer);
    int64_t offset = offset_value && offset_value->type == VNT_INT
        ? offset_value->integer : 0;
    uint64_t magnitude = offset < 0
        ? (uint64_t)(-(offset + 1)) + 1u : (uint64_t)offset;
    if (!base || !offset_value || offset_value->type != VNT_INT ||
        (offset >= 0 && magnitude > UINTPTR_MAX - base) ||
        (offset < 0 && magnitude > base)) {
        fprintf(stderr, "Runtime error: invalid pointer offset.\n");
        exit(1);
    }
    return vnt_pointer_value((void *)(offset >= 0
        ? base + (uintptr_t)magnitude
        : base - (uintptr_t)magnitude));
}

#define VNT_MEM_READ(name, ctype) \
VNT_NATIVE_ABI VntValue *vnt_mem_read_##name(VntValue *pointer, VntValue *offset) { \
    ctype value; memcpy(&value, vnt_memory_address(pointer, offset, sizeof(value)), sizeof(value)); \
    return vnt_int((int64_t)value); \
}
VNT_MEM_READ(u8, uint8_t)
VNT_MEM_READ(u32, uint32_t)
VNT_MEM_READ(u64, uint64_t)
#undef VNT_MEM_READ

#define VNT_MEM_WRITE(name, ctype) \
VNT_NATIVE_ABI VntValue *vnt_mem_write_##name(VntValue *pointer, VntValue *offset, VntValue *input) { \
    if (!input || input->type != VNT_INT) { \
        fprintf(stderr, "Runtime error: memory write value must be int.\n"); exit(1); \
    } \
    ctype value = (ctype)input->integer; \
    memcpy(vnt_memory_address(pointer, offset, sizeof(value)), &value, sizeof(value)); \
    return vnt_bool(1); \
}
VNT_MEM_WRITE(u8, uint8_t)
VNT_MEM_WRITE(u32, uint32_t)
VNT_MEM_WRITE(u64, uint64_t)
#undef VNT_MEM_WRITE

VNT_NATIVE_ABI VntValue *vnt_mem_copy(VntValue *destination, VntValue *source,
                                      VntValue *length_value) {
    size_t length = vnt_require_memory_size(length_value, 1);
    void *dst = vnt_memory_address(destination, vnt_int(0), length);
    void *src = vnt_memory_address(source, vnt_int(0), length);
    memmove(dst, src, length);
    return vnt_bool(1);
}

VNT_NATIVE_ABI VntValue *vnt_mem_set(VntValue *destination, VntValue *byte_value,
                                     VntValue *length_value) {
    if (!byte_value || byte_value->type != VNT_INT ||
        byte_value->integer < 0 || byte_value->integer > 255) {
        fprintf(stderr, "Runtime error: mem_set byte must be in range 0..255.\n");
        exit(1);
    }
    size_t length = vnt_require_memory_size(length_value, 1);
    memset(vnt_memory_address(destination, vnt_int(0), length),
           byte_value->integer, length);
    return vnt_bool(1);
}


VNT_NATIVE_ABI VntValue *vnt_mem_is_null(VntValue *pointer) {
    return vnt_bool(vnt_require_pointer(pointer) == NULL);
}
