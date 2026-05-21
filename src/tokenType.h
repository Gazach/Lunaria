#pragma once
#include <string.h>

// ALL TOKEN TYPES FOR LUNARIS LANGUAGE
// Token used for lexing and parsing where each token has a type and a value
enum TokenType {
    // Single-character tokens
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, MINUS, PLUS, SEMICOLON, SLASH, STAR,

    // One or two character tokens
    NOT,        NOT_EQUAL,
    EQUAL,      EQUAL_EQUAL,
    GREATER,    GREATER_EQUAL,
    LESS,       LESS_EQUAL,

    // Logical operators
    AND,        AND_AND,
    OR,         OR_OR,
    
    // Keywords
    CLASS, ELSE, FALSE, FUNCTION, FOR, IF, NIL,
    RETURN, SUPER, THIS, TRUE, LET, CONST, WHILE, 
    CONTINUE, BREAK, ELIF, IMPORT,

    // Literals
    IDENTIFIER, STRING, NUMBER,

    // End of file
    EOF_TOKEN
};
// Define TokenType as an alias for the enum
typedef enum TokenType TokenType;