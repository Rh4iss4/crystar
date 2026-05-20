#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "token.h"

// controle do programa
void semBeginProgram();
void semEndProgram();

// declaracao e uso de variaveis
void semDeclareVariable(const char *name, TokenType type);
void semAssign(const char *name);
void semUseVariable(const char *name);

// estruturas de controle
void semCheckCondition();
void semCheckIncrement(const char *name);

// controle de escopo
void semBeginBlock();
void semEndBlock();

#endif
