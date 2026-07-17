#include <stdio.h>
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

// forward declaration for expression parsing functions
Expr *expression(Parser *parser); // Forward declaration of the expression parsing function

// forward declaration for statement parsing functions
Stmt *statement(Parser *parser); // Forward declaration of the statement parsing function
static Stmt *Declaration(Parser *parser); // Forward declaration of the declaration parsing function


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

// Check if the next token matches the given type without advancing the parser.
bool check_next(Parser *parser, TokenType type) {
    if (parser->current + 1 >= parser->token_count) return false;
    // printf("check_next: current=%d next type=%d looking for=%d\n", 
    //     parser->current, parser->data[parser->current + 1].type, type);
    // fflush(stdout);
    return parser->data[parser->current + 1].type == type;
}

// ========================
// Main Parser functions
// ========================


// Parse a primary expression, which can be a literal, variable, or parenthesized expression.
Expr *primary_base(Parser *parser) {
    if (match(parser, (TokenType[]){TOKEN_FALSE}, 1)) {
        return expr_boolean(false);
    }
    if (match(parser, (TokenType[]){TOKEN_TRUE}, 1)) {
        return expr_boolean(true);
    }
    if (match(parser, (TokenType[]){TOKEN_NIL}, 1)) {
        return expr_literal(0, TOKEN_NIL); 
    }
    if (match(parser, (TokenType[]){TOKEN_IDENTIFIER}, 1)) {
        Token name_token = *previous(parser);
        char *name = STRDUP(previous(parser)->literal);
        if (match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
            // Function call
            Expr **arguments = NULL;
            int arg_count = 0;
            int arg_capacity = 0;

            if (!check(parser, TOKEN_RIGHT_PAREN)) {
                do {
                    if (arg_count >= arg_capacity) {
                        int newCapacity = arg_capacity == 0 ? 8 : arg_capacity * 2;
                        arguments = realloc(arguments, newCapacity * sizeof(Expr *));
                        arg_capacity = newCapacity;
                    }
                    arguments[arg_count++] = expression(parser);
                } while (match(parser, (TokenType[]){TOKEN_COMMA}, 1));
            }

            consume(parser, TOKEN_RIGHT_PAREN, "Expected ')' after arguments.");
            return expr_call(name, TYPE_FUNCTION, arguments, arg_count);
        } else {
            // Variable
            return expr_variable(name, TYPE_NULL, name_token);
        }
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
    if (match(parser, (TokenType[]){TOKEN_LEFT_BRACKET}, 1)) {
        // Array literal: [1, 2, 3]
        Expr **elements = NULL;
        int count = 0;
        int capacity = 0;

        if (!check(parser, TOKEN_RIGHT_BRACKET)) {
            do {
                if (count >= capacity) {
                    int newCapacity = capacity == 0 ? 8 : capacity * 2;
                    elements = realloc(elements, newCapacity * sizeof(Expr *));
                    capacity = newCapacity;
                }
                elements[count++] = expression(parser);
            } while (match(parser, (TokenType[]){TOKEN_COMMA}, 1));
        }

        consume(parser, TOKEN_RIGHT_BRACKET, "Expected ']' after array elements.");
        return expr_array_literal(elements, count);
    }
    // Handle error: expected expression
    parseError(parser, "Expected expression.");
    return NULL;
}

// Wraps primary_base with a postfix "[index]" loop, so indexing works on
// anything a primary expression can produce — a variable (arr[0]), a call
// result (getArr()[0]), a literal ([1,2,3][0]), and chains/nesting
// (arr[0][1]) all fall out of this for free.
Expr *primary(Parser *parser) {
    Expr *expr = primary_base(parser);

    while (match(parser, (TokenType[]){TOKEN_LEFT_BRACKET}, 1)) {
        Expr *index = expression(parser);
        consume(parser, TOKEN_RIGHT_BRACKET, "Expected ']' after index.");
        expr = expr_index(expr, index);
    }

    return expr;
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

    while (match(parser, (TokenType[]){TOKEN_STAR, TOKEN_SLASH, TOKEN_PERCENT}, 3)) {
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

// Parse a logical AND expression (a && b, a and b). Sits above equality so that
// `a == b && c == d` groups as `(a == b) && (c == d)`.
Expr *logic_and(Parser *parser) {
    Expr *expr = equality(parser);

    while (match(parser, (TokenType[]){TOKEN_AND_AND, TOKEN_AND}, 2)) {
        Token *operator = previous(parser);
        Expr *right = equality(parser);
        expr = expr_logical(operator->type, expr, right);
    }

    return expr;
}

// Parse a logical OR expression (a || b, a or b). Lowest-precedence binary operator,
// so `a && b || c && d` groups as `(a && b) || (c && d)`.
Expr *logic_or(Parser *parser) {
    Expr *expr = logic_and(parser);

    while (match(parser, (TokenType[]){TOKEN_OR_OR, TOKEN_OR}, 2)) {
        Token *operator = previous(parser);
        Expr *right = logic_and(parser);
        expr = expr_logical(operator->type, expr, right);
    }

    return expr;
}


// Parse a ternary expression (condition ? then_branch : else_branch).
// Sits above logic_or so `a || b ? c : d` groups as `(a || b) ? c : d`.
// Right-associative, so `a ? b : c ? d : e` groups as `a ? b : (c ? d : e)`,
// matching C's ternary chaining.
Expr *ternary(Parser *parser) {
    Expr *condition = logic_or(parser);

    if (match(parser, (TokenType[]){TOKEN_QUESTION_MARK}, 1)) {
        Expr *then_branch = expression(parser);
        if (!match(parser, (TokenType[]){TOKEN_COLON}, 1)) {
            parseError(parser, "Expected ':' in ternary expression.");
            return NULL;
        }
        Expr *else_branch = ternary(parser);
        return expr_ternary(condition, then_branch, else_branch);
    }

    return condition;
}

// Parse an expression, which can be a ternary expression followed by an
// optional assignment operator and another expression. The left-hand side
// is parsed as a normal expression first, then checked for whether it's a
// valid assignment target (a bare variable, or an index expression like
// arr[0]) — anything else after "=" is a parse error.
Expr *expression(Parser *parser) {
    Expr *expr = ternary(parser);

    if (match(parser, (TokenType[]){TOKEN_EQUAL}, 1)) {
        Expr *value = expression(parser); // right-associative: a = b = 5, arr[0] = arr[1] = 5

        if (expr->type == EXPR_VARIABLE) {
            char *name = expr->variable.name; // take ownership, don't leak the original
            Token name_token = expr->variable.name_token;
            free(expr); // free just the shell — its name string now belongs to expr_assign
            return expr_assign(name, name_token, value);
        } else if (expr->type == EXPR_INDEX) {
            Expr *array_expr = expr->index.array;
            Expr *index_expr = expr->index.index;
            free(expr); // shell only; array/index sub-expressions transfer to the new node
            return expr_index_assign(array_expr, index_expr, value);
        } else {
            parseError(parser, "Invalid assignment target.");
            return NULL;
        }
    }
    return expr;
}

//====================================================================================================================================================
// STATEMENT PARSING PART
//====================================================================================================================================================

// block statement, which is a sequence of statements enclosed in braces.
Stmt *parse_block(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_LEFT_BRACE}, 1)) {
        parseError(parser, "Expected '{' at the beginning of a block.");
        return NULL;
    }

    Stmt **statements = NULL;
    int statement_count = 0;
    int statement_capacity = 0;

    while (!check(parser, TOKEN_RIGHT_BRACE) && !isAtEnd(parser)) {
        Stmt *stmt = Declaration(parser);
        if (stmt != NULL) {
            if (statement_count >= statement_capacity) {
                int newCapacity = statement_capacity == 0 ? 8 : statement_capacity * 2;
                statements = realloc(statements, newCapacity * sizeof(Stmt *));
                statement_capacity = newCapacity;
            }
            statements[statement_count++] = stmt;
        } else {
            synchronize(parser);
        }
    }

    if (!match(parser, (TokenType[]){TOKEN_RIGHT_BRACE}, 1)) {
        parseError(parser, "Expected '}' at the end of a block.");
        // Free any statements that were parsed before the error
        for (int i = 0; i < statement_count; i++) {
            stmt_free(statements[i]);
        }
        free(statements);
        return NULL;
    }

    return stmt_block(statements, statement_count);
}


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

    VariableDeclaration *variable = calloc(1, sizeof(VariableDeclaration));
    
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

