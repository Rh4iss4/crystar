#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "token.h"
#include "semantic.h"

// ================= LEXER =================
Token getNextToken();

// token atual
Token currentToken;

// ================= UTIL =================

void syntaxError(const char *msg) {
    printf("\n===== ERRO SINTATICO =====\n");
    printf("%s\n", msg);
    printf("Token encontrado: %s\n", currentToken.lexeme);
    exit(1);
}

void advance() {
    currentToken = getNextToken();

    // ERRO LEXICO
    if (currentToken.type == TOK_ERROR) {
        printf("\n===== ERRO LEXICO =====\n");
        printf("Lexema invalido: %s\n", currentToken.lexeme);
        exit(1);
    }
}

void match(TokenType expected) {
    if (currentToken.type == expected) {
        advance();
    } else {
        syntaxError("token inesperado");
    }
}

// ================= PROTOTIPOS =================

void programa();
void listaDeclaracoes();
void declaracao();

void declaracaoVariavel();
void tipo();

void comando();
void atribuicao();

void comandoIf();
void comandoFor();

void bloco();

void expressao();
void expressaoRelacional();
void expressaoAritmetica();

void termo();
void fator();

void opRelacional();
void literal();

// ================= PARSER PRINCIPAL =================

void parseProgram() {

    semBeginProgram();

    currentToken = getNextToken();   // 🔥 CORREÇÃO PRINCIPAL

    if (currentToken.type == TOK_EOF) {
        syntaxError("programa vazio");
    }

    programa();

    if (currentToken.type != TOK_EOF) {
        syntaxError("tokens apos fim do programa");
    }

    semEndProgram();

    printf("\nAnalise sintatica e semantica concluida com sucesso!\n");
}

// ================= GRAMATICA =================

// programa ::= listaDeclaracoes
void programa() {
    listaDeclaracoes();
}

// listaDeclaracoes ::= declaracao*
void listaDeclaracoes() {
    while (
        currentToken.type == TOK_INTEGER ||
        currentToken.type == TOK_REAL ||
        currentToken.type == TOK_CHAR ||
        currentToken.type == TOK_LITERAL ||
        currentToken.type == TOK_BOOL ||
        currentToken.type == TOK_IF ||
        currentToken.type == TOK_FOR ||
        currentToken.type == TOK_LEFT_BRACE ||
        currentToken.type == TOK_ID
    ) {
        declaracao();
    }
}

// declaracao
void declaracao() {
    if (
        currentToken.type == TOK_INTEGER ||
        currentToken.type == TOK_REAL ||
        currentToken.type == TOK_CHAR ||
        currentToken.type == TOK_LITERAL ||
        currentToken.type == TOK_BOOL
    ) {
        declaracaoVariavel();
    } else {
        comando();
    }
}

// declaracao_variavel ::= tipo id = expressao ;
void declaracaoVariavel() {

    TokenType tipoVar = currentToken.type;

    tipo();

    char nome[MAX_LEXEMA];
    strcpy(nome, currentToken.lexeme);

    match(TOK_ID);

    semDeclareVariable(nome, tipoVar);

    match(TOK_ASSIGN);

    expressao();

    match(TOK_SEMICOLON);
}

// tipo
void tipo() {
    switch (currentToken.type) {

        case TOK_INTEGER: match(TOK_INTEGER); break;
        case TOK_REAL:    match(TOK_REAL); break;
        case TOK_CHAR:    match(TOK_CHAR); break;
        case TOK_LITERAL: match(TOK_LITERAL); break;
        case TOK_BOOL:    match(TOK_BOOL); break;

        default:
            syntaxError("tipo invalido");
    }
}

// comando
void comando() {
    switch (currentToken.type) {

        case TOK_IF:
            comandoIf();
            break;

        case TOK_FOR:
            comandoFor();
            break;

        case TOK_LEFT_BRACE:
            bloco();
            break;

        case TOK_ID:
            atribuicao();
            break;

        default:
            syntaxError("comando invalido");
    }
}

// atribuicao ::= id = expressao ;
void atribuicao() {

    char nome[MAX_LEXEMA];
    strcpy(nome, currentToken.lexeme);

    match(TOK_ID);

    semAssign(nome);

    match(TOK_ASSIGN);

    expressao();

    match(TOK_SEMICOLON);
}

