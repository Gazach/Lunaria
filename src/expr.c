#include "expr.h"
#include <stdlib.h>

// Create a new expression node with the given type and left/right sub-expressions.
struct Expr *expr_new(ExprType type, struct Expr *left, struct Expr *right) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = type;
    e->left             = left;
    e->right            = right;
    return e;
}

// Create a new literal expression with the given value.
struct Expr *expr_literal(double value) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_LITERAL;
    e->literal.value    = value;
    return e;
}

struct Expr *expr_string(const char *value) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_STRING;
    e->string.value     = value;
    return e;
}

// Create a new variable expression with the given name.
struct Expr *expr_variable(char *name) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_VARIABLE;
    e->variable.name    = name;
    return e;
}

// Create a new binary expression with the given operator and left/right sub-expressions.
struct Expr *expr_binary(TokenType op, struct Expr *left, struct Expr *right) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_BINARY;
    e->binary.op        = op;
    e->binary.left      = left;
    e->binary.right     = right;
    return e;
}

// Create a new unary expression with the given operator and operand.
struct Expr *expr_unary(TokenType op, struct Expr *operand) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_UNARY;
    e->unary.op         = op;
    e->unary.operand    = operand;
    return e;
}

// Create a new function call expression with the given name and arguments.
struct Expr *expr_call(char *name, struct Expr **args, int arg_count) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_CALL;
    e->call.name        = name;
    e->call.args        = args;
    e->call.arg_count   = arg_count;
    return e;
}

// Create a new grouping expression with the given sub-expression.
struct Expr *expr_grouping(struct Expr *expression) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_GROUPING;
    e->left             = expression;
    return e;
}

// Create a new index expression with the given array and index sub-expressions.
struct Expr *expr_index(struct Expr *array, struct Expr *index) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_INDEX;
    e->index.array      = array;
    e->index.index      = index;
    return e;
}

// Create a new member access expression with the given object and member name.
struct Expr *expr_member_access(struct Expr *object, char *member) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_MEMBER_ACCESS;
    e->member_access.object = object;
    e->member_access.member = member;
    return e;
}

struct Expr *expr_ternary(struct Expr *condition, struct Expr *then_branch, struct Expr *else_branch) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_TERNARY;
    e->ternary.condition = condition;
    e->ternary.then_branch = then_branch;
    e->ternary.else_branch = else_branch;
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
        case EXPR_GROUPING:
            expr_free(e->left);
            break;
        case EXPR_INDEX:
            expr_free(e->index.array);
            expr_free(e->index.index);
            break;
        case EXPR_MEMBER_ACCESS:
            expr_free(e->member_access.object);
            free(e->member_access.member);
            break;
        case EXPR_TERNARY:
            expr_free(e->ternary.condition);
            expr_free(e->ternary.then_branch);
            expr_free(e->ternary.else_branch);
            break;
        default:
            break;
    }
    free(e);
}

