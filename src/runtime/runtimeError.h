#ifndef RUNTIME_ERROR_H
#define RUNTIME_ERROR_H

#include "../common/token.h"
#include "environment.h"

// Runtime error handling for Lunaris Language
// Report a runtime error with a message and the line number where it occurred
typedef struct {
    Token token;
    const char *message;
    const char *name;
} RuntimeError;

//expose functions to create and free runtime errors
RuntimeError *runtime_error(Token token, const char *message, const char *name);
void free_runtime_error(RuntimeError *error, const char *message);
extern int hadRuntimeError; // Flag to indicate if a runtime error has occurred

#endif // RUNTIME_ERROR_H