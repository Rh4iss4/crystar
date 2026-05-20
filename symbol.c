#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol.h"

// inicio da tabela de simbolos
static Symbol *table = NULL;
// escopo atual
static int currentScope = 0;

// entra em um novo escopo
void enterScope() {
    currentScope++;
}

// sai do escopo atual removendo simbolos
void exitScope() {
    Symbol *s = table;

    while (s && s->scope == currentScope) {
        Symbol *tmp = s;
        s = s->next;
        free(tmp);
    }

    table = s;
    currentScope--;
}

// busca simbolo pelo nome (todos os escopos)
Symbol* findSymbol(const char *name) {
    Symbol *s = table;
    while (s) {
        if (strcmp(s->name, name) == 0) {
            return s;
        }
        s = s->next;
    }
    return NULL;
}

// adiciona simbolo na tabela
void addSymbol(const char *name, VarType type) {

    // verifica redeclaracao no mesmo escopo
    Symbol *s = table;
    while (s && s->scope == currentScope) {
        if (strcmp(s->name, name) == 0) {
            printf("ERRO SEMANTICO: variavel '%s' redeclarada\n", name);
            exit(1);
        }
        s = s->next;
    }

    Symbol *novo = (Symbol *) malloc(sizeof(Symbol));
    if (!novo) {
        fprintf(stderr, "Erro de memoria\n");
        exit(1);
    }

    strcpy(novo->name, name);
    novo->type = type;
    novo->scope = currentScope;
    novo->next = table;

    table = novo;
}