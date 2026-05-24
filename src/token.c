#include <stdio.h>
#include "token.h"

static const char* tokenTypeName(TokenType type) {
    switch (type) {
        case LEFT_PAREN:            return "LEFT_PAREN";
        case RIGHT_PAREN:           return "RIGHT_PAREN";
        case LEFT_BRACE:            return "LEFT_BRACE";
        case RIGHT_BRACE:           return "RIGHT_BRACE";
        case COMMA:                 return "COMMA";
        case DOT:                   return "DOT";
        case MINUS:                 return "MINUS";
        case PLUS:                  return "PLUS";
        case SEMICOLON:             return "SEMICOLON";
        case SLASH:                 return "SLASH";
        case STAR:                  return "STAR";
        case NOT:                   return "NOT";
        case NOT_EQUAL:             return "NOT_EQUAL";
        case EQUAL:                 return "EQUAL";
        case EQUAL_EQUAL:           return "EQUAL_EQUAL";
        case GREATER:               return "GREATER";
        case GREATER_EQUAL:         return "GREATER_EQUAL";
        case LESS:                  return "LESS";
        case LESS_EQUAL:            return "LESS_EQUAL";
        case AND:                   return "AND";
        case AND_AND:               return "AND_AND";
        case OR:                    return "OR";
        case OR_OR:                 return "OR_OR";
        case CLASS:                 return "CLASS";
        case ELSE:                  return "ELSE";
        case FALSE:                 return "FALSE";
        case FUNCTION:              return "FUNCTION";
        case FOR:                   return "FOR";
        case IF:                    return "IF";
        case NIL:                   return "NIL";
        case RETURN:                return "RETURN";
        case TYPE_INT:              return "TYPE_INT";
        case TYPE_STRING:           return "TYPE_STRING";
        case TYPE_BOOL:             return "TYPE_BOOL";
        case TYPE_FLOATING_POINT:   return "TYPE_FLOATING_POINT";
        case SUPER:                 return "SUPER";
        case THIS:                  return "THIS";
        case TRUE:                  return "TRUE";
        case CONST:                 return "CONST";
        case WHILE:                 return "WHILE";
        case CONTINUE:              return "CONTINUE";
        case BREAK:                 return "BREAK";
        case ELIF:                  return "ELIF";
        case IMPORT:                return "IMPORT";
        case PRINT:                 return "PRINT";
        case IDENTIFIER:            return "IDENTIFIER";
        case STRING:                return "STRING";
        case NUMBER:                return "NUMBER";
        case EOF_TOKEN:             return "EOF";
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
