// code where Lunaris evaluates the AST and executes the program. 
// This is where the core interpretation logic will go.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"

// Forward declaration of the expression evaluation function.
void *evaluate(Expr *expr);
extern int hadRuntimeError; // Flag to indicate if a runtime error has occurred

// =========================
// Eval literal expressions/helper functions
// =========================

// Evaluate a literal expression and return its value as a void pointer (which can be cast to the appropriate type by the caller).
void *eval_literal(struct Expr *expr) {
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
        runtime_error(operator, "Operand is not a number.");
        hadRuntimeError = 1;
        return false;
    }
    Value *left = (Value *)left_value;
    Value *right = (Value *)right_value;
    if (left->type != VAL_NUMBER || right->type != VAL_NUMBER) {
        runtime_error(operator, "Operand is not a number.");
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
void *eval_unary(struct Expr *expr) {
    void *operand_value = evaluate(expr->unary.operand);
    switch (expr->unary.op) {

        case TOKEN_MINUS:
            return eval_unary_minus((Value *)operand_value);
        case TOKEN_NOT:
            return eval_logical_not((Value *)operand_value);
        default:
            runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Unknown unary operator.");
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
void *eval_binary(struct Expr *expr) {
    Expr *left = expr->binary.left;
    Expr *right = expr->binary.right;
    Value *left_value = (Value *)evaluate(left);
    Value *right_value = (Value *)evaluate(right);
    
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
                runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Division by zero.");
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
            runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Unknown binary operator.");
            return NULL;
    }
}

// =========================
// Eval grouping expressions
// =========================

void *eval_grouping(struct Expr *expr) {
    return evaluate(expr->grouping.expression);
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
// Main evaluation function
// =========================

// Main function to evaluate an expression and return its result. This is where the core interpretation logic will go.
void *evaluate(Expr *expr) {
    // check the type of the expression and evaluate it accordingly
    switch (expr->type) {
        case EXPR_LITERAL:
            return eval_literal(expr);
        case EXPR_UNARY:
            return eval_unary(expr);
        case EXPR_BINARY:
            return eval_binary(expr);
        case EXPR_GROUPING:
            return eval_grouping(expr);
        // TODO: Implement evaluation for other expression types (variables, binary, unary, etc.)
        default:
            runtime_error((Token){.type = TOKEN_EOF, .line = 0, .literal = "<unknown>"}, "Evaluation not implemented for this expression type.");
            return NULL;
    }
}

// Main function to interpret an expression and print its result. This is the entry point for the interpreter.
void interpreter(struct Expr *expr) {
    void *result = evaluate(expr);
    if (!hadRuntimeError) {
        char *result_str = stringifyValue((Value *)result);
        printf("%s\n", result_str);
        free(result_str);
    } else {
        printf("Runtime error occurred during interpretation.\n");
    }
}