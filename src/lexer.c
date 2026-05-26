#include <corecrt_search.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "lexer.h"
#include "errorHandling.h"
#include "tokenType.h"

//==============================
// ---- Keywords table ----
// C equivalent of: static final Map<String, TokenType> keywords
//==============================

typedef struct {
    const char* word;
    TokenType   type;
} Keyword;

static const Keyword keywords[] = {
    { "and",      TOKEN_AND      },
    { "break",    TOKEN_BREAK    },
    { "class",    TOKEN_CLASS    },
    { "const",    TOKEN_CONST    },
    { "let",       TOKEN_LET      },
    { "continue", TOKEN_CONTINUE },
    { "elif",     TOKEN_ELIF     },
    { "else",     TOKEN_ELSE     },
    { "false",    TOKEN_FALSE    },
    { "for",      TOKEN_FOR      },
    { "fn", TOKEN_FUNCTION },
    { "if",       TOKEN_IF       },
    { "import",   TOKEN_IMPORT   },
    { "null",      TOKEN_NIL      },
    { "or",       TOKEN_OR       },
    { "print",    TOKEN_PRINT    },
    { "return",   TOKEN_RETURN   },
    { "super",    TOKEN_SUPER    },
    { "this",     TOKEN_THIS     },
    { "true",     TOKEN_TRUE     },
    { "while",    TOKEN_WHILE    },
    { "int",       TOKEN_TYPE_INT},
    { "string",    TOKEN_TYPE_STRING},
    { "bool",      TOKEN_TYPE_BOOL},
    { "float",     TOKEN_TYPE_FLOAT},
    { "char",      TOKEN_TYPE_CHAR},
    { "void",      TOKEN_TYPE_VOID},
    { "switch",    TOKEN_SWITCH},
    { "case",      TOKEN_CASE},
    { "mut",   TOKEN_MUTABLE},
    { "static",    TOKEN_STATIC},
    { "pub",    TOKEN_PUBLIC},
    { "priv",   TOKEN_PRIVATE},
    { "default",   TOKEN_DEFAULT},
};

