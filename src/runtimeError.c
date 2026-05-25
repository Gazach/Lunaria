#include <stdio.h>
#include <stdlib.h>

#include "runtimeError.h"

extern int hadRuntimeError;

RuntimeError *runtime_error(Token token, const char *message) {
    hadRuntimeError = 1;
    RuntimeError *error = malloc(sizeof(RuntimeError));
    error->token = token;
    error->message = message;
    fprintf(stderr, "[Runtime Error] Line %d: %s\n", token.line, message);
    return error;
}

void free_runtime_error(RuntimeError *error, const char *message) {
    if (error) {
        free((void *)message); // Assuming message was dynamically allocated
        free(error);
    }
}