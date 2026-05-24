#ifndef EXPR_H
#define EXPR_H

typedef struct Expr Expr;

typedef enum { //define expression types
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_CALL,
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
    };
};

#endif