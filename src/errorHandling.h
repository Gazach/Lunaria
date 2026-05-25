#pragma once

extern int hadError;
#include "token.h"

// error handling for Lunaris Language
// report an error with a message and the line number
void reportError(int line, const char* where, const char* message);
void error(Token token, const char* message);
