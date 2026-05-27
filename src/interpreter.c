// code where Lunaris evaluates the AST and executes the program. 
// This is where the core interpretation logic will go.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "environment.h"

// Forward declaration of the expression evaluation function.
void *evaluate(Expr *expr, Environment *env, struct Stmt *stmt);
void *execute(Stmt *stmt, Environment *env);
extern int hadRuntimeError; // Flag to indicate if a runtime error has occurred

// =========================
// Eval literal expressions/helper functions
// =========================

// Evaluate a literal expression and return its value as a void pointer (which can be cast to the appropriate type by the caller).
void *eval_literal(struct Expr *expr, Environment *env, struct Stmt *stmt) {
    Value *result = malloc(sizeof(Value));
    result->type = VAL_NUMBER;
    result->as.number = expr->literal.value;
    return result;
}

// Evaluate a unary expression by evaluating its operand and applying the unary operator to it.

// =========================
// Eval unary expressions/helper functions
// =========================

// Helper function to check if an operand is a number and return its value. This is used for unary minus and binary arithmetic operators.
// Checks if both operands are valid (for numbers or strings). Reports runtime error if not.
bool checkNumberOperand(Token operator, void *left_value, void *right_value) {
    if (left_value == NULL || right_value == NULL) {
        runtime_error(operator, "Operand is not a number.", NULL);
        hadRuntimeError = 1;
        return false;
    }
    Value *left = (Value *)left_value;
    Value *right = (Value *)right_value;
    if (left->type != VAL_NUMBER || right->type != VAL_NUMBER) {
        runtime_error(operator, "Operand is not a number.", NULL);
        hadRuntimeError = 1;
        return false;
    }
    return true;
}

// It first evaluates the operand of the unary expression and then applies the appropriate unary operator to the result.
bool is_truthy(Value *value) {
    if (value == NULL) return false;
    switch (value->type) {
        case VAL_NUMBER:
            return value->as.number != 0;
        case VAL_BOOLEAN:
            return value->as.boolean;
        case VAL_NULL:
            return false;
        default:
            return false;
    }
}

Value *eval_unary_minus(Value *operand_value) {
    Value *result = malloc(sizeof(Value));
    result->type = VAL_NUMBER;
    checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, operand_value, operand_value);
    result->as.number = -((Value *)operand_value)->as.number;
    return result;
}

// Evaluate a logical NOT expression by evaluating its operand and applying the logical NOT operator to it.
Value *eval_logical_not(Value *operand_value) {
    Value *result = malloc(sizeof(Value));
    result->type = VAL_BOOLEAN;
    result->as.boolean = !is_truthy(operand_value);
    return result;
}

// =========================
// unary expression evaluation
// =========================

// we evaluate a unary expression by evaluating its operand and applying the appropriate unary operator to it. 
// The result is returned as a void pointer, which can be cast to the appropriate type by the caller.
void *eval_unary(struct Expr *expr, Environment *env, struct Stmt *stmt) {
    void *operand_value = evaluate(expr->unary.operand, env, stmt);
    switch (expr->unary.op) {

        case TOKEN_MINUS:
            return eval_unary_minus((Value *)operand_value);
        case TOKEN_NOT:
            return eval_logical_not((Value *)operand_value);
        default:
            runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Unknown unary operator.", NULL);
            return NULL;
    }
}

// =========================
// Eval binaryhelper functions
// =========================

// Helper function to check if two values are equal, handling both numbers and booleans. This is used for equality comparisons in binary expressions.
bool is_equal(Value *a, Value *b) {
    if (a == NULL && b == NULL) return true;
    if (a == NULL || b == NULL) return false;
    if (a->type != b->type) return false;
    switch (a->type) {
        case VAL_NUMBER:
            return a->as.number == b->as.number;
        case VAL_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        case VAL_NULL:
            return true;
        default:
            return false;
    }
}

// ========================
// Eval binary expressions
// ========================

