#include "environment.h"
#include "runtime.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
// =========================
// Environment 
// ========================

// Initialize a new environment for variable storage and scope management. This will be used to store variable bindings and manage scopes during interpretation.
Environment *create_environment() {
    Environment *env = malloc(sizeof(Environment));
    env->entries = NULL;
    env->count = 0;
    env->capacity = 0;
    env->parent = NULL; // For global environment, parent is NULL
    env->static_scope = NULL; // No enclosing function by default
    return env;
}

//=================
// variable environment management
//=================

// Set a variable in the environment with the given name and value. This will be used to create new variable bindings or update existing ones during interpretation.
void *env_set_variable(Environment *env, const char *name, bool is_mutable, Value value) {
    // Check if the variable already exists in the current environment
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            if (!env->entries[i].is_mutable) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Cannot assign to immutable variable '%s'.", name);
                runtime_error((Token){.line = 0, .literal = name}, msg, NULL);
                return NULL;
            }
            env->entries[i].value = value; // Update existing variable
            return NULL;
        }
    }
    // If not found, add a new entry
    if (env->count >= env->capacity) {
        int newCapacity = env->capacity == 0 ? 8 : env->capacity * 2;
        env->entries = realloc(env->entries, newCapacity * sizeof(Entry));
        env->capacity = newCapacity;
    }
    env->entries[env->count].is_mutable = is_mutable;
    env->entries[env->count].name = STRDUP(name);
    env->entries[env->count].value = value;
    if (value.type == TYPE_STRING && value.as.string != NULL) {
        env->entries[env->count].value.as.string = STRDUP(value.as.string);
    }
    env->count++;
    return NULL;
}



// Get the value of a variable from the environment by its name. This will be used to retrieve variable values during interpretation.
Value *env_get_variable(Environment *env, const char *name) {
    if (env == NULL) {
        // printf("env_get: '%s' — env is NULL!\n", name);
        // fflush(stdout);
        return NULL;
    }

    // printf("env_get: '%s' env=%p parent=%p count=%d\n",
    //        name,
    //        (void*)env,
    //        (void*)env->parent,
    //        env->count);
    // fflush(stdout);

    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            // printf("FOUND '%s' in env %p\n", name, (void*)env);
            // fflush(stdout);
            return &env->entries[i].value;
        }
    }

    if (env->parent != NULL) {
        // printf("Going to parent %p\n", (void*)env->parent);
        // fflush(stdout);

        return env_get_variable(env->parent, name);
    }

    // printf("'%s' not found anywhere.\n", name);
    // fflush(stdout);

    return NULL;
}

// Update the value of an existing variable in the environment. This will be used to update variable values during interpretation, such as in assignment statements.
void env_update_variable(Environment *env, const char *name, Value value) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
             // printf("env_update: looking for '%s' in env=%p\n", name, (void*)env); 
             // fflush(stdout);
            if (!env->entries[i].is_mutable) {
                char msg[256];
                snprintf(msg, sizeof(msg), "Cannot assign to immutable variable '%s'.", name);
                runtime_error((Token){.line = 0, .literal = name}, msg, NULL);
                return;
            }
            // free old string if needed
            if (env->entries[i].value.type == TYPE_STRING) {
                free((char *)env->entries[i].value.as.string);
            }
            env->entries[i].value = value;
            if (value.type == TYPE_STRING && value.as.string != NULL) {
                env->entries[i].value.as.string = STRDUP(value.as.string);
            }
            return;
        }
    }
    // not found in current scope, check parent
    if (env->parent != NULL) {
        env_update_variable(env->parent, name, value);
    }
}

//=================
// Scope management
//=================

// simply create a new environment with the current environment as its parent.
Environment *env_create_child(Environment *parent) {
    Environment *child = create_environment();
    child->parent = parent;
    child->static_scope = parent->static_scope; // inherit; a function call
                                                 // site overrides this right
                                                 // after creation
    return child;
}

//=================
// Freeing environment
//=================

// Free the memory used by the environment and its entries. This should be called when the environment is no longer needed to avoid memory leaks.
void free_environment(Environment *env) {
    for (int i = 0; i < env->count; i++) {
        free(env->entries[i].name); // Free variable names
        if (env->entries[i].value.type == TYPE_STRING) {
            free((char *)env->entries[i].value.as.string); // Free string values
        }
        if (env->entries[i].value.type == TYPE_VOID) {
            free(env->entries[i].value.as.void_value); // Free void pointer values if needed
        }
    }
    free(env->entries); // Free entries array
    free(env); // Free the environment itself
}
