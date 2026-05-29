#ifndef EXPR_H
#define EXPR_H

#include "../../common/token.h"
#include "stdbool.h"
#include "../../common/tokenType.h"
#include "../../common/value.h"

typedef struct Expr Expr;

typedef enum { //define expression types
    EXPR_LITERAL,
    EXPR_VARIABLE,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_STRING,
    EXPR_BOOLEAN,
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
            TokenType type; // Store the type of the literal for better type checking during interpretation
        } literal;
        struct {
            char *name;
            LunarisType type;
            Token name_token; // Store the token for better error reporting
        } variable;
        struct {
            bool value;
        } boolean;
        struct {
            const char *value;
        } string;
        struct {
            TokenType op;
            struct Expr *left;
            struct Expr *right;
        } binary;
        struct {
            TokenType op;
            struct Expr *operand;
        } unary;
        struct {
            char *name;
            Token name_token; // Store the token for better error reporting
            LunarisType return_type;
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

// exposed functions for creating and freeing expressions
void expr_free(struct Expr *expr);

struct Expr *expr_literal(double value, TokenType type);
struct Expr *expr_variable(char *name, LunarisType type);
struct Expr *expr_binary(TokenType op, struct Expr *left, struct Expr *right);
struct Expr *expr_unary(TokenType op, struct Expr *operand);
struct Expr *expr_call(char *name, LunarisType return_type, struct Expr **args, int arg_count);
struct Expr *expr_grouping(struct Expr *expression);
struct Expr *expr_index(struct Expr *array, struct Expr *index);
struct Expr *expr_member_access(struct Expr *object, char *member);
struct Expr *expr_ternary(struct Expr *condition, struct Expr *then_branch, struct Expr *else_branch);
struct Expr *expr_string(const char *value);
struct Expr *expr_boolean(bool value);


#endif