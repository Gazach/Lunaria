#pragma once
#include "tokenType.h"

// C translation of:
//   class Token { TokenType type; String lexeme; Object literal; int line; }
//
// 'lexeme' is stored as a pointer into the source + length (no allocation).
// 'literal' is a string representation of the value, or NULL if none.
typedef struct {
    TokenType   type;
    const char* lexeme;   // points into source buffer
    int         length;   // length of lexeme
    const char* literal;  // evaluated literal value as string, or NULL
    int         line;
} Token;

// Equivalent of Token.toString()
void printToken(Token token);
