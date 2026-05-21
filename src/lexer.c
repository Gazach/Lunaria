#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "errorHandling.h"

// ---- TokenArray helpers ----
// Dynamic array of tokens for storing scanned tokens during lexing
static void initTokenArray(TokenArray* array) {
    array->data = NULL;
    array->count = 0;
    array->capacity = 0;
}
// Push a token to the dynamic array, resizing if necessary
static void pushToken(TokenArray* array, Token token) {
    if (array->count >= array->capacity) {
        int newCapacity = array->capacity == 0 ? 8 : array->capacity * 2;
        array->data = (Token*)realloc(array->data, newCapacity * sizeof(Token));
        array->capacity = newCapacity;
    }
    array->data[array->count++] = token;
}
// Free the memory used by the token array
void freeTokenArray(TokenArray* array) {
    free(array->data);
    array->data = NULL;
    array->count = 0;
    array->capacity = 0;
}

// ---- Lexer helpers ----
// Initialize the lexer with the source code to be scanned
void initLexer(Lexer* lexer, const char* source) {
    lexer->start = source;
    lexer->current = source;
    lexer->line = 1;
}
// Check if we've reached the end of the source code
static int isAtEnd(Lexer* lexer) {
    return *lexer->current == '\0';
}
// Create a token with the given type, using the current lexeme and line number
static Token makeToken(Lexer* lexer, TokenType type) {
    Token token;
    token.type    = type;
    token.lexeme  = lexer->start;
    token.length  = (int)(lexer->current - lexer->start);
    token.literal = NULL;
    token.line    = lexer->line;
    return token;
}

// ---- Scanning ----

// Scan the source code and return an array of tokens
static void scanToken(Lexer* lexer, TokenArray* tokens);

// Main function to scan tokens from the source code
TokenArray scanTokens(Lexer* lexer) {
    TokenArray tokens;
    initTokenArray(&tokens);

    while (!isAtEnd(lexer)) { // Scan tokens until we reach the end of the source
        lexer->start = lexer->current;
        scanToken(lexer, &tokens);
    }

    // Add an EOF token at the end of the token stream
    Token eof;
    eof.type    = EOF_TOKEN;
    eof.lexeme  = "";
    eof.length  = 0;
    eof.literal = NULL;
    eof.line    = lexer->line;
    pushToken(&tokens, eof);

    return tokens;
}

// Scan a single token and add it to the tokens array
static void scanToken(Lexer* lexer, TokenArray* tokens) {
    // TODO: implement full token scanning
    lexer->current++;
}
