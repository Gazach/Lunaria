// value.h - Defines the Value type used to represent values in the interpreter.
#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

// general types for values in the interpreter
typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOLEAN,
    TYPE_FLOAT,
    TYPE_CHARACTER,
    TYPE_VOID,
    TYPE_NULL,

    TYPE_FUNCTION, // for function values
} LunarisType;

// function values 
typedef struct {
    char **param_names;
    int param_count;
    struct Stmt **body;
    int body_count;
    struct Environment *closure; // The environment where the function was defined, for closures
} FunctionValue;


typedef struct {
    LunarisType type;
    union {
        int int_value;
        double float_value;
        const char *string;
        bool boolean;
        char character;
        void *void_value;
        FunctionValue *function;
    } as;
} Value;

#endif // VALUE_H