#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
        default:            result->as.string = STRDUP("unknown");  break;
    }
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
    { "print", builtin_print },
    { "type",  builtin_type  },
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