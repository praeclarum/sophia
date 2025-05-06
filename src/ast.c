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

    case AST_VAR_DECL:
        free(ast->data.var_decl.name);
        ast_free(ast->data.var_decl.type);
        ast_free(ast->data.var_decl.value);
        break;

    case AST_NUM_EXPR:
        // No dynamic memory to free
        break;
    
    default:
        break;
    }
    free(ast);
}

void ast_print(struct AST *ast, FILE *outfile, int indent_level) {
    if (!ast) {
        return;
    }
    for (int i = 0; i < indent_level; i++) {
        fprintf(outfile, "    ");
    }
    switch (ast->type) {
    case AST_CLASS_DECL:
        fprintf(outfile, "ClassDecl: %s\n", ast->data.class_decl.name);
        ast_print(ast->data.class_decl.base_class, outfile, indent_level + 1);
        ast_print(ast->data.class_decl.members, outfile, indent_level + 1);
        break;

    case AST_UNRESOLVED_TYPE_REF:
        fprintf(outfile, "UnresolvedTypeRef: %s\n", ast->data.unresolved_type_ref.name);
        break;

    case AST_VAR_DECL:
        fprintf(outfile, "VarDecl: %s\n", ast->data.var_decl.name);
        ast_print(ast->data.var_decl.value, outfile, indent_level + 1);
        break;

    case AST_NUM_EXPR:
        fprintf(outfile, "NumExpr: %d\n", ast->data.num_expr.value);
        break;

    default:
        fprintf(outfile, "Unknown AST type\n");
        break;
    }
    ast_print(ast->next, outfile, indent_level);
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

struct AST *ast_new_unresolved_type_ref(const char *name, int first_line) {
    struct AST *ast = ast_new(AST_UNRESOLVED_TYPE_REF);
    (void)first_line; // Unused parameter
    if (!ast) {
        return NULL;
    }
    ast->data.unresolved_type_ref.name = strdup(name);
    if (!ast->data.unresolved_type_ref.name) {
        fprintf(stderr, "Error: Could not allocate memory for unresolved type reference\n");
        ast_free(ast);
        return NULL;
    }
    return ast;
}

struct AST *ast_new_var_decl(const char *name, struct AST *type, struct AST *value, int mutable, int first_line) {
    struct AST *ast = ast_new(AST_VAR_DECL);
    (void)first_line; // Unused parameter
    if (!ast) {
        return NULL;
    }
    ast->data.var_decl.name = strdup(name);
    if (!ast->data.var_decl.name) {
        fprintf(stderr, "Error: Could not allocate memory for variable name\n");
        ast_free(ast);
        return NULL;
    }
    ast->data.var_decl.type = type;
    ast->data.var_decl.value = value;
    ast->data.var_decl.mutable = mutable;
    return ast;
}

struct AST *ast_new_num_expr(int value, int first_line) {
    struct AST *ast = ast_new(AST_NUM_EXPR);
    (void)first_line; // Unused parameter
    if (!ast) {
        return NULL;
    }
    ast->data.num_expr.value = value;
    return ast;
}

void ast_append(struct AST *parent, struct AST *child) {
    if (!parent || !child) {
        return;
    }
    struct AST *last = parent;
    while (last->next) {
        last = last->next;
    }
    last->next = child;
}
