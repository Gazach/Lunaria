#include "expr.h"
#include <stdlib.h>

// Create a new expression node with the given type and left/right sub-expressions.
struct Expr *expr_new(ExprType type, struct Expr *left, struct Expr *right) {
    struct Expr *e = malloc(sizeof(struct Expr));
    e->type = type;
    e->left = left;
    e->right = right;
    return e;
}

// Create a new literal expression with the given value.
struct Expr *expr_literal(double value) {
    struct Expr *e = malloc(sizeof(struct Expr));
    e->type = EXPR_LITERAL;
    e->literal.value = value;
    return e;
}

// Create a new variable expression with the given name.
struct Expr *expr_variable(char *name) {
    struct Expr *e = malloc(sizeof(struct Expr));
    e->type = EXPR_VARIABLE;
    e->variable.name = name;
    return e;
}

// Create a new binary expression with the given operator and left/right sub-expressions.
struct Expr *expr_binary(char op, struct Expr *left, struct Expr *right) {
    struct Expr *e = malloc(sizeof(struct Expr));
    e->type = EXPR_BINARY;
    e->binary.op = op;
    e->binary.left = left;
    e->binary.right = right;
    return e;
}

// Create a new unary expression with the given operator and operand.
struct Expr *expr_unary(char op, struct Expr *operand) {
    struct Expr *e = malloc(sizeof(struct Expr));
    e->type = EXPR_UNARY;
    e->unary.op = op;
    e->unary.operand = operand;
    return e;
}

// Create a new function call expression with the given name and arguments.
struct Expr *expr_call(char *name, struct Expr **args, int arg_count) {
    struct Expr *e = malloc(sizeof(struct Expr));
    e->type = EXPR_CALL;
    e->call.name = name;
    e->call.args = args;
    e->call.arg_count = arg_count;
    return e;
}

// Free the memory allocated for an expression and its sub-expressions.
void expr_free(struct Expr *e) {
    if (!e) return;
    switch (e->type) {
        case EXPR_LITERAL:
            break;
        case EXPR_VARIABLE:
            free(e->variable.name);
            break;
        case EXPR_BINARY:
            expr_free(e->binary.left);
            expr_free(e->binary.right);
            break;
        case EXPR_UNARY:
            expr_free(e->unary.operand);
            break;
        case EXPR_CALL:
            free(e->call.name);
            for (int i = 0; i < e->call.arg_count; i++) {
                expr_free(e->call.args[i]);
            }
            free(e->call.args);
            break;
    }
    free(e);
}