// if
void comandoIf() {

    match(TOK_IF);
    match(TOK_LEFT_PAREN);

    expressao();
    semCheckCondition();

    match(TOK_RIGHT_PAREN);

    bloco();

    if (currentToken.type == TOK_ELSE) {
        match(TOK_ELSE);
        bloco();
    }
}

// for
void comandoFor() {

    match(TOK_FOR);
    match(TOK_LEFT_PAREN);

    atribuicao();

    expressao();
    semCheckCondition();

    match(TOK_SEMICOLON);

    char nome[MAX_LEXEMA];
    strcpy(nome, currentToken.lexeme);

    match(TOK_ID);
    semAssign(nome);

    match(TOK_ASSIGN);

    expressao();

    semCheckIncrement(nome);

    match(TOK_RIGHT_PAREN);

    bloco();
}

// bloco ::= { declaracao* }
void bloco() {

    match(TOK_LEFT_BRACE);

    semBeginBlock();

    while (
        currentToken.type == TOK_INTEGER ||
        currentToken.type == TOK_REAL ||
        currentToken.type == TOK_CHAR ||
        currentToken.type == TOK_LITERAL ||
        currentToken.type == TOK_BOOL ||
        currentToken.type == TOK_IF ||
        currentToken.type == TOK_FOR ||
        currentToken.type == TOK_LEFT_BRACE ||
        currentToken.type == TOK_ID
    ) {
        declaracao();
    }

    semEndBlock();

    match(TOK_RIGHT_BRACE);
}

// expressao
void expressao() {
    expressaoRelacional();
}

// relacional
void expressaoRelacional() {

    expressaoAritmetica();

    if (
        currentToken.type == TOK_GT ||
        currentToken.type == TOK_GTE ||
        currentToken.type == TOK_LT ||
        currentToken.type == TOK_LTE ||
        currentToken.type == TOK_EQ ||
        currentToken.type == TOK_NE
    ) {
        opRelacional();
        expressaoAritmetica();
    }
}

// aritmetica
void expressaoAritmetica() {

    termo();

    while (
        currentToken.type == TOK_PLUS ||
        currentToken.type == TOK_MINUS
    ) {
        if (currentToken.type == TOK_PLUS)
            match(TOK_PLUS);
        else
            match(TOK_MINUS);

        termo();
    }
}

// termo
void termo() {

    fator();

    while (
        currentToken.type == TOK_MULT ||
        currentToken.type == TOK_DIV
    ) {
        if (currentToken.type == TOK_MULT)
            match(TOK_MULT);
        else
            match(TOK_DIV);

        fator();
    }
}

// fator
void fator() {

    if (currentToken.type == TOK_ID) {

        semUseVariable(currentToken.lexeme);
        match(TOK_ID);

    } else if (
        currentToken.type == TOK_INT_LITERAL ||
        currentToken.type == TOK_REAL_LITERAL ||
        currentToken.type == TOK_CHAR_LITERAL ||
        currentToken.type == TOK_STRING_LITERAL ||
        currentToken.type == TOK_TRUE ||
        currentToken.type == TOK_FALSE
    ) {
        literal();

    } else if (currentToken.type == TOK_LEFT_PAREN) {

        match(TOK_LEFT_PAREN);
        expressao();
        match(TOK_RIGHT_PAREN);

    } else {
        syntaxError("fator invalido");
    }
}

// operadores relacionais
void opRelacional() {

    switch (currentToken.type) {

        case TOK_GT:  match(TOK_GT); break;
        case TOK_GTE: match(TOK_GTE); break;
        case TOK_LT:  match(TOK_LT); break;
        case TOK_LTE: match(TOK_LTE); break;
        case TOK_EQ:  match(TOK_EQ); break;
        case TOK_NE:  match(TOK_NE); break;

        default:
            syntaxError("operador relacional invalido");
    }
}

// literais
void literal() {

    switch (currentToken.type) {

        case TOK_INT_LITERAL: match(TOK_INT_LITERAL); break;
        case TOK_REAL_LITERAL: match(TOK_REAL_LITERAL); break;
        case TOK_CHAR_LITERAL: match(TOK_CHAR_LITERAL); break;
        case TOK_STRING_LITERAL: match(TOK_STRING_LITERAL); break;
        case TOK_TRUE: match(TOK_TRUE); break;
        case TOK_FALSE: match(TOK_FALSE); break;

        default:
            syntaxError("literal invalido");
    }
}