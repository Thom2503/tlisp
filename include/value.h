#ifndef VALUE_H
#define VALUE_H

struct Env;
struct ASTValue;

typedef enum {
	TYPE_NUMBER,
	TYPE_SYMBOL,
	TYPE_PAIR,
	TYPE_FUNCTION,
	TYPE_BUILTIN,
	TYPE_SPECIAL,
	TYPE_BOOLEAN,
	TYPE_STR,
	TYPE_NIL,
} ValueType;

struct Value {
	ValueType type;
	union {
		double number;
		char *symbol;
		char *str;
		bool _bool;
		struct {
			struct Value *car;
			struct Value *cdr;
		}; // pair, eg list
		struct Value *(*builtin)(struct Value *args, struct Env *);
		struct Value *(*special)(struct Value *args, struct Env *);
		struct {
			struct ASTValue *params;
			struct ASTValue *body;
			struct Env *closure_env;
		};
	};
};

#endif