#include <stdlib.h>
#include <stdbool.h>

#include "token.h"
#include "expr.h"
#include "stmt.h"
#include "errorHandling.h"
#include "tokenType.h"

// Define the structure of the parser, which holds the list of tokens and the current position in that list.
typedef struct {
    Token *data;
    int token_count;
    int current;
    int line;
} Parser;

//===========================
// ---- Parser helpers ----
//===========================

// Initialize a new parser with the given list of tokens and token count.
Parser *initParser(Token *tokens, int token_count) {
    Parser *parser = malloc(sizeof(Parser));
    parser->data = tokens;
    parser->token_count = token_count;
    parser->current = 0;
    parser->line = 1; // Initialize the line number to 1 for error reporting
    return parser;
}

Expr *expression(Parser *parser); // Forward declaration of the expression parsing function

// Check if we've reached the end of the token list.
Token *peek(Parser *parser) {
    if (parser->current >= parser->token_count) return NULL;
    return &parser->data[parser->current];
}

// Advance the parser to the next token and return the previous token.
Token *previous(Parser *parser) {
    if (parser->current <= 0) return NULL;
    return &parser->data[parser->current - 1];
}

// Check if the current token matches the expected type and advance if it does.
static bool isAtEnd(Parser *parser) {
    return peek(parser)->type == EOF_TOKEN;
}

// Advance the parser and return the current token.
Token *advance(Parser *parser) {
    if (!isAtEnd(parser)) parser->current++;
    return previous(parser);
}

// Check if the current token matches the expected type without advancing.
static bool check(Parser *parser, TokenType type) {
    if (isAtEnd(parser)) return false;
    return peek(parser)->type == type;
}

// Check if the current token matches any of the expected types and advance if it does.
static bool match(Parser *parser, const TokenType *types, int type_count) {
    for (int i = 0; i < type_count; i++) {
        if (check(parser, types[i])) {
            advance(parser);
            return true;
        }
    }
    return false;
}

// Consume a token of the expected type, advancing the parser. If the token does not match, report an error.
Token *consume(Parser *parser, TokenType type, const char *message) {
    if (check(parser, type)) return advance(parser);
    // Handle error: expected token of type 'type'
    error(*peek(parser), message);
    return NULL;
}

// Report a parsing error with the given message and the current line number.
static void parseError(Parser *parser, const char *message) {
    error(*peek(parser), message);
    return;
}

// Synchronize the parser after an error by advancing until we find a statement boundary.
// This helps to prevent cascading errors and allows the parser to continue parsing after an error.
void synchronize(Parser *parser) {
    advance(parser);

    while (!isAtEnd(parser)) {
        if (previous(parser)->type == SEMICOLON) return;

        switch (peek(parser)->type) {
            case CLASS:
            case FUNCTION:
            case FOR:
            case IF:
            case WHILE:
            case RETURN:
            case ELSE:
            case ELIF:
            case IMPORT:
            case PRINT:
            case SWITCH:
            case CASE:
            case DEFAULT:
                return;
            default:
                break;
        }

        advance(parser);
    }
}


// ========================
// Main Parser functions
// ========================


// Parse a primary expression, which can be a literal, variable, or parenthesized expression.
Expr *primary(Parser *parser) {
    if (match(parser, (TokenType[]){FALSE}, 1)) {
        return expr_literal(false); // or expr_literal(0) if using double for all literals
    }
    if (match(parser, (TokenType[]){TRUE}, 1)) {
        return expr_literal(true);
    }
    if (match(parser, (TokenType[]){NIL}, 1)) {
        return expr_literal(0); 
    }
    if (match(parser, (TokenType[]){NUMBER}, 1)) {
        return expr_literal(strtod(previous(parser)->literal, NULL));
    }
    if (match(parser, (TokenType[]){STRING}, 1)) {
        return expr_string(previous(parser)->literal);
    }
    if (match(parser, (TokenType[]){LEFT_PAREN}, 1)) {
        Expr *expr = expression(parser);
        if (!match(parser, (TokenType[]){RIGHT_PAREN}, 1)) {
            // Handle error: expected ')'
            parseError(parser, "Expected ')' after expression.");
            return NULL;
        }
        return expr_grouping(expr);
    }
    // Handle error: expected expression
    parseError(parser, "Expected expression.");
    return NULL;
}

// Parse a unary expression, which can be a primary expression or a unary operator followed by another unary expression.
Expr *Unary(Parser *parser){
    if (match(parser, (TokenType[]){MINUS, NOT}, 2)) {
        Token *operator = previous(parser);
        Expr *right = Unary(parser);
        return expr_unary(operator->type, right);
    }

    return primary(parser); // Return the result of primary if no unary operator is found
}

// Parse a factor expression, which can be a unary expression followed by zero or more multiplication/division operators and another unary expression.
Expr *factor(Parser *parser) {
    Expr *expr = Unary(parser);

    while (match(parser, (TokenType[]){STAR, SLASH}, 2)) {
        Token *operator = previous(parser);
        Expr *right = Unary(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

// Parse a term expression, which can be a factor expression followed by zero or more addition/subtraction operators and another factor expression.
Expr *term(Parser *parser) {
    Expr *expr = factor(parser);

    while (match(parser, (TokenType[]){PLUS, MINUS}, 2)) {
        Token *operator = previous(parser);
        Expr *right = factor(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

Expr *comparison(Parser *parser) {
    Expr *expr = term(parser);

    while (match(parser, (TokenType[]){GREATER, GREATER_EQUAL, LESS, LESS_EQUAL}, 4)) {
        Token *operator = previous(parser);
        Expr *right = term(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

// Parse an equality expression, which can be a comparison expression followed by zero or more equality operators and another comparison expression.
Expr *equality(Parser *parser) {
    Expr *expr = comparison(parser);

    while (match(parser, (TokenType[]){EQUAL_EQUAL, NOT_EQUAL}, 2)) {
        Token *operator = previous(parser);
        Expr *right = comparison(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

// Parse an expression, which is currently just an equality expression.
Expr *expression(Parser *parser) {
    return equality(parser);
}

// Parse the tokens into an expression AST. This is the main entry point for the parser.
Expr *parse(Parser *parser) {
    Expr *result = expression(parser);
    if (result == NULL) {
        synchronize(parser);
    }
    return result;
}

// Free the memory used by the parser.
void freeParser(Parser *parser) {
    free(parser);
}