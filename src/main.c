#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <readline/readline.h>

#include "../include/main.h"
#include "../include/token.h"
#include "../include/parse.h"
#include "../include/eval.h"
#include "../include/env.h"

static char *line = (char *)NULL;

void print_par(struct Value *pair);
void print_value(struct Value *val);

void print_pair(struct Value *pair) {
    print_value(pair->car);

    if (pair->cdr == NULL) {
        return;
    } else if (pair->cdr->type == TYPE_PAIR) {
        printf(" ");
        print_pair(pair->cdr);
    } else {
        printf(" . ");
        print_value(pair->cdr);
    }
}

void print_value(struct Value *val) {
    switch (val->type) {
    case TYPE_NUMBER:
        printf("%g\n", val->number);
        break;
    case TYPE_SYMBOL:
        printf("%s\n", val->symbol);
        break;
    case TYPE_FUNCTION:
        printf("<function>\n");
        break;
    case TYPE_PAIR:
        printf("(");
        print_pair(val);
        printf(")\n");
        break;
	case TYPE_BOOLEAN:
		printf("%s\n", val->_bool == true ? "#t" : "#f");
		break;
    default:
        printf("<unknown>\n");
        break;
    }
}


int main(void) {
	struct Env *env = init_global_env();
	while (true) {
		if (line) {
			free(line);
			line = (char *)NULL;
		}
		line = readline("> ");
		if (line && *line) {
			DEBUG_PRINT("output: %s\n", line);
			struct LispTokens tokens = tokenize(line, strlen(line));
			DEBUG_PRINT("token count: %zu\n", tokens.count);
			for (size_t i = 0; i < tokens.count; i++)
				DEBUG_PRINT("token[%zu]: type=%d, value='%s'\n", i, tokens.lispTokens[i].type, tokens.lispTokens[i].value);
			struct ASTValue ast = parse(&tokens);
	 		DEBUG_PRINT(printAST(&ast));
			struct Value *result = eval(&ast, env);
			print_value(result);
		}
	}
	return EXIT_SUCCESS;
}