// Evaluate a binary expression by evaluating its left and right sub-expressions 
// and applying the appropriate binary operator to the results.
void *eval_binary(struct Expr *expr, Environment *env, struct Stmt *stmt) {
    Expr *left = expr->binary.left;
    Expr *right = expr->binary.right;
    Value *left_value = (Value *)evaluate(left, env, stmt);
    Value *right_value = (Value *)evaluate(right, env, stmt);
    
    // we handle the evaluation of binary expressions, such as addition, subtraction, multiplication, division, and equality/inequality comparisons. 
    // The function first evaluates the left and right sub-expressions of the binary expression 
    // and then applies the appropriate binary operator to the results. The final result is returned as a void pointer, which can be cast to the appropriate type by the caller.
    switch (expr->binary.op) {
        case TOKEN_PLUS: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_NUMBER;
            result->as.number = left_value->as.number + right_value->as.number;
            return result;
        }
        case TOKEN_MINUS: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_NUMBER;
            result->as.number = left_value->as.number - right_value->as.number;
            return result;
        }
        case TOKEN_STAR: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_NUMBER;
            result->as.number = left_value->as.number * right_value->as.number;

            
            return result;
        }
        case TOKEN_SLASH: {
            if (right_value->as.number == 0) {
                runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Division by zero.", NULL);
                return NULL;
            }
            Value *result = malloc(sizeof(Value));
            result->type = VAL_NUMBER;
            result->as.number = left_value->as.number / right_value->as.number;
            return result;
        }
        case TOKEN_EQUAL_EQUAL: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_BOOLEAN;
            result->as.boolean = is_equal(left_value, right_value);
            return result;
        }
        case TOKEN_NOT_EQUAL: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_BOOLEAN;
            result->as.boolean = !is_equal(left_value, right_value);
            return result;
        }
        case TOKEN_LESS: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = VAL_BOOLEAN;
            result->as.boolean = left_value->as.number < right_value->as.number;
            return result;
        }
        case TOKEN_LESS_EQUAL: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = VAL_BOOLEAN;
            result->as.boolean = left_value->as.number <= right_value->as.number;
            return result;
        }
        case TOKEN_GREATER: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = VAL_BOOLEAN;
            result->as.boolean = left_value->as.number > right_value->as.number;
            return result;
        }
        case TOKEN_GREATER_EQUAL: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = VAL_BOOLEAN;
            result->as.boolean = left_value->as.number >= right_value->as.number;
            return result;
        }
        default:
            runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Unknown binary operator.", NULL);
            return NULL;
    }
}

// =========================
// Eval grouping expressions
// =========================

void *eval_grouping(struct Expr *expr, Environment *env, struct Stmt *stmt) {
    return evaluate(expr->grouping.expression, env, stmt);
}

// Helper function to convert a Value to a string representation for printing or debugging purposes.


char *stringifyValue(Value *value) {
    if (!value) return STRDUP("<null>");
    switch (value->type) {
        case VAL_NUMBER: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%g", value->as.number);
            return STRDUP(buffer);
        }
        case VAL_BOOLEAN:
            return STRDUP(value->as.boolean ? "true" : "false");
        case VAL_STRING:
            return STRDUP(value->as.string ? value->as.string : "");
        case VAL_NULL:
            return STRDUP("null");
        default:
            return STRDUP("<unknown>");
    }
}

// =========================
// Environment and variable handling functions will go here
// ========================

// Initialize a new environment for variable storage and scope management. This will be used to store variable bindings and manage scopes during interpretation.
Environment *create_environment() {
    Environment *env = malloc(sizeof(Environment));
    env->entries = NULL;
    env->count = 0;
    env->capacity = 0;
    env->parent = NULL; // For global environment, parent is NULL
    return env;
}

// Set a variable in the environment with the given name and value. This will be used to create new variable bindings or update existing ones during interpretation.
void *env_set_variable(Environment *env, const char *name, Value value) {
    // Check if the variable already exists in the current environment
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            env->entries[i].value = value; // Update existing variable
            return NULL;
        }
    }
    // If not found, add a new entry
    if (env->count >= env->capacity) {
        int newCapacity = env->capacity == 0 ? 8 : env->capacity * 2;
        env->entries = realloc(env->entries, newCapacity * sizeof(Entry));
        env->capacity = newCapacity;
    }
    env->entries[env->count].name = STRDUP(name);
    env->entries[env->count].value = value;
    env->count++;
    return NULL;
}

// Get the value of a variable from the environment by its name. This will be used to retrieve variable values during interpretation.
Value *env_get_variable(Environment *env, const char *name) {
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            return &env->entries[i].value; // Return pointer to the variable's value
        }
    }
    if (env->parent != NULL) {
        return env_get_variable(env->parent, name); // Look in parent environment
    }
    return NULL; // Variable not found
}

// Free the memory used by the environment and its entries. This should be called when the environment is no longer needed to avoid memory leaks.
void free_environment(Environment *env) {
    for (int i = 0; i < env->count; i++) {
        free(env->entries[i].name); // Free variable names
        if (env->entries[i].value.type == VAL_STRING) {
            free((char *)env->entries[i].value.as.string); // Free string values
        }
    }
    free(env->entries); // Free entries array
    free(env); // Free the environment itself
}

