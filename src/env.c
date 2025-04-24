#include "../include/env.h"
#include <math.h>

struct Value *make_builtin_function(struct Value *(*func)(struct Value *, struct Env *)) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_FUNCTION;
	val->builtin = func;
	return val;
}

struct Value *make_builtin_boolean(bool _bool) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_BOOLEAN;
	val->_bool = _bool;
	return val;
}

int list_length(struct Value *list) {
	int len = 0;
	while (list != NULL) {
		len++;
		list = list->cdr;
	}
	return len;
}

struct Value *builtin_add(struct Value *args, struct Env *env) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	double sum = 0;
	while (args != NULL) {
		struct Value *val = eval((struct ASTValue *)args->car, env);
		if (val->type != TYPE_NUMBER) {
			val->number = 0.0;
			return val;
		}
		sum += val->number;
		args = args->cdr;
	}
	val->number = sum;
	return val;
}

struct Value *builtin_prod(struct Value *args, struct Env *env) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	double res = 1;
	while (args != NULL) {
		struct Value *val = eval((struct ASTValue *)args->car, env);
		if (val->type != TYPE_NUMBER) {
			val->number = 1;
			return val;
		}
		res *= val->number;
		args = args->cdr;
	}
	val->number = res;
	return val;
}

struct Value *builtin_sub(struct Value *args, struct Env *env) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;

	if (args == NULL) {
		val->number = 0.0;
		return val;
	}

	struct Value *first = eval((struct ASTValue *)args->car, env);
	if (first->type != TYPE_NUMBER) {
		val->number = 0.0;
		return val;
	}

	double result = first->number;
	args = args->cdr;

	if (args == NULL) {
		val->number = -result;
		return val;
	}

	while (args != NULL) {
		struct Value *next = eval((struct ASTValue *)args->car, env);
		if (next->type != TYPE_NUMBER) {
			val->number = 0.0;
			return val;
		}
		result -= next->number;
		args = args->cdr;
	}

	val->number = result;
	return val;
}

struct Value *builtin_div(struct Value *args, struct Env *env) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;

	if (args == NULL) {
		val->number = 1.0;
		return val;
	}

	struct Value *first = eval((struct ASTValue *)args->car, env);
	if (first->type != TYPE_NUMBER) {
		val->number = 0.0;
		return val;
	}

	double result = first->number;
	args = args->cdr;

	if (args == NULL) {
		if (result == 0) {
			fprintf(stderr, "Error: division by zero\n");
			exit(1);
		}
		val->number = 1.0 / result;
		return val;
	}

	while (args != NULL) {
		struct Value *next = eval((struct ASTValue *)args->car, env);
		if (next->type != TYPE_NUMBER) {
			val->number = 0.0;
			return val;
		}
		if (next->number == 0) {
			fprintf(stderr, "Error: division by zero\n");
			exit(1);
		}
		result /= next->number;
		args = args->cdr;
	}

	val->number = result;
	return val;
}

struct Value *builtin_max(struct Value *args, struct Env *env) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;

	if (args == NULL) {
		val->number = 0.0;
		return val;
	}

	struct Value *first = eval((struct ASTValue *)args->car, env);
	if (first->type != TYPE_NUMBER) {
		val->number = 0.0;
		return val;
	}

	double result = first->number;
	args = args->cdr;

	if (args == NULL) {
		val->number = result;
		return val;
	}

	while (args != NULL) {
		struct Value *next = eval((struct ASTValue *)args->car, env);
		if (next->type != TYPE_NUMBER) {
			fprintf(stderr, "Error: sequence should be all numbers\n");
			exit(1);
		}
		if (next->number > result)
			result = next->number;
		args = args->cdr;
	}
	val->number = result;
	return val;
}

struct Value *builtin_min(struct Value *args, struct Env *env) {
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;

	if (args == NULL) {
		val->number = 0.0;
		return val;
	}

	struct Value *first = eval((struct ASTValue *)args->car, env);
	if (first->type != TYPE_NUMBER) {
		val->number = 0.0;
		return val;
	}

	double result = first->number;
	args = args->cdr;

	if (args == NULL) {
		val->number = result;
		return val;
	}

	while (args != NULL) {
		struct Value *next = eval((struct ASTValue *)args->car, env);
		if (next->type != TYPE_NUMBER) {
			fprintf(stderr, "Error: sequence should be all numbers\n");
			exit(1);
		}
		if (next->number < result)
			result = next->number;
		args = args->cdr;
	}
	val->number = result;
	return val;
}

