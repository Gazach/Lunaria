// code where Lunaris evaluates the AST and executes the program. 
// This is where the core interpretation logic will go.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "runtime.h"

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
    if (expr->literal.type == TOKEN_INT) {
        result->type = TYPE_INT;
        result->as.int_value = (int)expr->literal.value;
    } else if (expr->literal.type == TOKEN_FLOAT) {
        result->type = TYPE_FLOAT;
        result->as.float_value = expr->literal.value;
    }
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
    if ((left->type != TYPE_INT && left->type != TYPE_FLOAT) || (right->type != TYPE_INT && right->type != TYPE_FLOAT)) {
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
        case TYPE_INT:
            return value->as.int_value != 0;
        case TYPE_FLOAT:
            return value->as.float_value != 0.0;
        case TYPE_BOOLEAN:
            return value->as.boolean;
        case TYPE_NULL:
            return false;
        default:
            return false;
    }
}

Value *eval_unary_minus(Value *operand_value) {
    Value *result = malloc(sizeof(Value));
    if (operand_value->type == TYPE_INT) {
        result->type = TYPE_INT;
        result->as.int_value = -((Value *)operand_value)->as.int_value;
    } else if (operand_value->type == TYPE_FLOAT) {
        result->type = TYPE_FLOAT;
        result->as.float_value = -((Value *)operand_value)->as.float_value;
    }
    return result;
}

// Evaluate a logical NOT expression by evaluating its operand and applying the logical NOT operator to it.
Value *eval_logical_not(Value *operand_value) {
    Value *result = malloc(sizeof(Value));
    result->type = TYPE_BOOLEAN;
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
        case TYPE_INT:
            return a->as.int_value == b->as.int_value;
        case TYPE_FLOAT:
            return a->as.float_value == b->as.float_value;
        case TYPE_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        case TYPE_NULL:
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
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->type = TYPE_INT;
                result->as.int_value = left_value->as.int_value + right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value + right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = (float)left_value->as.int_value + right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value + (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_MINUS: {
            Value *result = malloc(sizeof(Value));
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->type = TYPE_INT;
                result->as.int_value = left_value->as.int_value - right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value - right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = (float)left_value->as.int_value - right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value - (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_STAR: {
            Value *result = malloc(sizeof(Value));
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->type = TYPE_INT;
                result->as.int_value = left_value->as.int_value * right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value * right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = (float)left_value->as.int_value * right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value * (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_SLASH: {
            // Division by zero check for both int and float
            if ((right_value->type == TYPE_INT && right_value->as.int_value == 0) ||
                (right_value->type == TYPE_FLOAT && right_value->as.float_value == 0.0f)) {
                runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Division by zero.", NULL);
                return NULL;
            }
            Value *result = malloc(sizeof(Value));
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = (float)left_value->as.int_value / (float)right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value / right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = (float)left_value->as.int_value / right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->type = TYPE_FLOAT;
                result->as.float_value = left_value->as.float_value / (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_EQUAL_EQUAL: {
            Value *result = malloc(sizeof(Value));
            result->type = TYPE_BOOLEAN;
            result->as.boolean = is_equal(left_value, right_value);
            return result;
        }
        case TOKEN_NOT_EQUAL: {
            Value *result = malloc(sizeof(Value));
            result->type = TYPE_BOOLEAN;
            result->as.boolean = !is_equal(left_value, right_value);
            return result;
        }
        case TOKEN_LESS: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = TYPE_BOOLEAN;
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.int_value < right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = left_value->as.float_value < right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = (float)left_value->as.int_value < right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.float_value < (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_LESS_EQUAL: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = TYPE_BOOLEAN;
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.int_value <= right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = left_value->as.float_value <= right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = (float)left_value->as.int_value <= right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.float_value <= (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_GREATER: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = TYPE_BOOLEAN;
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.int_value > right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = left_value->as.float_value > right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = (float)left_value->as.int_value > right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.float_value > (float)right_value->as.int_value;
            }
            return result;
        }
        case TOKEN_GREATER_EQUAL: {
            Value *result = malloc(sizeof(Value));
            checkNumberOperand((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, left_value, right_value);
            result->type = TYPE_BOOLEAN;
            if (left_value->type == TYPE_INT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.int_value >= right_value->as.int_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = left_value->as.float_value >= right_value->as.float_value;
            } else if (left_value->type == TYPE_INT && right_value->type == TYPE_FLOAT) {
                result->as.boolean = (float)left_value->as.int_value >= right_value->as.float_value;
            } else if (left_value->type == TYPE_FLOAT && right_value->type == TYPE_INT) {
                result->as.boolean = left_value->as.float_value >= (float)right_value->as.int_value;
            }
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
        case TYPE_INT: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%d", value->as.int_value);
            return STRDUP(buffer);
        }
        case TYPE_FLOAT: {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "%g", value->as.float_value);
            return STRDUP(buffer);
        }
        case TYPE_BOOLEAN:
            return STRDUP(value->as.boolean ? "true" : "false");
        case TYPE_STRING:
            return STRDUP(value->as.string ? value->as.string : "");
        case TYPE_NULL:
            return STRDUP("null");
        default:
            return STRDUP("<unknown>");
    }
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
    const char *name = stmt->variable_declaration_stmt.variable->name;

    // check if variable already declared in THIS scope (not parent)
    for (int i = 0; i < env->count; i++) {
        if (strcmp(env->entries[i].name, name) == 0) {
            char msg[256];
            snprintf(msg, sizeof(msg), "Variable '%s' is already declared in this scope.", name);
            runtime_error(stmt->variable_declaration_stmt.variable->name_token, msg, NULL);
            return NULL;
        }
    }
    
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
                type_ok = init_value->type == TYPE_BOOLEAN;
                break;
            case TOKEN_TYPE_INT:
                type_ok = init_value->type == TYPE_INT;
                break;
            case TOKEN_TYPE_FLOAT:
                type_ok = init_value->type == TYPE_FLOAT || init_value->type == TYPE_INT; // Allow implicit int to float conversion
                break;
            case TOKEN_TYPE_CHAR:
                type_ok = init_value->type == TYPE_STRING && strlen(init_value->as.string) == 1; // A char is a string of length 1
                break;
            case TOKEN_TYPE_STRING:
                type_ok = init_value->type == TYPE_STRING;
                break;
            case TOKEN_TYPE_VOID:
                type_ok = init_value->type == TYPE_NULL;
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
    env_set_variable(env, stmt->variable_declaration_stmt.variable->name, stmt->variable_declaration_stmt.variable->is_mutable, *init_value);
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
            result->type = TYPE_STRING;
            result->as.string = STRDUP(expr->string.value);
            return result;
        }
        case EXPR_BOOLEAN: {
            Value *result = malloc(sizeof(Value));
            result->type = TYPE_BOOLEAN;
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
        case STMT_BLOCK: {
            Environment *block_env = env_create_child(env);
            for (int i = 0; i < stmt->block_stmt.statement_count; i++) {
                execute(stmt->block_stmt.statements[i], block_env);
                if (hadRuntimeError) {
                    free_environment(block_env);
                    return NULL;
                }
            }
            free_environment(block_env);
            return NULL;
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