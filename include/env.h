#ifndef ENV_H
#define ENV_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "hash_table.h"
#include "parse.h"
#include "value.h"
#include "eval.h"

struct Env {
	struct Table *bindings;
	struct Env *parent;
};

struct Env *init_global_env(void);
struct Env *create_child_env(struct Env *parent);
void env_set(struct Env *env, const char *key, struct Value *val);
struct Value *env_get(struct Env *env, const char *key);
struct Value *make_builtin_function(struct Value *(*fn)(struct Value *, struct Env *));
struct Value *builtin_add(struct Value *args, struct Env *env);
struct Value *builtin_prod(struct Value *args, struct Env *env);

#endif