// Parse an assignment statement, which consists of an identifier, an equal sign, and an expression.
Stmt *parse_assignment(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_IDENTIFIER}, 1)) {
        parseError(parser, "Expected variable name.");
        return NULL;
    }
    Token name_token = *previous(parser);
    char *name = STRDUP(name_token.literal);

    match(parser, (TokenType[]){TOKEN_EQUAL}, 1); // consume =

    Expr *value = expression(parser);

    if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
        parseError(parser, "Expected ';' after assignment.");
        free(name);
        return NULL;
    }
    return stmt_assign(name, name_token, value);
}

// function declaration statement.
Stmt *parse_FunctionDeclaration(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_FUNCTION}, 1)) {
        parseError(parser, "Expected 'fn' as keyword.");
        return NULL;
    }
    if (!match(parser, (TokenType[]){TOKEN_IDENTIFIER}, 1)) {
        parseError(parser, "Expected function name.");
        return NULL;
    }
    char *name = STRDUP(previous(parser)->literal);

    if (!match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
        parseError(parser, "Expected '(' after function name.");
        free(name);
        return NULL;
    }

    // parse parameters
    char **param_names = NULL;
    int param_count = 0;
    int param_capacity = 0;

    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        do {
            if (!match(parser, (TokenType[]){TOKEN_IDENTIFIER}, 1)) {
                parseError(parser, "Expected parameter name.");
                free(name);
                return NULL;
            }
            if (param_count >= param_capacity) {
                int newCapacity = param_capacity == 0 ? 4 : param_capacity * 2;
                param_names = realloc(param_names, newCapacity * sizeof(char *));
                param_capacity = newCapacity;
            }
            param_names[param_count++] = STRDUP(previous(parser)->literal);
        } while (match(parser, (TokenType[]){TOKEN_COMMA}, 1));
    }

    if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
        parseError(parser, "Expected ')' after function parameters.");
        free(name);
        return NULL;
    }

    Stmt *body = parse_block(parser);
    if (!body) {
        parseError(parser, "Expected function scope.");
        free(name);
        return NULL;
    }

    return stmt_function_declaration(
        name,
        param_names, NULL, param_count,
        NULL,
        body->block_stmt.statements,
        body->block_stmt.statement_count
    );
}

