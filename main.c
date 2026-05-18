#include <stdio.h>

#include "token.h"

// lexer
void initLexer(const char *filename);
void closeLexer();
Token getNextToken();

// parser
void parseProgram();

int main() {

    Token token;

    // ANALISE LEXICA 

    printf("===== ANALISE LEXICA =====\n\n");

    initLexer("codigo.star");

    do {

        token = getNextToken();

        printf("Token: %d | Lexema: %s\n",
               token.type,
               token.lexeme);

    } while (token.type != TOK_EOF);

    closeLexer();

    // ANALISE SINTATICA 

    printf("\n===== ANALISE SINTATICA =====\n");

    // reinicia lexer
    initLexer("codigo.star");

    parseProgram();

    closeLexer();

    return 0;
}
