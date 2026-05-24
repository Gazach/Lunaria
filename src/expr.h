#ifndef EXPR_H
#define EXPR_H

typedef struct Expr Expr;

typedef enum { //define expression types
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL, // New expression type for function calls
    EXPR_GROUPING, // New expression type for grouping parentheses like (a + b)
    EXPR_INDEX, // New expression type for indexing which arrays and lists like arr[0]
    EXPR_MEMBER_ACCESS, // New expression type for member access like obj.field
    EXPR_TERNARY, // New expression type for ternary operator like condition ? expr1 : expr2
} ExprType;

struct Expr { // define the structure of an expression
    ExprType type;
    struct Expr *left;
    struct Expr *right;
    union {
        struct {
            double value;
        } literal;
        struct {
            char *name;
        } variable;
        struct {
            char op;
            struct Expr *left;
            struct Expr *right;
        } binary;
        struct {
            char op;
            struct Expr *operand;
        } unary;
        struct {
            char *name;
            struct Expr **args;
            int arg_count;
        } call;
        struct {
            struct Expr *expression;
        } grouping;
        struct {
            struct Expr *array;
            struct Expr *index;
        } index;
        struct {
            struct Expr *object;
            char *member;
        } member_access;
        struct {
            struct Expr *condition;
            struct Expr *then_branch;
            struct Expr *else_branch;
        } ternary;

    };
};


void expr_free(struct Expr *expr);

#endif