#include "../include/parse.h"
#include "../include/env.h"
#include "../include/eval.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printAST(struct ASTValue *ast) {
    switch (ast->type) {
        case ASTTYPE_NUMBER:
            printf("%f", ast->number);
            break;
        case ASTTYPE_SYMBOL:
            printf("%s", ast->str);
            break;
        case ASTTYPE_STR:
            printf("\"%s\"", ast->str);
            break;
        case ASTTYPE_LIST:
            printf("(");
            for (size_t i = 0; i < ast->list.count; i++) {
                printAST(&ast->list.items[i]);
                if (i != ast->list.count - 1) {
                    printf(" ");
                }
            }
            printf(")");
            break;
        case ASTTYPE_DOT_PAIR:
            printAST(ast->pair.car);
            printf(" . ");
            printAST(ast->pair.cdr);
            break;
        case ASTTYPE_NIL:
            printf("()");
            break;
    }
}

struct Value *eval(struct ASTValue *ast, struct Env *env) {
	switch (ast->type) {
	case ASTTYPE_NUMBER: {
		struct Value *val = (struct Value *)malloc(sizeof(struct Value));
		val->type = TYPE_NUMBER;
		val->number = ast->number;
		return val;
	}
	case ASTTYPE_SYMBOL: {
		return env_get(env, ast->str);
	}
	case ASTTYPE_LIST: {
		if (ast->list.count == 0) {
			struct Value *val = (struct Value *)malloc(sizeof(struct Value));
			val->type = TYPE_SYMBOL;
			val->symbol = strdup("nil");
			return val;
		}
		struct ASTValue *first = &ast->list.items[0];
		if (first->type == ASTTYPE_SYMBOL) {
			if (strcmp(first->str, "define") == 0)
				return eval_define(ast, env);
			if (strcmp(first->str, "if") == 0)
				return eval_if(ast, env);
		}
		struct Value *fn = eval(first, env);
		if (fn->type != TYPE_FUNCTION && fn->type != TYPE_SPECIAL) {
			fprintf(stderr, "First element is not a function\n");
			exit(EXIT_FAILURE);
		}

		struct Value *args = NULL, **arg_tail = &args;
		for (size_t i = 1; i < ast->list.count; i++) {
			struct Value *arg = eval(&ast->list.items[i], env);
			struct Value *pair = malloc(sizeof(struct Value));
			pair->type = TYPE_PAIR;
			pair->car = arg;
			pair->cdr = NULL;
			*arg_tail = pair;
			arg_tail = &pair->cdr;
		}
		if(fn->type == TYPE_FUNCTION)
			return fn->builtin(args, env);
		else
			return fn->special(args, env);
	}
	case ASTTYPE_DOT_PAIR:
	case ASTTYPE_STR:
	case ASTTYPE_NIL:
	default:
		fprintf(stderr, "Unknown node found in evaluation\n");
		exit(EXIT_FAILURE);
	}
}

struct Value *eval_define(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 3) {
		fprintf(stderr, "Invalid define form\n");
		exit(1);
	}
	struct ASTValue *sym = &ast->list.items[1];
	struct ASTValue *expr = &ast->list.items[2];

	if (sym->type != ASTTYPE_SYMBOL) {
		fprintf(stderr, "define: expected symbol\n");
		exit(1);
	}

	struct Value *value = eval(expr, env);
	env_set(env, sym->str, value);

	return value;
}

struct Value *eval_if(struct ASTValue *ast, struct Env *env) {
    if (ast->list.count != 4) {
        fprintf(stderr, "Invalid if form\n");
        exit(1);
    }

    struct Value *cond = eval(&ast->list.items[1], env);
    if (cond->type != TYPE_NUMBER && cond->type != TYPE_BOOLEAN) {
        fprintf(stderr, "if: condition must be number\n");
        exit(1);
    }

    if (cond->number != 0.0 || cond->_bool == true)
        return eval(&ast->list.items[2], env);
    else
        return eval(&ast->list.items[3], env);
}
