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


int main(int argc, char **argv) {
	struct Env *env = init_global_env();
	if (argc == 2) {
		char *filename = argv[1];
		char *buffer = 0;
		long length;
		FILE *fp = fopen(filename, "r");
		if (fp == NULL) {
			fprintf(stderr, "Could not open file\n");
			return EXIT_FAILURE;
		}

		// XXX: need to check return values
		fseek(fp, 0, SEEK_END);
		length = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		buffer = malloc(length + 1);
		if (buffer == NULL) {
			fprintf(stderr, "Couldn't allocate buffer\n");
			fclose(fp);
			return EXIT_FAILURE;
		} else {
			fread(buffer, 1, length, fp);
		}
		buffer[length] = '\0';
		fclose(fp);
		struct LispTokens tokens = tokenize(buffer, length);
		size_t pos = 0;
		while (pos < tokens.count) {
			struct ASTValue ast = parseExpr(&tokens, &pos);
			struct Value *result = eval(&ast, env);
			print_value(result);
			freeAST(&ast);
			freeValue(result);
		}
		freeTokens(&tokens);
		free(buffer);
		return EXIT_SUCCESS;
	} else {
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
				freeTokens(&tokens);
				freeValue(result);
				freeAST(&ast);
			}
		}
	}
	return EXIT_SUCCESS;
}