Stmt *parse_return(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_RETURN}, 1)) {
        parseError(parser, "Expected 'return' as keyword.");
        return NULL;
    }
    Expr *value = NULL;
    if (!check(parser, TOKEN_SEMICOLON)) {
        value = expression(parser);
    }
    if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
        parseError(parser, "Expected ';' after return value.");
        return NULL;
    }
    return stmt_return(value);
}

// Parse an if statement, which consists of an 'if' keyword
Stmt *parse_if(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_IF}, 1)) {
        parseError(parser, "Expected 'if' as keyword.");
        return NULL;
    }
    if (!match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
        parseError(parser, "Expected '(' after 'if'.");
        return NULL;
    }
    Expr *condition = expression(parser);

    if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
        parseError(parser, "Expected ')' after if condition.");
        return NULL;
    }
    Stmt *then_branch = parse_block(parser);

    if (!then_branch) {
        parseError(parser, "Expected block '{' after 'if' condition.");
        return NULL;
    }

    // elif branches
    Expr **elif_conditions = NULL;
    Stmt **elif_branches = NULL;
    int elif_count = 0;

    while (check(parser, TOKEN_ELIF)) {
        advance(parser); // consume elif
        if (!match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
            parseError(parser, "Expected '(' after 'elif'.");
            return NULL;
        }
        Expr *elif_cond = expression(parser);
        if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
            parseError(parser, "Expected ')' after elif condition.");
            return NULL;
        }
        Stmt *elif_body = parse_block(parser);
        if (!elif_body) {
            parseError(parser, "Expected '{' after elif condition.");
            return NULL;
        }
        elif_count++;
        elif_conditions = realloc(elif_conditions, elif_count * sizeof(Expr *));
        elif_branches   = realloc(elif_branches,   elif_count * sizeof(Stmt *));
        elif_conditions[elif_count - 1] = elif_cond;
        elif_branches[elif_count - 1]   = elif_body;
    }

    Stmt *else_branch = NULL;
    if (match(parser, (TokenType[]){TOKEN_ELSE}, 1)) {
        else_branch = parse_block(parser);
        if (!else_branch) {
            parseError(parser, "Expected block '{' after 'else' keyword.");
            return NULL;
        }
    }

    return stmt_if(condition, then_branch, else_branch, elif_conditions, elif_branches, elif_count);
}

