#include <stdlib.h>
#include <stdbool.h>
#include <string.h>


#include "../runtime/runtime.h"
#include "../common/token.h"
#include "../frontend/ast/expr.h"
#include "../frontend/ast/stmt.h"
#include "../common/errorHandling.h"
#include "../common/tokenType.h"


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
Stmt *statement(Parser *parser); // Forward declaration of the statement parsing function


//====================================================================================================================================================
// EXPRESSION PARSING PART
//====================================================================================================================================================

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
    Token *current = peek(parser);
    return current == NULL || current->type == TOKEN_EOF;
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
static void *consume(Parser *parser, TokenType type, const char *message) {
    if (check(parser, type)) return advance(parser);

    // Handle error: expected token of a specific type
    Token *current = peek(parser);
    Token eof = {.type = TOKEN_EOF, .line = current ? current->line : parser->line, .literal = "<unknown>"};
    error(eof, message);
    return NULL;
}

// Report a parsing error with the given message and the current line number.
static void parseError(Parser *parser, const char *message) {
    Token *current = peek(parser);
    if (current) {
        Token eof = {.type = TOKEN_EOF, .line = current->line, .literal = "<unknown>"};
        error(eof, message);
    } else {
        Token eof = {.type = TOKEN_EOF, .line = parser->line, .literal = "<unknown>"};
        error(eof, message);
    }
}

