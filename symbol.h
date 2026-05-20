#ifndef SYMBOL_H
#define SYMBOL_H

#include "token.h"

// tipos de variaveis suportados
typedef enum {
    TYPE_INTEGER,
    TYPE_REAL,
    TYPE_CHAR,
    TYPE_LITERAL,
    TYPE_BOOL
} VarType;

// estrutura da tabela de simbolos
typedef struct Symbol {
    char name[MAX_LEXEMA];   // nome da variavel
    VarType type;            // tipo da variavel
    int scope;               // nivel de escopo
    struct Symbol *next;     // proximo simbolo
} Symbol;

// controle de escopo
void enterScope();
void exitScope();

// operacoes da tabela de simbolos
void addSymbol(const char *name, VarType type);
Symbol* findSymbol(const char *name);

#endif
