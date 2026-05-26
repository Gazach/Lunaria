#include <stdio.h>
#include "token.h"

static const char* tokenTypeName(TokenType type) {
    switch (type) {
        case TOKEN_LEFT_PAREN:            return "LEFT_PAREN";
        case TOKEN_RIGHT_PAREN:           return "RIGHT_PAREN";
        case TOKEN_LEFT_BRACE:            return "LEFT_BRACE";
        case TOKEN_RIGHT_BRACE:           return "RIGHT_BRACE";
        case TOKEN_COMMA:                 return "COMMA";
        case TOKEN_DOT:                   return "DOT";
        case TOKEN_MINUS:                 return "MINUS";
        case TOKEN_PLUS:                  return "PLUS";
        case TOKEN_SEMICOLON:             return "SEMICOLON";
        case TOKEN_SLASH:                 return "SLASH";
        case TOKEN_STAR:                  return "STAR";
        case TOKEN_NOT:                   return "NOT";
        case TOKEN_NOT_EQUAL:             return "NOT_EQUAL";
        case TOKEN_EQUAL:                 return "EQUAL";
        case TOKEN_EQUAL_EQUAL:           return "EQUAL_EQUAL";
        case TOKEN_GREATER:               return "GREATER";
        case TOKEN_GREATER_EQUAL:         return "GREATER_EQUAL";
        case TOKEN_LESS:                  return "LESS";
        case TOKEN_LESS_EQUAL:            return "LESS_EQUAL";
        case TOKEN_AND:                   return "AND";
        case TOKEN_AND_AND:               return "AND_AND";
        case TOKEN_OR:                    return "OR";
        case TOKEN_OR_OR:                 return "OR_OR";
        case TOKEN_CLASS:                 return "CLASS";
        case TOKEN_ELSE:                  return "ELSE";
        case TOKEN_FALSE:                 return "FALSE";
        case TOKEN_FUNCTION:              return "FUNCTION";
        case TOKEN_FOR:                   return "FOR";
        case TOKEN_IF:                    return "IF";
        case TOKEN_NIL:                   return "NIL";
        case TOKEN_RETURN:                return "RETURN";
        case TOKEN_TYPE_INT:              return "TYPE_INT";
        case TOKEN_TYPE_STRING:           return "TYPE_STRING";
        case TOKEN_TYPE_BOOL:             return "TYPE_BOOL";
        case TOKEN_TYPE_FLOAT:   return "TYPE_FLOAT";
        case TOKEN_TYPE_CHAR:        return "TYPE_CHAR";
        case TOKEN_TYPE_VOID:             return "TYPE_VOID";
        case TOKEN_SUPER:                 return "SUPER";
        case TOKEN_THIS:                  return "THIS";
        case TOKEN_TRUE:                  return "TRUE";
        case TOKEN_CONST:                 return "CONST";
        case TOKEN_LET:                   return "LET";
        case TOKEN_WHILE:                 return "WHILE";
        case TOKEN_CONTINUE:              return "CONTINUE";
        case TOKEN_BREAK:                 return "BREAK";
        case TOKEN_ELIF:                  return "ELIF";
        case TOKEN_IMPORT:                return "IMPORT";
        case TOKEN_PRINT:                 return "PRINT";
        case TOKEN_IDENTIFIER:            return "IDENTIFIER";
        case TOKEN_NUMBER:                return "NUMBER";
        case TOKEN_SWITCH:                return "SWITCH";
        case TOKEN_CASE:                  return "CASE";
        case TOKEN_DEFAULT:               return "DEFAULT";
        case TOKEN_IMMUTABLE:             return "IMMUTABLE";
        case TOKEN_MUTABLE:               return "MUTABLE";
        case TOKEN_STATIC:                return "STATIC";
        case TOKEN_PUBLIC:                return "PUBLIC";
        case TOKEN_PRIVATE:               return "PRIVATE";
        case TOKEN_EOF:             return "EOF";
        default:                    return "UNKNOWN";
    }
}

// Equivalent of Token.toString(): type + lexeme + literal
void printToken(Token token) {
    if (token.literal != NULL) {
        printf("%s '%.*s' %s\n", tokenTypeName(token.type), token.length, token.lexeme, token.literal);
    } else {
        printf("%s '%.*s'\n", tokenTypeName(token.type), token.length, token.lexeme);
    }
}
