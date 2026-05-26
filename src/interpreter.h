#ifndef INTERPRETER_H
#define INTERPRETER_H
#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

#include "expr.h"
#include "stmt.h"
#include "tokenType.h"
#include "runtimeError.h"
#include "value.h"
#include "tokenType.h"
#include "environment.h"

// Forward declaration of the expression evaluation function.
void *evaluate(Expr *expr, Environment *env);
extern int hadRuntimeError; // Flag to indicate if a runtime error has occurred
void *execute(Stmt *stmt, Environment *env);
Environment *create_environment();
void free_environment(Environment *env);
// Main function to interpret an expression and print its result. This is the entry point for the interpreter.
void interpreter(struct Stmt *stmt, Environment *env);


#endif // INTERPRETER_H