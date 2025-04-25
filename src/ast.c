#include <stdlib.h>
#include <stdio.h>

#include "ast.h"

struct AST *ast_new(enum ASTType type) {
    struct AST *ast = calloc(1, sizeof(struct AST));
    if (!ast) {
        fprintf(stderr, "Error: Could not allocate memory for AST\n");
        return NULL;
    }
    ast->type = type;
    return ast;
}

void ast_free(struct AST *ast) {
    if (!ast) {
        return;
    }
    free(ast);
}

