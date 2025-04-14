#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include "../include/main.h"
#include "../include/token.h"

struct LispToken charToLispToken(const char *input, size_t len, size_t *i) {
	// skip the whitespaces
	while (*i < len && isspace(input[*i])) (*i)++;

	if (*i >= len)
		return (struct LispToken){.type = TYPE_INVALID, .value = NULL};

	char chr = input[*i];
	if (chr == '(') {
		(*i)++;
		char *val = (char *)malloc(2); val[0] = chr; val[1] = '\0';
		return (struct LispToken){.type = TOKEN_TYPE_PAREN_LEFT, .value = val};
	}
	if (chr == ')') {
		(*i)++;
		char *val = (char *)malloc(2); val[0] = chr; val[1] = '\0';
		return (struct LispToken){.type = TOKEN_TYPE_PAREN_RIGHT, .value = val};
	}

	if (chr == '"') {
		(*i)++;
		size_t start = *i;
		while (*i < len && input[*i] != '"') (*i)++;

		size_t length = *i - start;
		char *str = (char *)malloc(length + 1);
		memcpy(str, &input[start], length);
		str[length] = '\0';
		if (*i < len && input[*i] == '"') (*i)++;
		return (struct LispToken){.type = TOKEN_TYPE_STR, .value = str};
	}

	if (isdigit(chr) || (chr == '-' && isdigit(input[*i + 1]))) {
		size_t start = *i;
		// for negative sign go to the next chars
		if (input[*i] == '-') (*i)++;

		while (*i < len && (isdigit(input[*i]) || input[*i] == '.')) (*i)++;
		size_t length = *i - start;
		char *number = (char *)malloc(length + 1);
		memcpy(number, &input[start], length);
		number[length] = '\0';
		return (struct LispToken){.type = TOKEN_TYPE_NUMBER, .value = number};
	}

	if (isalpha(chr) || strchr("+-*/<>=!?_&%$-#", chr)) {
		size_t start = *i;
		// skip over whitespaces and the parens
		while (*i < len && !isspace(input[*i]) && input[*i] != '(' && input[*i] != ')') (*i)++;

		size_t length = *i - start;
		char *symbol = (char *)malloc(length + 1);
		memcpy(symbol, &input[start], length);
		symbol[length] = '\0';
		return (struct LispToken){.type = TOKEN_TYPE_SYMBOL, .value = symbol};
	}

	(*i)++;
	char *val = (char *)malloc(2); val[0] = chr; val[1] = '\0';
	return (struct LispToken){.type = TYPE_INVALID, .value = val};
}

struct LispTokens tokenize(const char *input, size_t len) {
	struct LispTokens tokens = {0};
	tokens.capacity = 8;
	tokens.lispTokens = (struct LispToken*)malloc(tokens.capacity * sizeof(struct LispToken));
	size_t i = 0;
	while (i < len) {
		if (tokens.count >= tokens.capacity) {
			tokens.capacity *= 2;
			tokens.lispTokens = (struct LispToken*)realloc(tokens.lispTokens, tokens.capacity * sizeof(*tokens.lispTokens));
		}

		struct LispToken token = charToLispToken(input, len, &i);
		DEBUG_PRINT("token: %s\n", token.value);
		if (token.type != TYPE_INVALID) {
			tokens.lispTokens[tokens.count++] = token;
		}
	}
	return tokens;
}

void freeTokens(struct LispTokens *tokens) {
	for (size_t i = 0; i < tokens->count; ++i) {
		free(tokens->lispTokens[i].value);
	}
	free(tokens->lispTokens);
	tokens->lispTokens = NULL;
	tokens->count = tokens->capacity = 0;
}