// Synchronize the parser after an error by advancing until we find a statement boundary.
// This helps to prevent cascading errors and allows the parser to continue parsing after an error.
void synchronize(Parser *parser) {
    if (isAtEnd(parser)) return;
    advance(parser);

    while (!isAtEnd(parser)) {
        if (previous(parser)->type == TOKEN_SEMICOLON) return;

        switch (peek(parser)->type) {
            case TOKEN_CLASS:
            case TOKEN_FUNCTION:
            case TOKEN_FOR:
            case TOKEN_IF:
            case TOKEN_WHILE:
            case TOKEN_RETURN:
            case TOKEN_ELSE:
            case TOKEN_ELIF:
            case TOKEN_IMPORT:
            case TOKEN_PRINT:
            case TOKEN_SWITCH:
            case TOKEN_CASE:
            case TOKEN_DEFAULT:
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
    if (match(parser, (TokenType[]){TOKEN_FALSE}, 1)) {
        return expr_boolean(false);
    }
    if (match(parser, (TokenType[]){TOKEN_TRUE}, 1)) {
        return expr_boolean(true);
    }
    if (match(parser, (TokenType[]){TOKEN_NIL}, 1)) {
        return expr_literal(0, TOKEN_NIL); 
    }
    if (match(parser, (TokenType[]){TOKEN_INT, TOKEN_FLOAT}, 2)) {
        TokenType numType = previous(parser)->type;
        return expr_literal(strtod(previous(parser)->literal, NULL), numType);
    }
    if (match(parser, (TokenType[]){TOKEN_STRING}, 1)) {
        return expr_string(STRDUP(previous(parser)->literal));
    }
    if (match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
        Expr *expr = expression(parser);
        if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
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
    if (match(parser, (TokenType[]){TOKEN_MINUS, TOKEN_NOT}, 2)) {
        Token *operator = previous(parser);
        Expr *right = Unary(parser);
        return expr_unary(operator->type, right);
    }

    return primary(parser); // Return the result of primary if no unary operator is found
}

// Parse a factor expression, which can be a unary expression followed by zero or more multiplication/division operators and another unary expression.
Expr *factor(Parser *parser) {
    Expr *expr = Unary(parser);

    while (match(parser, (TokenType[]){TOKEN_STAR, TOKEN_SLASH}, 2)) {
        Token *operator = previous(parser);
        Expr *right = Unary(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

// Parse a term expression, which can be a factor expression followed by zero or more addition/subtraction operators and another factor expression.
Expr *term(Parser *parser) {
    Expr *expr = factor(parser);

    while (match(parser, (TokenType[]){TOKEN_PLUS, TOKEN_MINUS}, 2)) {
        Token *operator = previous(parser);
        Expr *right = factor(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

// Parse a comparison expression, which can be a term expression followed by zero or more comparison operators and another term expression.
Expr *comparison(Parser *parser) {
    Expr *expr = term(parser);

    while (match(parser, (TokenType[]){TOKEN_GREATER, TOKEN_GREATER_EQUAL, TOKEN_LESS, TOKEN_LESS_EQUAL}, 4)) {
        Token *operator = previous(parser);
        Expr *right = term(parser);
        expr = expr_binary(operator->type, expr, right);
    }

    return expr;
}

// Parse an equality expression, which can be a comparison expression followed by zero or more equality operators and another comparison expression.
Expr *equality(Parser *parser) {
    Expr *expr = comparison(parser);

    while (match(parser, (TokenType[]){TOKEN_EQUAL_EQUAL, TOKEN_NOT_EQUAL}, 2)) {
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

//====================================================================================================================================================
// STATEMENT PARSING PART
//====================================================================================================================================================


// variable declaration statement.
Stmt *var_Declaration(Parser *parser) {
    // boolean flags for modifiers
    bool is_const = match(parser, (TokenType[]){TOKEN_CONST}, 1);
    bool is_mutable = match(parser, (TokenType[]){TOKEN_MUTABLE}, 1);
    bool is_static = match(parser, (TokenType[]){TOKEN_STATIC}, 1);
    bool is_public = match(parser, (TokenType[]){TOKEN_PUBLIC}, 1);
    bool is_private = match(parser, (TokenType[]){TOKEN_PRIVATE}, 1);

    // we check for conflicting modifiers. For example, a variable cannot be both 'const' and 'mutable', and it cannot be both 'public' and 'private'. If we find any such conflicts, we report a parsing error with an appropriate message.
    if (is_const && is_mutable) {
        parseError(parser, "Variable cannot be both 'const' and 'mutable'.");
        return NULL;
    }
    // we allow both 'static' and 'public' or 'private' together, but not 'public' and 'private' together
    if (is_public && is_private) {
        parseError(parser, "Variable cannot be both 'public' and 'private'.");
        return NULL;
    }
  
    //check if the next token is an identifier for the variable name
    if (!match(parser, (TokenType[]){TOKEN_IDENTIFIER}, 1)) {
        parseError(parser, "Expected variable name.");
        return NULL;
    }
    

    
    Token *nameToken = previous(parser);
    const char *name = nameToken->literal;

    // Create a VariableDeclaration struct to hold the variable information, including its name and modifiers.

    VariableDeclaration *variable = malloc(sizeof(VariableDeclaration));
    
    variable->name = _strdup(nameToken->literal);
    variable->name_token = *nameToken;
    variable->modifiers = MODIFIER_NONE;
    if (is_mutable) variable->modifiers |= MODIFIER_MUTABLE;
    if (is_static) variable->modifiers |= MODIFIER_STATIC;
    if (is_public) variable->modifiers |= MODIFIER_PUBLIC;
    if (is_private) variable->modifiers |= MODIFIER_PRIVATE;

    TokenType type_annotation [] = {TOKEN_TYPE_NONE};
    // check for optional type annotation
    if (match(parser, (TokenType[]){TOKEN_COLON}, 1)) {
        TokenType possible_types[] = {TOKEN_TYPE_BOOL, TOKEN_TYPE_INT, TOKEN_TYPE_FLOAT, TOKEN_TYPE_CHAR, TOKEN_TYPE_STRING, TOKEN_TYPE_VOID};
            bool found_type = false;
        for (int i = 0; i < sizeof(possible_types) / sizeof(possible_types[0]); i++) {
            if (check(parser, possible_types[i])) {
                type_annotation [0] = possible_types[i];
                advance(parser);
                found_type = true;
                break;
            }
        }
        if (!found_type) {
            parseError(parser, "Expected type annotation after ':'.");
            free((void *)variable->name); // Free the variable name string
            return NULL;
        }
    
    }
    variable->typeannotation_types = type_annotation[0];

    // check equal sign for initializer
    if (!match(parser, (TokenType[]){TOKEN_EQUAL}, 1)) {
        parseError(parser, "Expected '=' after variable name.");
        free((void *)variable->name); // Free the variable name string
        return NULL;
    }

    variable->value = expression(parser);
    // check for semicolon at the end of the declaration
    if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
        parseError(parser, "Expected ';' after variable declaration.");
        free((void *)variable->name); // Free the variable name string
        return NULL;
    }

    return stmt_variable_declaration(variable);
}

Stmt *Declaration(Parser *parser){
    // check if the current token is 'let' or 'const' for variable declaration, and then check if the next token is an identifier for the variable name, and then check if the next token is '=' for variable initialization. If any of these checks fail, we report a parsing error with an appropriate message.
    if (match(parser, (TokenType[]){TOKEN_LET, TOKEN_CONST}, 2)) {
        return var_Declaration(parser);
    } else {
        parseError(parser, "Expected declaration.");
        synchronize(parser);
        return NULL;
    }
}

// Parse a statement, which can be an expression statement or other types of statements (if, while, etc.). For now, we just parse an expression statement.
Stmt *statement(Parser *parser) {
    // its do nothing helpful for now but we will use it later for parsing statements like if, while, etc.
    return stmt_expr(expression(parser));
}

//=================================
// we parse everything in the main parse function, which is the entry point for the parser. It will call the appropriate parsing functions based on the current token and build the AST accordingly.
//=================================

//we parse everything in the main parse function, which is the entry point for the parser. It will call the appropriate parsing functions based on the current token and build the AST accordingly.
StmtList parse(Parser *parser) {
    StmtList statements;
    statements.statements = NULL;
    statements.count = 0;
    statements.capacity = 0;

    while (!isAtEnd(parser)) {
        if (isAtEnd(parser)) break;
        Stmt *decl = Declaration(parser);
        if (decl != NULL) {
            if (statements.count >= statements.capacity) {
                int newCapacity = statements.capacity == 0 ? 8 : statements.capacity * 2;
                statements.statements = realloc(statements.statements, newCapacity * sizeof(Stmt *));
                statements.capacity = newCapacity;
            }
            statements.statements[statements.count++] = decl;
        } else {
            synchronize(parser);
        }
    }

    return statements;
}
// Free the memory used by the parser.
void freeParser(Parser *parser) {
    free(parser);
}