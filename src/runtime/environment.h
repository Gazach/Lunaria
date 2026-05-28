#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../common/value.h"

typedef struct Environment Environment;

typedef struct {
    char *name;
    Value value;
} Entry;

struct Environment{
    Entry *entries;
    int count;
    int capacity;
    struct Environment *parent; // Pointer to the parent environment for nested scopes
};

#endif // ENVIRONMENT_H