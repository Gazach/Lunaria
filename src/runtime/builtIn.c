#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "builtIn.h"
#include "runtimeError.h" 

#include "runtime.h"

// Forward declaration of the expression evaluation function.
char *stringifyValue(Value *value);
// =========================
// built-in functions
// =========================

// make built-in functions for print
static Value *builtin_print(Value **args, int arg_count) {
    // printf("builtin_print called, arg_count=%d\n", arg_count);
    for (int i = 0; i < arg_count; i++) {
        // printf("arg[%d] type=%d\n", i, args[i] ? args[i]->type : -1);
        // fflush(stdout);
        char *str = stringifyValue(args[i]);
        printf("%s", str);
        free(str);
        if (i < arg_count - 1) printf(" ");
    }
    printf("\n");
    Value *result = malloc(sizeof(Value));
    result->type = TYPE_NULL;
    return result;
}

// check the type of the argument and return a string representation of it
static Value *builtin_type(Value **args, int arg_count) {
    if (arg_count != 1) return NULL;
    Value *result = malloc(sizeof(Value));
    result->type = TYPE_STRING;
    switch (args[0]->type) {
        case TYPE_INT:      result->as.string = STRDUP("int");      break;
        case TYPE_FLOAT:    result->as.string = STRDUP("float");    break;
        case TYPE_STRING:   result->as.string = STRDUP("string");   break;
        case TYPE_BOOLEAN:  result->as.string = STRDUP("bool");     break;
        case TYPE_NULL:     result->as.string = STRDUP("null");     break;
        case TYPE_FUNCTION: result->as.string = STRDUP("function"); break;
        case TYPE_ARRAY:    result->as.string = STRDUP("array");    break;
        default:            result->as.string = STRDUP("unknown");  break;
    }
    return result;
}

// return the number of seconds elapsed since the program started, as a float.
// same idea as Lox's native clock() from Crafting Interpreters: mainly meant
// for benchmarking scripts, e.g. clock() before/after a loop and subtracting.
static Value *builtin_clock(Value **args, int arg_count) {
    if (arg_count != 0) return NULL;
    Value *result = malloc(sizeof(Value));
    result->type = TYPE_FLOAT;
    result->as.float_value = (double)clock() / CLOCKS_PER_SEC;
    return result;
}

// same as clock(), but in milliseconds instead of seconds — nicer for eyeballing
// fast loops without staring at 0.0001234.
static Value *builtin_clockMs(Value **args, int arg_count) {
    if (arg_count != 0) return NULL;
    Value *result = malloc(sizeof(Value));
    result->type = TYPE_FLOAT;
    result->as.float_value = ((double)clock() / CLOCKS_PER_SEC) * 1000.0;
    return result;
}

// return the number of elements in an array, or the number of characters in
// a string. This is what makes arrays actually loopable — without it there's
// no way to know where to stop indexing.
static Value *builtin_len(Value **args, int arg_count) {
    if (arg_count != 1) return NULL;
    Value *result = malloc(sizeof(Value));
    result->type = TYPE_INT;
    if (args[0]->type == TYPE_ARRAY) {
        result->as.int_value = args[0]->as.array->count;
    } else if (args[0]->type == TYPE_STRING) {
        result->as.int_value = args[0]->as.string ? (int)strlen(args[0]->as.string) : 0;
    } else {
        runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "len() expects an array or a string.", NULL);
        free(result);
        return NULL;
    }
    return result;
}

// append a value onto an array in place, Rust's Vec::push style — mutates
// the array and returns null, rather than returning a new array. Arrays are
// shared by reference (see eval_variable's shallow copy for TYPE_ARRAY), so
// this is visible through every alias of the same array, including whatever
// the array was passed in or returned as.
static Value *builtin_push(Value **args, int arg_count) {
    if (arg_count != 2) return NULL;
    if (args[0]->type != TYPE_ARRAY) {
        runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "push() expects an array as its first argument.", NULL);
        return NULL;
    }
    ArrayValue *arr = args[0]->as.array;
    if (arr->count >= arr->capacity) {
        int newCapacity = arr->capacity == 0 ? 8 : arr->capacity * 2;
        arr->elements = realloc(arr->elements, newCapacity * sizeof(Value));
        arr->capacity = newCapacity;
    }
    arr->elements[arr->count++] = *args[1];

    Value *result = malloc(sizeof(Value));
    result->type = TYPE_NULL;
    return result;
}

// ========================
// builtin registry
// ========================

// Define a structure for built-in functions, which includes the function name and a pointer to the function implementation. This will be used to register built-in functions in the environment.
typedef struct {
    const char *name;
    Value *(*fn)(Value **args, int arg_count);
} Builtin;

// Create an array of built-in functions that can be called from the interpreted code. This will allow the interpreter to recognize and execute built-in functions when they are called.
static Builtin builtins[] = {
    { "print",   builtin_print   },
    { "type",    builtin_type    },
    { "clock",   builtin_clock   },
    { "clockMs", builtin_clockMs },
    { "len",     builtin_len     },
    { "push",    builtin_push    },
};

// Get the number of built-in functions in the registry. This will be used to iterate over the built-in functions when looking them up by name. 
static int builtin_count = sizeof(builtins) / sizeof(builtins[0]);

// Check if a given function name corresponds to a built-in function. This will be used to determine if a function call should be handled as a built-in function or as a user-defined function.
bool is_builtin(const char *name) {
    for (int i = 0; i < builtin_count; i++) {
        if (strcmp(builtins[i].name, name) == 0) return true;
    }
    return false;
}

// call a built-in function by its name with the given arguments. This will be used to execute built-in functions when they are called from the interpreted code.
Value *call_builtin(const char *name, Value **args, int arg_count) {
    for (int i = 0; i < builtin_count; i++) {
        if (strcmp(builtins[i].name, name) == 0) {
            return builtins[i].fn(args, arg_count);
        }
    }
    return NULL;
}