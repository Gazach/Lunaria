#ifndef STMT_H
#define STMT_H
#include "expr.h"

typedef struct Stmt Stmt;
typedef enum { //define statement types
    STMT_EXPR,
    STMT_ASSIGN,
    STMT_IF,
    STMT_WHILE,
    STMT_RETURN,
    STMT_TYPE_NOTATION, // New statement type for type annotations like "let x: int = 5;"
    STMT_FUNCTION_DECLARATION // New statement type for function declarations like "fn add(a: int, b: int) -> int { return a + b; }"

} StmtType;

struct Stmt { // define the structure of a statement
    StmtType type;
    union {
        struct {
            Expr *expr;
        } expr_stmt;
        struct {
            char *name;
            Expr *value;
        } assign_stmt;
        struct {
            Expr *condition;
            struct Stmt **then_branch;
            int then_count;
            struct Stmt **else_branch;
            int else_count;
        } if_stmt;
        struct {
            Expr *condition;
            struct Stmt **body;
            int body_count;
        } while_stmt;
        struct {
            Expr *value;
        } return_stmt;
        struct {
            char *type_name;
            Expr *value;
        } type_notation_stmt;
        struct {
            char *name;
            char **param_names;
            char **param_types; // New field for parameter types
            int param_count;
            char *return_type; // New field for return type
            struct Stmt **body;
            int body_count;
        } function_declaration_stmt;
    };
};

#endif