// Returns the TokenType for the given identifier, or IDENTIFIER if not a keyword.
// Equivalent of: keywords.getOrDefault(text, null)
static TokenType getKeywordType(const char* text, int length) {
    for (int i = 0; i < (int)(sizeof(keywords) / sizeof(keywords[0])); i++) {
        if ((int)strlen(keywords[i].word) == length &&
            strncmp(keywords[i].word, text, length) == 0) {
            return keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

//==============================
// ---- TokenArray helpers ----
//==============================

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

//===========================
// ---- Lexer helpers ----
//===========================

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

// Advance the current pointer and return the next character
static int advance(Lexer* lexer) {
    return *lexer->current++;
}

// Conditionally advance if the next character matches expected
static int match(Lexer* lexer, char expected) {
    if (isAtEnd(lexer)) return 0;
    if (*lexer->current != expected) return 0;
    lexer->current++;
    return 1;
}
// Look at the current character without consuming it
static char peek(Lexer* lexer) {
    if (isAtEnd(lexer)) {
        return '\0';
    }
    return *lexer->current;
}

static char peekNext(Lexer* lexer) {
    if (isAtEnd(lexer)) return '\0';
    if (*(lexer->current + 1) == '\0') return '\0';
    return *(lexer->current + 1);
}

// ========================
// Main Lexer functions
// ========================

// Add a token with the given type and no literal value
#define addToken(lexer, tokens, type) addTokenLiteral(lexer, tokens, type, NULL)

// Add a token with the given type and literal value
static void addTokenLiteral(Lexer* lexer, TokenArray* tokens, TokenType type, const char* literal) {
    Token token = makeToken(lexer, type);
    token.literal = literal;
    pushToken(tokens, token);
}

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
    eof.type    = TOKEN_EOF;
    eof.lexeme  = "";
    eof.length  = 0;
    eof.literal = NULL;
    eof.line    = lexer->line;
    pushToken(&tokens, eof);

    return tokens;
}
// Scan a string literal, handling unterminated strings and escape sequences
static void string(Lexer* lexer, TokenArray* tokens) {
    while (peek(lexer) != '"' && !isAtEnd(lexer)) {
        if (peek(lexer) == '\n') lexer->line++;
        advance(lexer);
    }

    if (isAtEnd(lexer)) {
        error(makeToken(lexer, TOKEN_EOF), "Unterminated string.");
        return;
    }

    // The closing ".
    advance(lexer);

    // Trim the surrounding quotes.
    int length = (int)(lexer->current - lexer->start - 2);
    char* value = (char*)malloc(length + 1);
    strncpy_s(value, length + 1, lexer->start + 1, length);
    value[length] = '\0';

    addTokenLiteral(lexer, tokens, TOKEN_STRING, value);
}

// Check if a character is a digit (0-9)
static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

// Scan a number literal, handling integer and floating-point formats
static void number(Lexer* lexer, TokenArray* tokens) {
    while (isDigit(peek(lexer))) advance(lexer);

    // Look for a fractional part.
    // if we see a '.' followed by a digit, it's a floating-point number
    if (peek(lexer) == '.' && isDigit(peekNext(lexer))) {
        // Consume the "."
        advance(lexer);

        while (isDigit(peek(lexer))) advance(lexer);
    }

    int length = (int)(lexer->current - lexer->start);
    char* value = (char*)malloc(length + 1);
    strncpy_s(value, length + 1, lexer->start, length);
    value[length] = '\0';

    // Add the number token with the literal value as a string
    addTokenLiteral(lexer, tokens, TOKEN_NUMBER, value);
}

static bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
            c == '_';
}

static bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

static void identifier(Lexer* lexer, TokenArray* tokens) {
    while (isAlphaNumeric(peek(lexer))) advance(lexer);

    int length = (int)(lexer->current - lexer->start);
    TokenType type = getKeywordType(lexer->start, length);
    addToken(lexer, tokens, type);
}
// Scan a single token from the source code and add it to the token array
static void scanToken(Lexer* lexer, TokenArray* tokens) {
    char c = advance(lexer);

    switch (c) {
        // read single-character tokens and add them to the token array
        case '(': addToken(lexer, tokens, TOKEN_LEFT_PAREN); break;
        case ')': addToken(lexer, tokens, TOKEN_RIGHT_PAREN); break;
        case '{': addToken(lexer, tokens, TOKEN_LEFT_BRACE); break;
        case '}': addToken(lexer, tokens, TOKEN_RIGHT_BRACE); break;
        case ',': addToken(lexer, tokens, TOKEN_COMMA); break;
        case '.': addToken(lexer, tokens, TOKEN_DOT); break;
        case '-': addToken(lexer, tokens, TOKEN_MINUS); break;
        case '+': addToken(lexer, tokens, TOKEN_PLUS); break;
        case ';': addToken(lexer, tokens, TOKEN_SEMICOLON); break;
        case '*': addToken(lexer, tokens, TOKEN_STAR); break;
        case '!': addToken(lexer, tokens, match(lexer, '=') ? TOKEN_NOT_EQUAL : TOKEN_NOT); break;
        case '=': addToken(lexer, tokens, match(lexer, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL); break;
        case '<': addToken(lexer, tokens, match(lexer, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS); break;
        case '>': addToken(lexer, tokens, match(lexer, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER); break;
        case '&': addToken(lexer, tokens, match(lexer, '&') ? TOKEN_AND_AND : TOKEN_AND); break;
        case '|': addToken(lexer, tokens, match(lexer, '|') ? TOKEN_OR_OR : TOKEN_OR); break;
        case ':': 
            if (match(lexer, ':')) {
                addToken(lexer, tokens, TOKEN_DOUBLE_COLON);
            } else {
                addToken(lexer, tokens, TOKEN_COLON);
            }
            break;
        case '?': addToken(lexer, tokens, TOKEN_QUESTION_MARK); break;
        case '/':
            if (match(lexer, '/')) {
                // check if it's a comment and skip until the end of the line
                while (peek(lexer) != '\n' && !isAtEnd(lexer)) advance(lexer);
            } else {
                addToken(lexer, tokens, TOKEN_SLASH);
            }
            break;
        case ' ':
        case '\r':
        case '\t':
            // Ignore whitespace.
            break;
        case '\n':
            if (c == '\n') lexer->line++;
            break;
        case '"': string(lexer, tokens); break;

        default:
            if (isDigit(c)) {
                number(lexer, tokens);
            } else if (isAlpha(c)) {
                identifier(lexer, tokens);
            } else {
                error(makeToken(lexer, TOKEN_EOF), "Unexpected character.");
            }
            break;
    }
}
