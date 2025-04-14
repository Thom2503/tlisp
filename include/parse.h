#ifndef PARSE_H
#define PARSE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "token.h"

typedef enum {
	ASTTYPE_NUMBER,
	ASTTYPE_STR,
	ASTTYPE_SYMBOL,
	ASTTYPE_LIST,
	ASTTYPE_DOT_PAIR,
	ASTTYPE_NIL,
} ASTType;

struct ASTValue {
	ASTType type;
	union {
		double number;
		char *str;
		struct {
			struct ASTValue *items;
			size_t count;
		} list;
		struct {
			struct ASTValue *car;
			struct ASTValue *cdr;
		} pair;
	};
};

struct ASTValueArray {
	struct ASTValue *values;
	size_t count;
	size_t capacity;
};

void parseError(const char *error_msg);
void parseExpect(struct LispTokens *tokens, size_t *pos, LispTokenType type);
struct ASTValue parseList(struct LispTokens *tokens, size_t *pos);
struct ASTValue parseExpr(struct LispTokens *tokens, size_t *pos);
struct ASTValue parse(struct LispTokens *tokens);

#endif
