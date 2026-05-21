#include <stdio.h>
#include "errorHandling.h"

int hadError = 0;

// Error handling for Lunaris Language
// report an error with a message and the line number

// report an error with a message, the line number, and where the error occurred
void reportError(int line, const char* where, const char* message) {
    if (where[0] != '\0') {
        printf("[line %d] Error at %s: %s\n", line, where, message);
    } else {
        printf("[line %d] Error: %s\n", line, message);
    }
}

// report an error with a message and the line number
void error(int line, const char* message) {
    hadError = 1;
    reportError(line, "", message);
}

