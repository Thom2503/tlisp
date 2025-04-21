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
	case ASTTYPE_DOT_PAIR: {
		struct Value *val = (struct Value *)malloc(sizeof(struct Value));
		val->type = TYPE_PAIR;
		val->car = eval((struct ASTValue *)&ast->pair.car, env);
		val->cdr = eval((struct ASTValue *)&ast->pair.cdr, env);
		return val;
	}
	case ASTTYPE_LIST: {
		if (ast->list.count == 0) {
			struct Value *val = (struct Value *)malloc(sizeof(struct Value));
			val->type = TYPE_NIL;
			val->symbol = strdup("()");
			return val;
		}
		if (ast->type == ASTTYPE_DOT_PAIR) {
			struct Value *val = (struct Value *)malloc(sizeof(struct Value));
			val->type = TYPE_PAIR;
			val->car = eval(ast->pair.car, env);
			val->cdr = eval(ast->pair.cdr, env);
			return val;
		}
		struct ASTValue *first = &ast->list.items[0];
		if (first->type == ASTTYPE_SYMBOL) {
			if (strcmp(first->str, "define") == 0)
				return eval_define(ast, env);
			if (strcmp(first->str, "if") == 0)
				return eval_if(ast, env);
			if (strcmp(first->str, "atom") == 0)
				return eval_atom(ast, env);
			if (strcmp(first->str, "quote") == 0)
				return eval_quote(ast, env);
			if (strcmp(first->str, "eq") == 0)
				return eval_eq(ast, env);
		}
		struct Value *fn = eval(first, env);
		if (fn->type != TYPE_FUNCTION && fn->type != TYPE_SPECIAL && fn->type != TYPE_PAIR) {
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
	case ASTTYPE_STR: {
		struct Value *val = (struct Value *)malloc(sizeof(struct Value));
		val->type = TYPE_STR;
		val->str = ast->str;
		return val;
	}
	case ASTTYPE_NIL: {
		struct Value *val = (struct Value *)malloc(sizeof(struct Value));
		val->type = TYPE_NIL;
		val->symbol = strdup("()");
		return val;
	}
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

struct Value *eval_atom(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 2 && ast->list.count == 0) {
		fprintf(stderr, "Invalid atom form\n");
		exit(1);
	}
	struct Value *atom = eval(&ast->list.items[1], env);
	struct Value *res = (struct Value *)malloc(sizeof(struct Value));
	res->type = TYPE_BOOLEAN;
	res->_bool = atom->type != TYPE_PAIR;
	return res;
}

struct Value *eval_quote(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 2) {
		fprintf(stderr, "Invalid quote form\n");
		exit(1);
	}

	struct ASTValue *quoted_val = &ast->list.items[1];
	return eval_quote_single(quoted_val);
}

struct Value *eval_quote_single(struct ASTValue *quoted_val) {
	struct Value *res = (struct Value *)malloc(sizeof(struct Value));

	switch (quoted_val->type) {
	case ASTTYPE_NUMBER: {
		res->type = TYPE_NUMBER;
		res->number = quoted_val->number;
		break;
	}
	case ASTTYPE_SYMBOL: {
		res->type = TYPE_SYMBOL;
		res->symbol = quoted_val->str;
		break;
	}
	case ASTTYPE_STR: {
		res->type = TYPE_STR;
		res->symbol = quoted_val->str;
		break;
	}
	case ASTTYPE_NIL: {
		res->type = TYPE_NIL;
		res->symbol = strdup("()");
		break;
	}
	case ASTTYPE_DOT_PAIR: {
		res->type = TYPE_PAIR;
		res->car = eval_quote_single(quoted_val->pair.car);
		res->cdr = eval_quote_single(quoted_val->pair.cdr);
		break;
	}
	case ASTTYPE_LIST: {
		struct Value *head = NULL;
		struct Value **tail = &head;

		for (size_t i = 0; i < quoted_val->list.count; i++) {
			struct Value *item = eval_quote_single(&quoted_val->list.items[i]);
			struct Value *pair = (struct Value *)malloc(sizeof(struct Value));

			pair->type = TYPE_PAIR;
			pair->car = item;
			pair->cdr = NULL;
			*tail = pair;
			tail = &pair->cdr;
		}

		if (head == NULL) {
			head = (struct Value *)malloc(sizeof(struct Value));
			head->type = TYPE_NIL;
			head->symbol = strdup("()");
		}

		res = head;
		break;
	}
	default:
		fprintf(stderr, "Not a valid type to evaluate\n");
		exit(1);
	}
	return res;	

}

struct Value *eval_eq(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 3) {
		fprintf(stderr, "Invalid eq form\n");
		exit(1);
	}

	struct Value *lhs = eval(&ast->list.items[1], env);
	struct Value *rhs = eval(&ast->list.items[2], env);
	struct Value *res = (struct Value *)malloc(sizeof(struct Value));
	res->type = TYPE_BOOLEAN;
	res->_bool = false;

	if (lhs->type == TYPE_NUMBER && rhs->type == TYPE_NUMBER)
		res->_bool = lhs->number == rhs->number;
	else if ((lhs->type == TYPE_STR && rhs->type == TYPE_STR) || (lhs->type == TYPE_SYMBOL && rhs->type == TYPE_SYMBOL))
		res->_bool = (strcmp(lhs->str, rhs->str) == 0); 
	else if (lhs->type == TYPE_NIL && rhs->type == TYPE_NIL)
		res->_bool = true;

	return res;
}
