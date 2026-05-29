// helper code that being use everywhere in the interpreter. This file contains utility functions that are used across different parts of the interpreter, such as error handling, type checking, and other common operations.

#include "value.h"
#include "tokenType.h"


LunarisType token_type_to_type(TokenType token_type) {
    switch (token_type) {
        case TOKEN_INT: return TYPE_INT;
        case TOKEN_STRING: return TYPE_STRING;
        case TOKEN_BOOL: return TYPE_BOOLEAN;
        case TOKEN_FLOAT: return TYPE_FLOAT;
        case TOKEN_CHAR: return TYPE_CHARACTER;
        case TOKEN_VOID: return TYPE_VOID;
        default: return TYPE_NULL;
    }
}