#include "stmt.h"
#include "expr.h"
#include <stdlib.h>

// Create a new statement node with the given type and associated data.
struct Stmt *stmt_new(StmtType type) {
    struct Stmt *s  = malloc(sizeof(struct Stmt));
    s->type         = type;
    return s;
}

// Create a new expression statement with the given expression.
struct Stmt *stmt_expr(Expr *expr) {
    struct Stmt *s      = stmt_new(STMT_EXPR);
    s->expr_stmt.expr   = expr;
    return s;
}

// Create a new assignment statement with the given variable name and value expression.
struct Stmt *stmt_assign(char *name, Expr *value) {
    struct Stmt *s = stmt_new(STMT_ASSIGN);
    s->assign_stmt.name     = name;
    s->assign_stmt.value    = value;
    return s;
}

// Create a new if statement with the given condition, then-branch, and else-branch.
struct Stmt *stmt_if(Expr *condition, struct Stmt **then_branch, int
then_count, struct Stmt **else_branch, int else_count) {
    struct Stmt *s = stmt_new(STMT_IF);
    s->if_stmt.condition    = condition;
    s->if_stmt.then_branch  = then_branch;
    s->if_stmt.then_count   = then_count;
    s->if_stmt.else_branch  = else_branch;
    s->if_stmt.else_count   = else_count;
    return s;
}

// Create a new else statement with the given list of statements.
struct Stmt *stmt_else(struct Stmt **statements, int statement_count) {
    struct Stmt *s = stmt_new(STMT_ELSE);
    s->else_stmt.statements = statements;
    s->else_stmt.statement_count = statement_count;
    return s;
}

struct Stmt *stmt_elif(Expr *condition, struct Stmt **body, int body_count) {
    struct Stmt *s = stmt_new(STMT_ELIF);
    s->elif_stmt.condition = condition;
    s->elif_stmt.body = body;
    s->elif_stmt.body_count = body_count;
    return s;
}

// Create a new while statement with the given condition and body.
struct Stmt *stmt_while(Expr *condition, struct Stmt **body, int body_count) {
    struct Stmt *s = stmt_new(STMT_WHILE);
    s->while_stmt.condition     = condition;
    s->while_stmt.body          = body;
    s->while_stmt.body_count    = body_count;
    return s;
}

// Create a new return statement with the given value expression.
struct Stmt *stmt_return(Expr *value) {
    struct Stmt *s = stmt_new(STMT_RETURN);
    s->return_stmt.value    = value;
    return s;
}

// Create a new type notation statement with the given type name and value expression.
struct Stmt *stmt_type_notation(char *type_name, Expr *value) {
    struct Stmt *s = stmt_new(STMT_TYPE_NOTATION);
    s->type_notation_stmt.type_name     = type_name;
    s->type_notation_stmt.value         = value;
    return s;
}

// Create a new function declaration statement with the given name, parameters, return type, and body.
struct Stmt *stmt_function_declaration(char *name, char **param_names, char **
param_types, int param_count, char *return_type, struct Stmt **body, int body_count) {
    struct Stmt *s = stmt_new(STMT_FUNCTION_DECLARATION);
    s->function_declaration_stmt.name          = name;
    s->function_declaration_stmt.param_names   = param_names;
    s->function_declaration_stmt.param_types   = param_types;
    s->function_declaration_stmt.param_count   = param_count;
    s->function_declaration_stmt.return_type   = return_type;
    s->function_declaration_stmt.body          = body;
    s->function_declaration_stmt.body_count    = body_count;
    return s;
}

// Create a new for statement with the given iterator name, iterable expression, and body.
struct Stmt *stmt_for(char *iterator_name, Expr *iterable, struct Stmt **body, int body_count) {
    struct Stmt *s = stmt_new(STMT_FOR);
    s->for_stmt.iterator_name = iterator_name;
    s->for_stmt.iterable = iterable;
    s->for_stmt.body = body;
    s->for_stmt.body_count = body_count;
    return s;
}

// Create a new block statement with the given list of statements.
struct Stmt *stmt_block(struct Stmt **statements, int statement_count) {
    struct Stmt *s = stmt_new(STMT_BLOCK);
    s->block_stmt.statements = statements;
    s->block_stmt.statement_count = statement_count;
    return s;
}

// Create a new switch statement with the given expression, cases, and default case.
struct Stmt *stmt_switch(Expr *expression, struct Stmt **cases, int case_count, struct Stmt *default_case) {
    struct Stmt *s = stmt_new(STMT_SWITCH);
    s->switch_stmt.expression = expression;
    s->switch_stmt.cases = cases;
    s->switch_stmt.case_count = case_count;
    s->switch_stmt.default_case = default_case;
    return s;
}

// Create a new case statement with the given value expression and body.
struct Stmt *stmt_case(Expr *value, struct Stmt **body, int body_count) {
    struct Stmt *s = stmt_new(STMT_CASE);
    s->case_stmt.value = value;
    s->case_stmt.body = body;
    s->case_stmt.body_count = body_count;
    return s;
}

