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

// Create a new literal expression with the given value and type.
struct Expr *expr_literal(double value, TokenType type) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_LITERAL;
    e->literal.value    = value;
    e->literal.type     = type;
    return e;
}

struct Expr *expr_string(const char *value) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_STRING;
    e->string.value     = value;
    return e;
}

// Create a new variable expression with the given name and type.
struct Expr *expr_variable(char *name, LunarisType type, Token name_token) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_VARIABLE;
    e->variable.name    = name;
    e->variable.type    = type;
    e->variable.name_token = name_token;
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
struct Expr *expr_call(char *name, LunarisType return_type, struct Expr **args, int arg_count) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_CALL;
    e->call.name        = name;
    e->call.return_type = return_type;
    e->call.args        = args;
    e->call.arg_count   = arg_count;
    e->call.name_token  = (Token){.type = TOKEN_IDENTIFIER, .line = 0, .literal = name}; // add this
    return e;
}

// Create a new grouping expression with the given sub-expression.
struct Expr *expr_grouping(struct Expr *expression) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_GROUPING;
    e->grouping.expression = expression;
    return e;
}

struct Expr *expr_assign(char *name, Token name_token, struct Expr *value) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_ASSIGN;
    e->assign.name      = name;
    e->assign.name_token = name_token;
    e->assign.value     = value;
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

// Create a new boolean expression with the given value.
struct Expr *expr_boolean(bool value) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_BOOLEAN;
    e->boolean.value     = value;
    return e;
}

// Create a new logical expression (&&, ||) with the given operator and left/right sub-expressions.
// Kept separate from expr_binary so the interpreter can short-circuit instead of eagerly evaluating both sides.
struct Expr *expr_logical(TokenType op, struct Expr *left, struct Expr *right) {
    struct Expr *e      = malloc(sizeof(struct Expr));
    e->type             = EXPR_LOGICAL;
    e->logical.op       = op;
    e->logical.left     = left;
    e->logical.right    = right;
    return e;
}

// Create a new array literal expression, e.g. [1, 2, 3]. Takes ownership of
// the elements array (caller should not free it separately).
struct Expr *expr_array_literal(struct Expr **elements, int count) {
    struct Expr *e            = malloc(sizeof(struct Expr));
    e->type                   = EXPR_ARRAY_LITERAL;
    e->array_literal.elements = elements;
    e->array_literal.count    = count;
    return e;
}

// Create a new index-assignment expression, e.g. arr[0] = value.
struct Expr *expr_index_assign(struct Expr *array, struct Expr *index, struct Expr *value) {
    struct Expr *e         = malloc(sizeof(struct Expr));
    e->type                = EXPR_INDEX_ASSIGN;
    e->index_assign.array  = array;
    e->index_assign.index  = index;
    e->index_assign.value  = value;
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
        case EXPR_STRING:
            free((void *)e->string.value);
            break;
         case EXPR_BOOLEAN:
            break;
        case EXPR_CALL:
            free(e->call.name);
            for (int i = 0; i < e->call.arg_count; i++) {
                expr_free(e->call.args[i]);
            }
            free(e->call.args);
            break;
        case EXPR_GROUPING:
            expr_free(e->grouping.expression);
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
        case EXPR_LOGICAL:
            expr_free(e->logical.left);
            expr_free(e->logical.right);
            break;
        case EXPR_ARRAY_LITERAL:
            for (int i = 0; i < e->array_literal.count; i++) {
                expr_free(e->array_literal.elements[i]);
            }
            free(e->array_literal.elements);
            break;
        case EXPR_INDEX_ASSIGN:
            expr_free(e->index_assign.array);
            expr_free(e->index_assign.index);
            expr_free(e->index_assign.value);
            break;
        default:
            break;
    }
    free(e);
}

