#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
    switch (ast->type) {
    case AST_CLASS_DECL:
        free(ast->data.class_decl.name);
        ast_free(ast->data.class_decl.base_class);
        ast_free(ast->data.class_decl.members);
        break;
    
    case AST_UNRESOLVED_TYPE_REF:
        free(ast->data.unresolved_type_ref.name);
        break;
    
    default:
        break;
    }
    free(ast);
}

struct AST *ast_new_class_decl(const char *name, struct AST *base_class, struct AST *members, int first_line) {
    struct AST *ast = ast_new(AST_CLASS_DECL);
    (void)first_line; // Unused parameter
    if (!ast) {
        return NULL;
    }
    ast->data.class_decl.name = strdup(name);
    if (!ast->data.class_decl.name) {
        fprintf(stderr, "Error: Could not allocate memory for class name\n");
        ast_free(ast);
        return NULL;
    }
    ast->data.class_decl.base_class = base_class;
    ast->data.class_decl.members = members;
    return ast;
}