// Evaluate a variable expression by looking up its value in the environment. This will be used to retrieve the value of variables during interpretation.
Value *eval_variable(struct Expr *expr, Environment *env, struct Stmt *stmt) {
    Value *value = env_get_variable(env, expr->variable.name);
    if (value == NULL) {
        runtime_error(expr->variable.name_token, "Undefined variable '%s'.", expr->variable.name);
        return NULL;
    }
    return value;
}

// STMT for variable declaration will be handled in the execute function when we execute a variable declaration statement. We will evaluate the initializer expression and then set the variable in the environment with its initial value.

Value *eval_variable_declaration(struct Stmt *stmt, Environment *env) {
    Value *init_value = (Value *)evaluate(stmt->variable_declaration_stmt.variable->value, env, stmt);
    if (!init_value) {
        runtime_error(stmt->variable_declaration_stmt.variable->name_token, "Failed to initialize variable '%s'.", stmt->variable_declaration_stmt.variable->name);
        return NULL;
    }

    TokenType var_type = stmt->variable_declaration_stmt.variable->typeannotation_types;
    if (var_type != TOKEN_TYPE_NONE) {
        bool type_ok = false;
        switch (var_type) {
            case TOKEN_TYPE_BOOL:
                type_ok = init_value->type == VAL_BOOLEAN;
                break;
            case TOKEN_TYPE_INT:
            case TOKEN_TYPE_FLOAT:
                type_ok = init_value->type == VAL_NUMBER;
                break;
            case TOKEN_TYPE_CHAR:
            case TOKEN_TYPE_STRING:
                type_ok = init_value->type == VAL_STRING;
                break;
            case TOKEN_TYPE_VOID:
                type_ok = init_value->type == VAL_NULL;
                break;
            default:
                break;
        }
        if (!type_ok) {
            char msg[256];
            snprintf(msg, sizeof(msg), "Type mismatch for variable '%s'. Expected type does not match initializer type.", stmt->variable_declaration_stmt.variable->name);
            runtime_error(stmt->variable_declaration_stmt.variable->name_token, msg, NULL);
            return NULL;
        }
    }
    env_set_variable(env, stmt->variable_declaration_stmt.variable->name, *init_value);
    free(init_value); // Free the temporary value after setting it in the environment
    return NULL;
}

// =========================
// Main evaluation function
// =========================

// Main function to evaluate an expression and return its result. This is where the core interpretation logic will go.
void *evaluate(Expr *expr, Environment *env, struct Stmt *stmt) {
    // check the type of the expression and evaluate it accordingly
    switch (expr->type) {
        case EXPR_LITERAL:
            return eval_literal(expr, env, stmt);
        case EXPR_UNARY:
            return eval_unary(expr, env, stmt);
        case EXPR_BINARY:
            return eval_binary(expr, env, stmt);
        case EXPR_GROUPING:
            return eval_grouping(expr, env, stmt);
        case EXPR_VARIABLE:
            return eval_variable(expr, env, stmt);
        case EXPR_STRING: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_STRING;
            result->as.string = STRDUP(expr->string.value);
            return result;
        }
        case EXPR_BOOLEAN: {
            Value *result = malloc(sizeof(Value));
            result->type = VAL_BOOLEAN;
            result->as.boolean = expr->boolean.value;
            return result;
        }
        // TODO: Implement evaluation for other expression types (variables, binary, unary, etc.)
        default:
            runtime_error(stmt->variable_declaration_stmt.variable->name_token, "Evaluation not implemented for this expression type.", NULL);
            return NULL;
    }
}

// Main function to execute a statement. This will be used to execute statements during interpretation, such as variable declarations, expression statements, etc.
void *execute(Stmt *stmt, Environment *env) {
    // check the type of the statement and execute it accordingly
    switch (stmt->type) {
        case STMT_EXPR:
            return evaluate(stmt->expr_stmt.expr, env, stmt);
        case STMT_VARIABLE_DECLARATION: {
            return eval_variable_declaration(stmt, env);
        }
        default:
            runtime_error(stmt->variable_declaration_stmt.variable->name_token, "Execution not implemented for this statement type.", NULL);
            return NULL;
    }
}

// Main function to interpret an expression and print its result. This is the entry point for the interpreter.
void interpreter(struct Stmt *stmt, Environment *env) {
    void *result = execute(stmt, env);
    if (hadRuntimeError) {
        return;
    }
    if (result) {
        char *result_str = stringifyValue((Value *)result);
        printf("%s\n", result_str);
        free(result_str);
    }
}