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
			if (strcmp(first->str, "car") == 0)
				return eval_cdrcar(ast, env, true);
			if (strcmp(first->str, "cdr") == 0)
				return eval_cdrcar(ast, env, false);
			if (strcmp(first->str, "cons") == 0)
				return eval_cons(ast, env);
			if (strcmp(first->str, "cond") == 0)
				return eval_cond(ast, env);
			if (strcmp(first->str, "lambda") == 0)
				return eval_lambda(ast, env);
			if (strcmp(first->str, "label") == 0)
				return eval_label(ast, env);
		}
		struct Value *fn = eval(first, env);
		if (fn->type != TYPE_FUNCTION && fn->type != TYPE_SPECIAL && fn->type != TYPE_PAIR && fn->type != TYPE_BUILTIN) {
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
		if(fn->type == TYPE_BUILTIN)
			return fn->builtin(args, env);
		else if (fn->type == TYPE_FUNCTION)
			return call_lambda(fn, args, env);
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
	(void)env;
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

struct Value *eval_cdrcar(struct ASTValue *ast, struct Env *env, bool isCar) {
	if (ast->list.count != 2) {
		fprintf(stderr, "Invalid car form\n");
		exit(1);
	}

	struct Value *pair = eval(&ast->list.items[1], env);
	if (pair->type != TYPE_PAIR) {
		fprintf(stderr, "Expected list or pair\n");
		exit(1);
	}
	if (isCar)
		return pair->car;
	else
		return pair->cdr;
}

struct Value *eval_cons(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 3) {
		fprintf(stderr, "Invalid cons form\n");
		exit(1);
	}

	struct Value *first = eval(&ast->list.items[1], env);
	struct Value *second = eval(&ast->list.items[2], env);
	struct Value *res = (struct Value *)malloc(sizeof(struct Value));
	res->type = TYPE_PAIR;
	res->car = first;
	res->cdr = second;
	return res;
}

struct Value *eval_cond(struct ASTValue *ast, struct Env *env) {
	for (size_t i = 1; i < ast->list.count; i++) {
		struct ASTValue item = ast->list.items[i];
		if (item.type != ASTTYPE_LIST || item.list.count != 2) {
			fprintf(stderr, "Invalid cond clause it should have 2 elements\n");
			exit(1);
		}

		struct ASTValue test_val = item.list.items[0];
		struct ASTValue res_val = item.list.items[1];

		if (test_val.type == ASTTYPE_SYMBOL && strcmp(test_val.str, "else") == 0) {
			return eval(&res_val, env);
		}

		struct Value *test_eval = eval(&test_val, env);
		if ((test_eval->type == TYPE_NUMBER && test_eval->number != 0.0) ||
			(test_eval->type == TYPE_BOOLEAN && test_eval->_bool == true)) {
			return eval(&res_val, env);
		}
	}
	fprintf(stderr, "No matching cond form\n");
	exit(1);
}

struct Value *eval_lambda(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 4) {
		struct ASTValue *params = &ast->list.items[1];
		struct ASTValue *body = &ast->list.items[2];

		struct Value *closure = (struct Value *)malloc(sizeof(struct Value));
		closure->type = TYPE_FUNCTION;
		closure->params = params;
		closure->body = body;
		closure->closure_env = env;

		return closure;
	} else {
		fprintf(stderr, "Lambda expects a body and params\n");
		exit(1);
	}
}

struct Value *eval_label(struct ASTValue *ast, struct Env *env) {
	if (ast->list.count != 4) {
		struct ASTValue *name = &ast->list.items[1];
		struct ASTValue *lambda = &ast->list.items[2];
		struct Value *closure = eval(lambda, env);

		struct Env *child = create_child_env(env);
		env_set(child, name->str, closure);
		closure->closure_env = child;
		return closure;
	} else {
		fprintf(stderr, "Label expects name and lambda\n");
		exit(1);
	}
}

struct Value *call_lambda(struct Value *closure, struct Value *args, struct Env *env) {
	(void)env;
	struct Env *child = create_child_env(closure->closure_env);

	for (size_t i = 0; i < closure->params->list.count; i++) {
		struct ASTValue param = closure->params->list.items[i];
		struct Value *arg = args->car;
		env_set(child, param.str, arg);
		arg = arg->cdr;
	}

	return eval(closure->body, child);
}

void freeValue(struct Value *val) {
    if (!val) return;
    switch (val->type) {
        case TYPE_SYMBOL:
            free(val->symbol);
            break;
        case TYPE_PAIR:
            freeValue(val->car);
            freeValue(val->cdr);
            free(val->car);
            free(val->cdr);
            break;
        case TYPE_FUNCTION:
        case TYPE_SPECIAL:
            break;
        default:
            break;
    }
    free(val);
}

