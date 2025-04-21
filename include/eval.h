#ifndef EVAL_H
#define EVAL_H

#include "../include/parse.h"

struct Env;

void printAST(struct ASTValue *ast);
struct Value *eval_list(struct ASTValueArray *list, size_t start, struct Env *env);
struct Value *eval(struct ASTValue *ast, struct Env *env);
struct Value *eval_define(struct ASTValue *ast, struct Env *env);
struct Value *eval_if(struct ASTValue *ast, struct Env *env);
struct Value *eval_atom(struct ASTValue *ast, struct Env *env);
struct Value *eval_quote(struct ASTValue *ast, struct Env *env);

#endif
