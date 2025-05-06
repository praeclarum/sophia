#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ast.h"

struct AST *ast_new(enum ASTType type, int first_line) {
    struct AST *ast = calloc(1, sizeof(struct AST));
    if (!ast) {
        fprintf(stderr, "Error: Could not allocate memory for AST\n");
        return NULL;
    }
    ast->type = type;
    (void)first_line; // Unused parameter
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
    case AST_TYPE_REF:
        free(ast->data.type_ref.name);
        break;
    case AST_VAR_DECL:
        free(ast->data.var_decl.name);
        ast_free(ast->data.var_decl.type);
        ast_free(ast->data.var_decl.value);
        break;
    case AST_VAR_REF:
        free(ast->data.var_ref.name);
        break;
    case AST_FUNC_DECL:
        free(ast->data.func_decl.name);
        ast_free(ast->data.func_decl.parameters);
        ast_free(ast->data.func_decl.type);
        ast_free(ast->data.func_decl.value);
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
    case AST_TYPE_REF:
        fprintf(outfile, "TypeRef: %s\n", ast->data.type_ref.name);
        break;
    case AST_VAR_DECL:
        fprintf(outfile, "VarDecl: %s\n", ast->data.var_decl.name);
        ast_print(ast->data.var_decl.value, outfile, indent_level + 1);
        break;
    case AST_VAR_REF:
        fprintf(outfile, "VarRef: %s\n", ast->data.type_ref.name);
        break;
    case AST_FUNC_DECL:
        fprintf(outfile, "FuncDecl: %s\n", ast->data.func_decl.name);
        ast_print(ast->data.func_decl.parameters, outfile, indent_level + 1);
        ast_print(ast->data.func_decl.type, outfile, indent_level + 1);
        ast_print(ast->data.func_decl.value, outfile, indent_level + 1);
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
    struct AST *ast = ast_new(AST_CLASS_DECL, first_line);
    (void)first_line; // Unused parameter
    ast->data.class_decl.name = strdup(name);
    ast->data.class_decl.base_class = base_class;
    ast->data.class_decl.members = members;
    return ast;
}

struct AST *ast_new_type_ref(const char *name, int first_line) {
    struct AST *ast = ast_new(AST_TYPE_REF, first_line);
    (void)first_line; // Unused parameter
    ast->data.type_ref.name = strdup(name);
    return ast;
}

struct AST *ast_new_var_decl(const char *name, struct AST *type, struct AST *value, int mutable, int first_line) {
    struct AST *ast = ast_new(AST_VAR_DECL, first_line);
    ast->data.var_decl.name = strdup(name);
    ast->data.var_decl.type = type;
    ast->data.var_decl.value = value;
    ast->data.var_decl.mutable = mutable;
    return ast;
}

struct AST *ast_new_var_ref(const char *name, int first_line) {
    struct AST *ast = ast_new(AST_VAR_REF, first_line);
    ast->data.var_ref.name = strdup(name);
    return ast;
}

struct AST *ast_new_func_decl(const char *name, struct AST *parameters, struct AST *type, struct AST *value, int first_line) {
    struct AST *ast = ast_new(AST_FUNC_DECL, first_line);
    ast->data.func_decl.name = strdup(name);
    ast->data.func_decl.parameters = parameters;
    ast->data.func_decl.type = type;
    ast->data.func_decl.value = value;
    return ast;
}

struct AST *ast_new_num_expr(int value, int first_line) {
    struct AST *ast = ast_new(AST_NUM_EXPR, first_line);
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
