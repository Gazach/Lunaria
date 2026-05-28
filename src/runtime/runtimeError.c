#include <stdio.h>
#include <stdlib.h>

#include "runtimeError.h"

extern int hadRuntimeError;

// Create a new runtime error with the given token, message, and name. This function also sets the hadRuntimeError flag to indicate that an error has occurred.
RuntimeError *runtime_error(Token token, const char *message, const char *name) {
    hadRuntimeError = 1;
    RuntimeError *error = malloc(sizeof(RuntimeError));
    error->token = token;
    error->message = message;
    error->name = name;
    if (name) {
        fprintf(stderr, "[line %d] Runtime Error at '%s': %s\n", token.line, name, message);
    } else {
        fprintf(stderr, "[line %d] Runtime Error: %s\n", token.line, message);
    }
    return error;
}

// Free the memory used by a runtime error. This should be called when the error is no longer needed to avoid memory leaks.
void free_runtime_error(RuntimeError *error, const char *message) {
    if (error) {
        
        free(error);
    }
}