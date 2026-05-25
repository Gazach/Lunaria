#ifndef INTERPRETER_H
#define INTERPRETER_H
#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

#include "expr.h"
#include "tokenType.h"
#include "runtimeError.h"
#include "value.h"

// Forward declaration of the expression evaluation function.
void *evaluate(Expr *expr);
extern int hadRuntimeError; // Flag to indicate if a runtime error has occurred
// Main function to interpret an expression and print its result. This is the entry point for the interpreter.
void interpreter(struct Expr *expr);


#endif // INTERPRETER_H