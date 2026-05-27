#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "stmt.h"

typedef struct {
	Token *data;
	int token_count;
	int current;
	int line;
} Parser;

Parser *initParser(Token *tokens, int token_count);
StmtList parse(Parser *parser);
void freeParser(Parser *parser);

#endif // PARSER_H