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
struct Value *eval_quote_single(struct ASTValue *ast);
struct Value *eval_eq(struct ASTValue *ast, struct Env *env);
struct Value *eval_cdrcar(struct ASTValue *ast, struct Env *env, bool isCar);
struct Value *eval_cons(struct ASTValue *ast, struct Env *env);
struct Value *eval_cond(struct ASTValue *ast, struct Env *env);
struct Value *eval_lambda(struct ASTValue *ast, struct Env *env);
struct Value *eval_label(struct ASTValue *ast, struct Env *env);
struct Value *call_lambda(struct Value *closure, struct Value *args, struct Env *env);
void freeValue(struct Value *val);

#endif