// Parse a for statement, which consists of a 'for' keyword, an initializer, a condition, an increment expression, and a body.
Stmt *parse_for(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_FOR}, 1)) {
        parseError(parser, "Expected 'for' as keyword.");
        return NULL;
    }

    if (!match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
        parseError(parser, "Expected '(' after 'for'.");
        return NULL;
    }
    
    Stmt *initializer = NULL;
    if (!check(parser, TOKEN_SEMICOLON)) {
        initializer = Declaration(parser);
        // A for-loop's own control variable has to be mutable for the
        // increment clause to ever work — force it regardless of whether
        // "mut" was written, since `for (let i = 0; ...; i = i + 1)`
        // would otherwise fail on the very first increment and the loop
        // would never terminate (the condition never changes).
        if (initializer != NULL && initializer->type == STMT_VARIABLE_DECLARATION) {
            initializer->variable_declaration_stmt.variable->modifiers |= MODIFIER_MUTABLE;
        }
    } else {
        match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1); // consume the semicolon if there's no initializer
    }
    Expr *condition = NULL;
    if (!check(parser, TOKEN_SEMICOLON)) {
        condition = expression(parser);
    }
    if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
        parseError(parser, "Expected ';' after loop condition.");
        return NULL;
    }
    Expr *increment = NULL;
    if (!check(parser, TOKEN_RIGHT_PAREN)) {
        increment = expression(parser);
    }
    if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
        parseError(parser, "Expected ')' after for clauses.");
        return NULL;
    }
    Stmt *body = parse_block(parser);
    if (!body) {
        parseError(parser, "Expected block '{' after for clauses.");
        return NULL;
    }
    return stmt_for(initializer, condition, increment, body);
}

// Parse a while statement: 'while' '(' condition ')' '{' body '}'.
Stmt *parse_while(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_WHILE}, 1)) {
        parseError(parser, "Expected 'while' as keyword.");
        return NULL;
    }

    if (!match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
        parseError(parser, "Expected '(' after 'while'.");
        return NULL;
    }

    Expr *condition = expression(parser);

    if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
        parseError(parser, "Expected ')' after while condition.");
        return NULL;
    }

    Stmt *body = parse_block(parser);
    if (!body) {
        parseError(parser, "Expected block '{' after while condition.");
        return NULL;
    }

    return stmt_while(condition, body);
}

// parsing break for loop
Stmt *parse_break(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_BREAK}, 1)) {
        parseError(parser, "Expected 'break' as keyword.");
        return NULL;
    }
    if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
        parseError(parser, "Expected ';' after 'break'.");
        return NULL;
    }
    return stmt_break();
}

// parsing continue for loop
Stmt *parse_continue(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_CONTINUE}, 1)) {
        parseError(parser, "Expected 'continue' as keyword.");
        return NULL;
    }
    if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
        parseError(parser, "Expected ';' after 'continue'.");
        return NULL;
    }
    return stmt_continue();
}

// parse the statements inside one case/default body: everything up to the
// next "case", "default", or the closing "}" of the switch.
static Stmt **parse_case_body(Parser *parser, int *out_count) {
    Stmt **body = NULL;
    int count = 0;
    int capacity = 0;

    while (!check(parser, TOKEN_CASE) && !check(parser, TOKEN_DEFAULT) &&
           !check(parser, TOKEN_RIGHT_BRACE) && !isAtEnd(parser)) {
        Stmt *stmt = Declaration(parser);
        if (stmt != NULL) {
            if (count >= capacity) {
                int newCapacity = capacity == 0 ? 4 : capacity * 2;
                body = realloc(body, newCapacity * sizeof(Stmt *));
                capacity = newCapacity;
            }
            body[count++] = stmt;
        } else {
            synchronize(parser);
        }
    }

    *out_count = count;
    return body;
}

