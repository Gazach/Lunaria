#pragma once
#include "../common/token.h"
#include "../common/errorHandling.h"
#include "../common/tokenType.h"

typedef struct {
    Token* data;
    int count;
    int capacity;
} TokenArray;

typedef struct {
    const char* start;
    const char* current;
    int line;
} Lexer;

void initLexer(Lexer* lexer, const char* source);
TokenArray scanTokens(Lexer* lexer);
void freeTokenArray(TokenArray* array);
