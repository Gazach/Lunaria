// value.h - Defines the Value type used to represent values in the interpreter.
#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum {
    TYPE_INT,
    TYPE_STRING,
    TYPE_BOOLEAN,
    TYPE_FLOAT,
    TYPE_CHARACTER,
    TYPE_VOID,
    TYPE_NULL,
} LunarisType;

typedef struct {
    LunarisType type;
    union {
        int int_value;
        double float_value;
        const char *string;
        bool boolean;
        char character;
        void *void_value;
    } as;
} Value;

#endif // VALUE_H