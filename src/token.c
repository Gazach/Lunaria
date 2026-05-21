#include <stdio.h>
#include "token.h"

// Equivalent of Token.toString(): type + lexeme + literal
void printToken(Token token) {
    if (token.literal != NULL) {
        printf("%d %.*s %s\n", token.type, token.length, token.lexeme, token.literal);
    } else {
        printf("%d %.*s null\n", token.type, token.length, token.lexeme);
    }
}