// Create a new break statement.
struct Stmt *stmt_break() {
    return stmt_new(STMT_BREAK);
}

// Create a new continue statement.
struct Stmt *stmt_continue() {
    return stmt_new(STMT_CONTINUE);
}

// Create a new default case statement with the given list of cases.
struct Stmt *stmt_default(struct Stmt **cases, int case_count) {
    struct Stmt *s = stmt_new(STMT_DEFAULT);
    s->default_stmt.cases = cases;
    s->default_stmt.case_count = case_count;
    return s;
}

struct Stmt *stmt_import(char *module_name) {
    struct Stmt *s = stmt_new(STMT_IMPORT);
    s->import_stmt.module_name = module_name;
    return s;
}

// Free the memory used by a statement and its associated data.
void stmt_free(struct Stmt *stmt) {
    if (!stmt) return;
    switch (stmt->type) {
        case STMT_EXPR:
            expr_free(stmt->expr_stmt.expr);
            break;
        case STMT_ASSIGN:
            free(stmt->assign_stmt.name);
            expr_free(stmt->assign_stmt.value);
            break;
        case STMT_IF:
            expr_free(stmt->if_stmt.condition);
            for (int i = 0; i < stmt->if_stmt.then_count; i++) {
                stmt_free(stmt->if_stmt.then_branch[i]);
            }
            free(stmt->if_stmt.then_branch);
            for (int i = 0; i < stmt->if_stmt.else_count; i++) {
                stmt_free(stmt->if_stmt.else_branch[i]);
            }
            free(stmt->if_stmt.else_branch);
            break;
        case STMT_ELSE:
            for (int i = 0; i < stmt->else_stmt.statement_count; i++) {
                stmt_free(stmt->else_stmt.statements[i]);
            }
            free(stmt->else_stmt.statements);
            break;
        case STMT_ELIF:
            expr_free(stmt->elif_stmt.condition);
            for (int i = 0; i < stmt->elif_stmt.body_count; i++) {
                stmt_free(stmt->elif_stmt.body[i]);
            }
            free(stmt->elif_stmt.body);
            break;
        case STMT_WHILE:
            expr_free(stmt->while_stmt.condition);
            for (int i = 0; i < stmt->while_stmt.body_count; i++) {
                stmt_free(stmt->while_stmt.body[i]);
            }
            free(stmt->while_stmt.body);
            break;
        case STMT_RETURN:
            expr_free(stmt->return_stmt.value);
            break;
        case STMT_TYPE_NOTATION:
            free(stmt->type_notation_stmt.type_name);
            expr_free(stmt->type_notation_stmt.value);
            break;
        case STMT_FUNCTION_DECLARATION:
            free(stmt->function_declaration_stmt.name);
            for (int i = 0; i < stmt->function_declaration_stmt.param_count; i++) {
                free(stmt->function_declaration_stmt.param_names[i]);
                free(stmt->function_declaration_stmt.param_types[i]);
            }
            free(stmt->function_declaration_stmt.param_names);
            free(stmt->function_declaration_stmt.param_types);
            free(stmt->function_declaration_stmt.return_type);
            for (int i = 0; i < stmt->function_declaration_stmt.body_count; i++) {
                stmt_free(stmt->function_declaration_stmt.body[i]);
            }
            free(stmt->function_declaration_stmt.body);
            break;
        case STMT_FOR:
            free(stmt->for_stmt.iterator_name);
            expr_free(stmt->for_stmt.iterable);
            for (int i = 0; i < stmt->for_stmt.body_count; i++) {
                stmt_free(stmt->for_stmt.body[i]);
            }
            free(stmt->for_stmt.body);
            break;
        case STMT_BLOCK:
            for (int i = 0; i < stmt->block_stmt.statement_count; i++) {
                stmt_free(stmt->block_stmt.statements[i]);
            }
            free(stmt->block_stmt.statements);
            break;
        case STMT_SWITCH:
            expr_free(stmt->switch_stmt.expression);
            for (int i = 0; i < stmt->switch_stmt.case_count; i++) {
                stmt_free(stmt->switch_stmt.cases[i]);
            }
            free(stmt->switch_stmt.cases);
            stmt_free(stmt->switch_stmt.default_case);
            break;
        case STMT_CASE:
            expr_free(stmt->case_stmt.value);
            for (int i = 0; i < stmt->case_stmt.body_count; i++) {
                stmt_free(stmt->case_stmt.body[i]);
            }
            free(stmt->case_stmt.body);
            break;
        case STMT_BREAK:
        case STMT_CONTINUE: 
        case STMT_DEFAULT:
            for (int i = 0; i < stmt->default_stmt.case_count; i++) {
                stmt_free(stmt->default_stmt.cases[i]);
            }
            free(stmt->default_stmt.cases);
            break;
        case STMT_IMPORT:
            free(stmt->import_stmt.module_name);
            break;
    }
    free(stmt);
}
