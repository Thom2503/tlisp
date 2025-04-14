#ifndef LispToken_H
#define LispToken_H

#include <stdlib.h>

typedef enum {
	TOKEN_TYPE_PAREN_LEFT,
	TOKEN_TYPE_PAREN_RIGHT,
	TOKEN_TYPE_NUMBER,
	TOKEN_TYPE_STR,
	TOKEN_TYPE_SYMBOL,
	TYPE_INVALID,
} LispTokenType;

struct LispToken {
	char *value;
	LispTokenType type;
};

struct LispTokens {
	struct LispToken *lispTokens;
	size_t count;
	size_t capacity;
};

struct LispToken charToLispToken(const char *input, size_t len, size_t *i);
struct LispTokens tokenize(const char *input, size_t len);
void freeTokens(struct LispTokens *tokens);

#endif