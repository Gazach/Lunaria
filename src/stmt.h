#ifndef STMT_H
#define STMT_H
#include "expr.h"

typedef struct Stmt Stmt;
typedef enum { //define statement types
    STMT_EXPR,
    STMT_ASSIGN, // New statement type for variable assignment like "x = 5;"
    STMT_IF,
    STMT_ELSE, // New statement type for else branches of if statements
    STMT_ELIF, // New statement type for else-if branches of if statements
    STMT_WHILE,
    STMT_RETURN,
    STMT_TYPE_NOTATION, // New statement type for type annotations like "let x: int = 5;"
    STMT_FUNCTION_DECLARATION, // New statement type for function declarations like "fn add(a: int, b: int) -> int { return a + b; }"
    STMT_FOR, // New statement type for for loops like "for i in 0..10 { print(i); }"
    STMT_BLOCK, // New statement type for blocks of statements like "{ stmt1; stmt2; }"
    STMT_SWITCH, // New statement type for switch statements like "switch x { case 1: stmt1; break; case 2: stmt2; break; default: stmt3; }"
    STMT_CASE, // New statement type for case branches of switch statements
    STMT_BREAK, // New statement type for break statements like "break;"
    STMT_CONTINUE, // New statement type for continue statements like "continue;"
    STMT_DEFAULT // New statement type for default branches of switch statements
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
            struct Stmt **statements;
            int statement_count;
        } else_stmt;
        struct {
            Expr *condition;
            struct Stmt **body;
            int body_count;
        } elif_stmt;
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
        struct {
            char *iterator_name;
            Expr *iterable;
            struct Stmt **body;
            int body_count;
        } for_stmt;
        struct {
            struct Stmt **statements;
            int statement_count;
        } block_stmt;
        struct {
            Expr *expression;
            struct Stmt **cases;
            int case_count;
            struct Stmt *default_case;
        } switch_stmt;
        struct {
            Expr *value;
            struct Stmt **body;
            int body_count;
        } case_stmt;
        struct {
        } break_stmt;
        struct {
        } continue_stmt;
        struct {
            struct Stmt **cases;
            int case_count;
        } default_stmt;  
    };
};

#endif