// parsing switch statements, e.g.
// switch (i) {
//     case 1:
//         print("one");
//     case 2:
//         print("two");
//     default:
//         print("something else");
// }
// Note: no fall-through between cases — each matching case's body runs and
// the switch is done. "break;" still works if you want to bail out of a
// case body early; "continue;"/"return;" pass through to an enclosing
// loop/function like normal.
Stmt *parse_switch(Parser *parser) {
    if (!match(parser, (TokenType[]){TOKEN_SWITCH}, 1)) {
        parseError(parser, "Expected 'switch' as keyword.");
        return NULL;
    }
    if (!match(parser, (TokenType[]){TOKEN_LEFT_PAREN}, 1)) {
        parseError(parser, "Expected '(' after 'switch'.");
        return NULL;
    }

    Expr *switch_value = expression(parser);

    if (!match(parser, (TokenType[]){TOKEN_RIGHT_PAREN}, 1)) {
        parseError(parser, "Expected ')' after switch expression.");
        return NULL;
    }
    if (!match(parser, (TokenType[]){TOKEN_LEFT_BRACE}, 1)) {
        parseError(parser, "Expected '{' after switch expression.");
        return NULL;
    }

    Stmt **cases = NULL;
    int case_count = 0;
    int case_capacity = 0;
    Stmt *default_case = NULL;

    while (!check(parser, TOKEN_RIGHT_BRACE) && !isAtEnd(parser)) {
        if (match(parser, (TokenType[]){TOKEN_CASE}, 1)) {
            Expr *value = expression(parser);
            if (!match(parser, (TokenType[]){TOKEN_COLON}, 1)) {
                parseError(parser, "Expected ':' after case value.");
                return NULL;
            }
            int body_count = 0;
            Stmt **body = parse_case_body(parser, &body_count);

            if (case_count >= case_capacity) {
                int newCapacity = case_capacity == 0 ? 4 : case_capacity * 2;
                cases = realloc(cases, newCapacity * sizeof(Stmt *));
                case_capacity = newCapacity;
            }
            cases[case_count++] = stmt_case(value, body, body_count);
        } else if (match(parser, (TokenType[]){TOKEN_DEFAULT}, 1)) {
            if (default_case != NULL) {
                parseError(parser, "Switch statement can only have one 'default' case.");
                return NULL;
            }
            if (!match(parser, (TokenType[]){TOKEN_COLON}, 1)) {
                parseError(parser, "Expected ':' after 'default'.");
                return NULL;
            }
            int body_count = 0;
            Stmt **body = parse_case_body(parser, &body_count);
            // reuse stmt_case with a NULL value as the "this is default" marker —
            // it's never matched against, just executed when nothing else matches.
            default_case = stmt_case(NULL, body, body_count);
        } else {
            parseError(parser, "Expected 'case' or 'default' inside switch body.");
            return NULL;
        }
    }

    if (!match(parser, (TokenType[]){TOKEN_RIGHT_BRACE}, 1)) {
        parseError(parser, "Expected '}' after switch body.");
        return NULL;
    }

    return stmt_switch(switch_value, cases, case_count, default_case);
}

// Parse a declaration, which can be a variable declaration, function declaration, or other types of declarations (class, import, etc.). For now, we just parse variable declarations and function declarations.
Stmt *Declaration(Parser *parser){
    // check if the current token is 'let' or 'const' for variable declaration, and then check if the next token is an identifier for the variable name, and then check if the next token is '=' for variable initialization. If any of these checks fail, we report a parsing error with an appropriate message.
    if (match(parser, (TokenType[]){TOKEN_LET, TOKEN_CONST}, 2)) {
        return var_Declaration(parser);
    }  else if (check(parser, TOKEN_IDENTIFIER) && check_next(parser, TOKEN_EQUAL)) {
        return parse_assignment(parser);
    } else if (check(parser, TOKEN_LEFT_BRACE)) {
        return parse_block(parser);
    } else if (check(parser, TOKEN_FUNCTION)) {
        return parse_FunctionDeclaration(parser);
    } else if (check(parser, TOKEN_RETURN)) {
        return parse_return(parser);
    } else if (check(parser, TOKEN_IF)) {
        return parse_if(parser);
    } else if (check(parser, TOKEN_FOR)) {
        return parse_for(parser);
    } else if (check(parser, TOKEN_WHILE)) {
        return parse_while(parser);
    } else if (check(parser, TOKEN_BREAK)) {
        return parse_break(parser);
    } else if (check(parser, TOKEN_CONTINUE)) {
        return parse_continue(parser);
    } else if (check(parser, TOKEN_SWITCH)) {
        return parse_switch(parser);
    }
    else {
        Expr *expr = expression(parser);
        if (expr == NULL) {
            parseError(parser, "Expected declaration.");
            synchronize(parser);
            return NULL;
        }
        if (!match(parser, (TokenType[]){TOKEN_SEMICOLON}, 1)) {
            parseError(parser, "Expected ';' after expression.");
            return NULL;
        }
        return stmt_expr(expr);
    }
}


// Parse a statement, which can be an expression statement or other types of statements (if, while, etc.). For now, we just parse an expression statement.
Stmt *statement(Parser *parser) {
    // its do nothing helpful for now but we will use it later for parsing statements like if, while, etc.
    return stmt_expr(expression(parser));
}

// parse block statememt.

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