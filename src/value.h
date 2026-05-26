// value.h - Defines the Value type used to represent values in the interpreter.
#ifndef VALUE_H
#define VALUE_H

#include <stdbool.h>

typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_BOOLEAN,
    VAL_NULL,
} ValueType;

typedef struct {
    ValueType type;
    union {
        double number;
        const char *string;
        bool boolean;
    } as;
} Value;

#endif // VALUE_H