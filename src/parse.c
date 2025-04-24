#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../include/parse.h"

void parseError(const char *error_msg) {
	fprintf(stderr, "%s\n", error_msg);
	exit(64);
}

void parseExpect(struct LispTokens *tokens, size_t *pos, LispTokenType type) {
	if (*pos >= tokens->count)
		parseError("Unexpected token found, but reached end of input");

	struct LispToken token = tokens->lispTokens[*pos];
	if (token.type != type)
		parseError("Expected some other type of input");

	(*pos)++;
}


struct ASTValue parseList(struct LispTokens *tokens, size_t *pos) {
	struct ASTValueArray values = {
		.capacity = 4,
		.values = (struct ASTValue*)malloc(4 * sizeof(struct ASTValue)),
		.count = 0
	};
	while (*pos < tokens->count) {
		struct LispToken token = tokens->lispTokens[*pos];
		if (token.type == TOKEN_TYPE_PAREN_RIGHT) {
			(*pos)++;
			return (struct ASTValue){.type = ASTTYPE_LIST, .list = {.items = values.values, .count = values.count}};
		} else if (token.type == TOKEN_TYPE_SYMBOL && strcmp(token.value, ".") == 0) {
			(*pos)++;
			if (values.count != 1) {
				parseError("Invalid dotted pair, must have exactly one item before dot");
			}
			struct ASTValue *car = &values.values[0];
			struct ASTValue cdr_val = parseExpr(tokens, pos);
			struct ASTValue *cdr = (struct ASTValue *)malloc(sizeof(struct ASTValue));
			*cdr = cdr_val;
			parseExpect(tokens, pos, TOKEN_TYPE_PAREN_RIGHT);
			if (values.count == 1) {
				car = &values.values[0]; 
			} else {
				parseError("Invalid dotted pair");
			}
			if (car == NULL) parseError("Car is invalid");
			return (struct ASTValue){.type = ASTTYPE_DOT_PAIR, .pair = {.car = car, .cdr = cdr}};
		} else {
			if (values.count >= values.capacity) {
				values.capacity *= 2;
				values.values = (struct ASTValue*)realloc(values.values, values.capacity * sizeof(*values.values));
			}
			struct ASTValue expr = parseExpr(tokens, pos);
			values.values[values.count++] = expr;
		}
	}
	parseError("Unclosed '('");
	return (struct ASTValue){.type = ASTTYPE_NIL};
}

struct ASTValue parseExpr(struct LispTokens *tokens, size_t *pos) {
	if (*pos >= tokens->count)
		parseError("Unexpected end of input");

	struct LispToken token = tokens->lispTokens[*pos];
	switch (token.type) {
	case TOKEN_TYPE_PAREN_LEFT:
		(*pos)++;
		return parseList(tokens, pos);
	case TOKEN_TYPE_NUMBER:
		(*pos)++;
		char *end= NULL;
		return (struct ASTValue){.type = ASTTYPE_NUMBER, .number = strtod(token.value, &end)};
	case TOKEN_TYPE_SYMBOL:
		(*pos)++;
		return (struct ASTValue){.type = ASTTYPE_SYMBOL, .str = strdup(token.value)};
	case TOKEN_TYPE_STR:
		(*pos)++;
		return (struct ASTValue){.type = ASTTYPE_STR, .str = strdup(token.value)};
	case TOKEN_TYPE_PAREN_RIGHT:
		parseError("Unexpected ')'");
		break;
	case TYPE_INVALID:
	default:
		parseError("Unexpected token found");
		break;
	}

	return (struct ASTValue){.type = ASTTYPE_NIL};
}

struct ASTValue parse(struct LispTokens *tokens) {
	size_t pos = 0;
	return parseExpr(tokens, &pos);
}