struct Value *builtin_abs(struct Value *args, struct Env *env) {
	if (list_length(args) != 1) {
		fprintf(stderr, "Error: argument count must be one\n");
		exit(1);
	}

	struct Value *arg = eval((struct ASTValue *)args->car, env);
	if (arg->type != TYPE_NUMBER) {
		fprintf(stderr, "Error: argument must be an number\n");
		exit(1);
	}
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	val->number = fabs(arg->number);
	return val;
}

struct Value *builtin_sqrt(struct Value *args, struct Env *env) {
	if (list_length(args) != 1) {
		fprintf(stderr, "Error: argument count must be one\n");
		exit(1);
	}

	struct Value *arg = eval((struct ASTValue *)args->car, env);
	if (arg->type != TYPE_NUMBER) {
		fprintf(stderr, "Error: argument must be an number\n");
		exit(1);
	}
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	val->number = sqrt(arg->number);
	return val;
}

struct Value *builtin_floor(struct Value *args, struct Env *env) {
	if (list_length(args) != 1) {
		fprintf(stderr, "Error: argument count must be one\n");
		exit(1);
	}

	struct Value *arg = eval((struct ASTValue *)args->car, env);
	if (arg->type != TYPE_NUMBER) {
		fprintf(stderr, "Error: argument must be an number\n");
		exit(1);
	}
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	val->number = floor(arg->number);
	return val;
}

struct Value *builtin_ceil(struct Value *args, struct Env *env) {
	if (list_length(args) != 1) {
		fprintf(stderr, "Error: argument count must be one\n");
		exit(1);
	}

	struct Value *arg = eval((struct ASTValue *)args->car, env);
	if (arg->type != TYPE_NUMBER) {
		fprintf(stderr, "Error: argument must be an number\n");
		exit(1);
	}
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	val->number = ceil(arg->number);
	return val;
}

struct Value *builtin_mod(struct Value *args, struct Env *env) {
	if (list_length(args) != 2) {
		fprintf(stderr, "Error: argument count must be two\n");
		exit(1);
	}

	struct Value *arg = eval((struct ASTValue *)args->car, env);
	if (arg->type != TYPE_NUMBER) {
		fprintf(stderr, "Error: argument must be an number\n");
		exit(1);
	}

	struct Value *next = eval((struct ASTValue *)args->cdr->car, env);
	if (next->type != TYPE_NUMBER) {
		fprintf(stderr, "Error: argument must be an number\n");
		exit(1);
	}
	struct Value *val = (struct Value *)malloc(sizeof(struct Value));
	val->type = TYPE_NUMBER;
	val->number = fmod(arg->number, next->number);
	return val;
}

void env_set(struct Env *env, const char *name, struct Value *value) {
	struct Entry *entry = (struct Entry *)malloc(sizeof(struct Entry));
	entry->key = strdup(name);
	entry->value = (void *)value;
	table_set(env->bindings, entry->key, entry->value);
}

struct Value *env_get(struct Env *env, const char *name) {
	void *value;
	struct Env *current = env;
	while (current) {
		value = table_get(env->bindings, name);
		if (value) return (struct Value *)value;
		current = current->parent;
	}
	fprintf(stderr, "Unbound variable: %s\n", name);
	exit(EXIT_FAILURE);
}

struct Env *init_global_env(void) {
	struct Env *env = (struct Env *)malloc(sizeof(struct Env));
	if (env == NULL)
		return NULL;
	env->parent = NULL;
	env->bindings = table_create();

	env_set(env, "+", make_builtin_function(builtin_add));
	env_set(env, "*", make_builtin_function(builtin_prod));
	env_set(env, "-", make_builtin_function(builtin_sub));
	env_set(env, "/", make_builtin_function(builtin_div));
	env_set(env, "max", make_builtin_function(builtin_max));
	env_set(env, "min", make_builtin_function(builtin_min));
	env_set(env, "abs", make_builtin_function(builtin_abs));
	env_set(env, "sqrt", make_builtin_function(builtin_sqrt));
	env_set(env, "floor", make_builtin_function(builtin_floor));
	env_set(env, "ceil", make_builtin_function(builtin_ceil));
	env_set(env, "mod", make_builtin_function(builtin_mod));
	env_set(env, "%", make_builtin_function(builtin_mod));
	env_set(env, "#t", make_builtin_boolean(true));
	env_set(env, "#f", make_builtin_boolean(false));
	return env;
}

struct Env *create_child_env(struct Env *p) {
	struct Env *child_env = (struct Env *)malloc(sizeof(struct Env));
	if (child_env == NULL)
		return NULL;
	child_env->parent = p;
	child_env->bindings = table_create();
	return child_env;
}
