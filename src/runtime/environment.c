#include "environment.h"
#include "runtime.h"
#include <stdlib.h>
#include <string.h>
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
    return env;
}

// Set a variable in the environment with the given name and value. This will be used to create new variable bindings or update existing ones during interpretation.
void *env_set_variable(Environment *env, const char *name, Value value) {
    // Check if the variable already exists in the current environment
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
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
    env->entries[env->count].name = STRDUP(name);
    env->entries[env->count].value = value;
    env->count++;
    return NULL;
}

// Get the value of a variable from the environment by its name. This will be used to retrieve variable values during interpretation.
Value *env_get_variable(Environment *env, const char *name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            return &env->entries[i].value; // Return pointer to the variable's value
        }
    }
    if (env->parent != NULL) {
        return env_get_variable(env->parent, name); // Look in parent environment
    }
    return NULL; // Variable not found
}

// Free the memory used by the environment and its entries. This should be called when the environment is no longer needed to avoid memory leaks.
void free_environment(Environment *env) {
    for (int i = 0; i < env->count; i++) {
        free(env->entries[i].name); // Free variable names
        if (env->entries[i].value.type == TYPE_STRING) {
            free((char *)env->entries[i].value.as.string); // Free string values
        }
    }
    free(env->entries); // Free entries array
    free(env); // Free the environment itself
}
