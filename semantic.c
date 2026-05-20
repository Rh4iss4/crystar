#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"
#include "symbol.h"

// controla se houve erro semantico
static int semanticError = 0;
// controla se o banner ja foi exibido
static int semanticBannerPrinted = 0;

// imprime o banner apenas uma vez
static void printSemanticBannerOnce() {
    if (!semanticBannerPrinted) {
        printf("\n===== ANALISE SEMANTICA =====\n");
        semanticBannerPrinted = 1;
    }
}

// impressao final
static void printSemanticResult() {
    if (semanticError) return;
    printSemanticBannerOnce();
    printf("Analise semantica concluida com sucesso!\n");
}

void semBeginProgram() {
    atexit(printSemanticResult);
}

void semEndProgram() {
    // nada a fazer
}

// controle de escopo
void semBeginBlock() {
    enterScope();
}

void semEndBlock() {
    exitScope();
}

// declaracao de variavel
void semDeclareVariable(const char *name, TokenType type) {
    if (findSymbol(name)) {
        semanticError = 1;
        printSemanticBannerOnce();
        printf("ERRO SEMANTICO\n");
        printf("Variavel '%s' redeclarada\n", name);
        exit(1);
    }
    addSymbol(name, type);
}

// atribuicao
void semAssign(const char *name) {
    if (!findSymbol(name)) {
        semanticError = 1;
        printSemanticBannerOnce();
        printf("ERRO SEMANTICO\n");
        printf("Variavel '%s' nao declarada\n", name);
        exit(1);
    }
}

// uso em expressao
void semUseVariable(const char *name) {
    if (!findSymbol(name)) {
        semanticError = 1;
        printSemanticBannerOnce();
        printf("ERRO SEMANTICO\n");
        printf("Variavel '%s' nao declarada\n", name);
        exit(1);
    }
}

// condicoes (if / for)
void semCheckCondition() {
    // futura validacao de tipo bool
}

// incremento do for
void semCheckIncrement(const char *name) {
    if (!findSymbol(name)) {
        semanticError = 1;
        printSemanticBannerOnce();
        printf("ERRO SEMANTICO\n");
        printf("Variavel '%s' nao declarada\n", name);
        exit(1);
    }
}