#pragma once

extern int hadError;

// error handling for Lunaris Language
// report an error with a message and the line number
void reportError(int line, const char* where, const char* message);
void error(int line, const char* message);
