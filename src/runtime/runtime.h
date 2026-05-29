#ifndef RUNTIME_H
#define RUNTIME_H
#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

#include "../frontend/ast/expr.h"
#include "../frontend/ast/stmt.h"
#include "../common/tokenType.h"
#include "runtimeError.h"
#include "../common/value.h"
#include "../common/tokenType.h"
#include "environment.h"
#include "builtIn.h"

typedef struct {
    bool is_return;
    Value return_value;
} returnSignal;

// Forward declaration of the expression evaluation function.
void *evaluate(Expr *expr, Environment *env, struct Stmt *stmt);
extern int hadRuntimeError; // Flag to indicate if a runtime error has occurred
void *execute(Stmt *stmt, Environment *env);
Environment *env_create_child(Environment *parent);
void free_environment(Environment *env);
// Main function to interpret an expression and print its result. This is the entry point for the interpreter.
void interpreter(struct Stmt *stmt, Environment *env);
char *stringifyValue(Value *value);

#endif // RUNTIME_H