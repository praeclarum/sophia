#include <stdio.h>
#include "ast.h"

static void swift_print(struct AST *ast, FILE *outfile, int indent_level);

static void swift_print_decl(struct AST *ast, FILE *outfile, int indent_level) {
    if (!ast) {
        return;
    }
    for (int i = 0; i < indent_level; i++) {
        fprintf(outfile, "    ");
    }
    switch (ast->type) {
    case AST_CLASS_DECL:
        fprintf(outfile, "class %s {\n", ast->data.class_decl.name);
        swift_print(ast->data.class_decl.members, outfile, indent_level + 1);
        fprintf(outfile, "}\n");
        break;

    case AST_VAR_DECL:
        fprintf(outfile, "let %s = ", ast->data.var_decl.name);
        swift_print(ast->data.var_decl.value, outfile, indent_level);
        fprintf(outfile, "\n");
        break;

    default:
        fprintf(outfile, "/*Unknown decl type: %d*/0", ast->type);
        break;
    }
}

static void swift_print_expr(struct AST *ast, FILE *outfile) {
    if (!ast) {
        return;
    }
    switch (ast->type) {
    case AST_UNRESOLVED_TYPE_REF:
        fprintf(outfile, "%s", ast->data.unresolved_type_ref.name);
        break;

    case AST_NUM_EXPR:
        fprintf(outfile, "%d", ast->data.num_expr.value);
        break;

    default:
        fprintf(outfile, "/*Unknown expr type: %d*/0", ast->type);
        break;
    }
}

static void swift_print(struct AST *ast, FILE *outfile, int indent_level) {
    if (!ast) {
        return;
    }
    switch (ast->type) {
    case AST_CLASS_DECL:
    case AST_VAR_DECL:
        while (ast) {
            swift_print_decl(ast, outfile, indent_level);
            ast = ast->next;
        }
        break;

    case AST_UNRESOLVED_TYPE_REF:
    case AST_NUM_EXPR:
        swift_print_expr(ast, outfile);
        if (ast->next) {
            fprintf(outfile, "/*Unexpected next expr node*/");
        }
        break;

    default:
        fprintf(outfile, "/*Unknown expr type: %d*/0", ast->type);
        break;
    }
}

int translate_swift(struct AST *ast, FILE *outfile) {
    if (!ast) {
        return 0;
    }
    swift_print(ast, outfile, 0);
    return 0;
}
