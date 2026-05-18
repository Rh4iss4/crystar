#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "token.h"

typedef struct {
    char *name;
    TokenType type;
} Keyword;

// KEYWORDS 

Keyword keywords[] = {
    {"integer", TOK_INTEGER},
    {"real", TOK_REAL},
    {"char", TOK_CHAR},
    {"literal", TOK_LITERAL},
    {"bool", TOK_BOOL},
    {"if", TOK_IF},
    {"else", TOK_ELSE},
    {"for", TOK_FOR},
    {"true", TOK_TRUE},
    {"false", TOK_FALSE},
    {"void", TOK_VOID},
    {"null", TOK_NULL}
};

int keywordCount = sizeof(keywords) / sizeof(Keyword);

// FILE 

FILE *fp;
int currentChar;

// FUNÇÕES 

void nextChar() {
    currentChar = fgetc(fp);
}

void initLexer(const char *filename) {
    fp = fopen(filename, "r");

    if (!fp) {
        printf("Erro ao abrir arquivo\n");
        exit(1);
    }

    nextChar();
}

void closeLexer() {
    fclose(fp);
}

int isKeyword(char *str, TokenType *type) {
    for (int i = 0; i < keywordCount; i++) {
        if (strcmp(str, keywords[i].name) == 0) {
            *type = keywords[i].type;
            return 1;
        }
    }

    return 0;
}

void skipWhitespace() {
    while (isspace(currentChar)) {
        nextChar();
    }
}

void skipComment() {
    nextChar();

    // comentário de linha
    if (currentChar == '/') {
        while (currentChar != '\n' && currentChar != EOF) {
            nextChar();
        }
    }

    // comentário de bloco
    else if (currentChar == '*') {
        int prev = 0;

        while (currentChar != EOF) {
            prev = currentChar;
            nextChar();

            if (prev == '*' && currentChar == '/') {
                nextChar();
                break;
            }
        }
    }

    else {
        ungetc(currentChar, fp);
        currentChar = '/';
    }
}

Token makeToken(TokenType type, const char *lexeme) {
    Token t;

    t.type = type;

    strncpy(t.lexeme, lexeme, MAX_LEXEMA - 1);
    t.lexeme[MAX_LEXEMA - 1] = '\0';

    return t;
}

// LEXER 

Token getNextToken() {

    skipWhitespace();

    // EOF
    if (currentChar == EOF) {
        return makeToken(TOK_EOF, "EOF");
    }

    // comentários
    if (currentChar == '/') {

        int next = fgetc(fp);
        ungetc(next, fp);

        if (next == '/' || next == '*') {
            skipComment();
            return getNextToken();
        }
    }

    char lexeme[MAX_LEXEMA];
    int i = 0;

    // IDENTIFICADORES

    if (isalpha(currentChar)) {

        while (isalnum(currentChar)) {

            if (i < MAX_LEXEMA - 1) {
                lexeme[i++] = currentChar;
            }

            nextChar();
        }

        lexeme[i] = '\0';

        TokenType type;

        if (isKeyword(lexeme, &type)) {
            return makeToken(type, lexeme);
        }

        return makeToken(TOK_ID, lexeme);
    }

    // NÚMEROS

    if (isdigit(currentChar)) {

        int isReal = 0;

        while (isdigit(currentChar)) {

            if (i < MAX_LEXEMA - 1) {
                lexeme[i++] = currentChar;
            }

            nextChar();
        }

        if (currentChar == '.') {

            isReal = 1;

            if (i < MAX_LEXEMA - 1) {
                lexeme[i++] = currentChar;
            }

            nextChar();

            if (!isdigit(currentChar)) {
                return makeToken(TOK_ERROR, "real mal formado");
            }

            while (isdigit(currentChar)) {

                if (i < MAX_LEXEMA - 1) {
                    lexeme[i++] = currentChar;
                }

                nextChar();
            }
        }

        lexeme[i] = '\0';

        if (isReal) {
            return makeToken(TOK_REAL_LITERAL, lexeme);
        }

        return makeToken(TOK_INT_LITERAL, lexeme);
    }

    // Literal 

    if (currentChar == '"') {

        nextChar();

        while (currentChar != '"' && currentChar != EOF) {

            if (i < MAX_LEXEMA - 1) {
                lexeme[i++] = currentChar;
            }

            nextChar();
        }

        if (currentChar == EOF) {
            return makeToken(TOK_ERROR, "string nao fechada");
        }

        nextChar();

        lexeme[i] = '\0';

        return makeToken(TOK_STRING_LITERAL, lexeme);
    }

    // CHAR 

    if (currentChar == '\'') {

        nextChar();

        if (currentChar == '\'' || currentChar == EOF) {
            return makeToken(TOK_ERROR, "char invalido");
        }

        lexeme[i++] = currentChar;

        nextChar();

        if (currentChar != '\'') {
            return makeToken(TOK_ERROR, "char mal formado");
        }

        nextChar();

        lexeme[i] = '\0';

        return makeToken(TOK_CHAR_LITERAL, lexeme);
    }

    // OPERADORES

    switch (currentChar) {

        case '+':
            nextChar();
            return makeToken(TOK_PLUS, "+");

        case '-':
            nextChar();
            return makeToken(TOK_MINUS, "-");

        case '*':

            nextChar();

            if (currentChar == '*') {
                nextChar();
                return makeToken(TOK_POW, "**");
            }

            return makeToken(TOK_MULT, "*");

        case '/':
            nextChar();
            return makeToken(TOK_DIV, "/");

        case '=':

            nextChar();

            if (currentChar == '=') {
                nextChar();
                return makeToken(TOK_EQ, "==");
            }

            return makeToken(TOK_ASSIGN, "=");

        case '!':

            nextChar();

            if (currentChar == '=') {
                nextChar();
                return makeToken(TOK_NE, "!=");
            }

            return makeToken(TOK_ERROR, "!");

        case '>':

            nextChar();

            if (currentChar == '=') {
                nextChar();
                return makeToken(TOK_GTE, ">=");
            }

            return makeToken(TOK_GT, ">");

        case '<':

            nextChar();

            if (currentChar == '=') {
                nextChar();
                return makeToken(TOK_LTE, "<=");
            }

            return makeToken(TOK_LT, "<");

        // DELIMITADORES 

        case ';':
            nextChar();
            return makeToken(TOK_SEMICOLON, ";");

        case '{':
            nextChar();
            return makeToken(TOK_LEFT_BRACE, "{");

        case '}':
            nextChar();
            return makeToken(TOK_RIGHT_BRACE, "}");

        case '(':
            nextChar();
            return makeToken(TOK_LEFT_PAREN, "(");

        case ')':
            nextChar();
            return makeToken(TOK_RIGHT_PAREN, ")");

        default: {

            char err[2] = {currentChar, '\0'};

            nextChar();

            return makeToken(TOK_ERROR, err);
        }
    }
}