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
    };
};

#endif