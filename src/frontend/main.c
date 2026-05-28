
//===========================================
// Main entry point for Lunaris Language
// Interpreted Shorten to Lun for easier use in command line
//===========================================
#include <stdio.h>
#include <stdlib.h>

#include "../common/errorHandling.h"
#include "../frontend/lexer.h"
#include "../frontend/parser.h"
#include "../runtime/interpreter.h"
#include "../runtime/environment.h"

// Flag to indicate if a runtime error has occurred
int hadRuntimeError = 0;

// Run the given source code by lexing, parsing, and interpreting it.
void run(const char* source) {
    hadRuntimeError = 0;
    hadError = 0;
    printf("Lexing...\n");
    Lexer lexer;
    initLexer(&lexer, source);
    TokenArray tokens = scanTokens(&lexer);
    printf("Parsing...\n");
    Parser *parser = initParser(tokens.data, tokens.count);
    StmtList statements = parse(parser);
    printf("Freeing parser...\n");
    freeParser(parser);
    freeTokenArray(&tokens);

    printf("Interpreting...\n");
    Environment *env = create_environment();
    for (int i = 0; i < statements.count; i++) {
        hadRuntimeError = 0; // reset between statements
        interpreter(statements.statements[i], env);
    }
    free_environment(env);

    // free the list
    for (int i = 0; i < statements.count; i++) stmt_free(statements.statements[i]);
    free(statements.statements);
}
// run a file with the given filename
// if the file does not exist, print an error message and return
void runFile(const char* filename) {

    // read the file and if it exists execute it
    FILE* file = NULL;
    fopen_s(&file, filename, "rb");

    if (file == NULL) {
        printf("[Lunaris] : Could not open file %s\n", filename);
        hadError = 1;
        return;
    }

    // get the file size
    fseek(file, 0, SEEK_END); // move the file pointer to the end of the file
    long fileSize = ftell(file); // get the size of the file
    rewind(file); // go back to the beginning of the file

    // read the file into a buffer
    char* buffer = (char*)malloc(fileSize + 1);

    // check if the buffer was allocated successfully
    // if not, print an error message and return
    if (buffer == NULL) {
        printf("[Lunaris] : Could not allocate memory for file %s\n", filename);
        fclose(file); // close the file before returning
        hadError = 1;
        return;
    }

    // read file contents into buffer and null-terminate
    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0';
    fclose(file);

    // check if there was an error while running the file
    if(hadError) {
        printf("[Lunaris] : Error occurred while running file %s\n", filename);
        free(buffer); // free the buffer before returning
        return;
    } else {
        // run file into buffer
        run(buffer);
        hadError = 0; // reset error flag for next run
    }
    
    // free the buffer
    free(buffer);
}

// run the prompt to get user input and run it
void runPromt() {
    char input[1024]; // get user input with a buffer of 1024 characters
    while (1) { // infinite loop to get user input until they exit
        printf("[Lunaris] : > ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n[Lunaris] : Exiting...\n");
            break;
        }
        run(input); // run the user input
    }
}

// main entry point for the program
int main(int argc, char* argv[]) {
    // if there are more than 2 arguments, print an error message and return
    if (argc > 2) {
        printf("[Lunaris] : Usage Lun <file>\n");
        hadError = 1;
    }
    else if (argc == 2) {
        runFile(argv[1]);
    }
    else {
        runPromt();
    }
    return 0;
}