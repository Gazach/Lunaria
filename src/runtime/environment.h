#ifndef ENVIRONMENT_H
#define ENVIRONMENT_H

#include "../common/value.h"
#include <stdbool.h>

typedef struct Environment Environment;

typedef struct {
    char *name;
    Value value;
    bool is_mutable; 
} Entry;

struct Environment{
    Entry *entries;
    int count;
    int capacity;
    struct Environment *parent; // Pointer to the parent environment for nested scopes
    struct Environment *static_scope; // Nearest enclosing function's persistent
                                       // store for `static` locals, or NULL if
                                       // there isn't one (e.g. global scope).
};

// exposed functions for environment management
Environment *create_environment();
void *env_set_variable(Environment *env, const char *name, bool is_mutable, Value value);
Value *env_get_variable(Environment *env, const char *name);
void env_update_variable(Environment *env, const char *name, Value value);
void free_environment(Environment *env);


#endif // ENVIRONMENT_H