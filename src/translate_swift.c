#include <stdio.h>
#include <string.h>
#include "ast.h"

static void swift_print(struct AST *ast, FILE *outfile, int indent_level);
static void swift_print_type(struct AST *ast, FILE *outfile);

static void swift_print_stmt(struct AST *ast, FILE *outfile, int indent_level) {
    if (!ast) {
        return;
    }
    for (int i = 0; i < indent_level; i++) {
        fprintf(outfile, "    ");
    }
    switch (ast->type) {
    case AST_CLASS_DECL:
        fprintf(outfile, "class %s", ast->data.class_decl.name);
        if (ast->data.class_decl.base_class) {
            fprintf(outfile, " : ");
            swift_print(ast->data.class_decl.base_class, outfile, indent_level);
        }
        fprintf(outfile, " {\n");
        swift_print(ast->data.class_decl.members, outfile, indent_level + 1);
        fprintf(outfile, "}\n");
        break;

    case AST_VAR_DECL:
        if (ast->data.var_decl.mutable) {
            fprintf(outfile, "var ");
        } else {
            fprintf(outfile, "let ");
        }
        fprintf(outfile, "%s ", ast->data.var_decl.name);
        if (ast->data.var_decl.type) {
            fprintf(outfile, ": ");
            swift_print_type(ast->data.var_decl.type, outfile);
        }
        fprintf(outfile, " = ");
        swift_print(ast->data.var_decl.value, outfile, indent_level);
        fprintf(outfile, "\n");
        break;

    case AST_FUNC_DECL:
        fprintf(outfile, "func ");
        fprintf(outfile, "%s(", ast->data.func_decl.name);
        fprintf(outfile, ")");
        if (ast->data.func_decl.type) {
            fprintf(outfile, " -> ");
            swift_print_type(ast->data.func_decl.type, outfile);
        }
        fprintf(outfile, " {\n");
        swift_print(ast->data.func_decl.value, outfile, indent_level + 1);
        fprintf(outfile, "\n}\n");
        break;

    default:
        fprintf(outfile, "/*Unknown statement type: %d*/\n", ast->type);
        break;
    }
}

static void swift_print_expr(struct AST *ast, FILE *outfile) {
    if (!ast) {
        return;
    }
    switch (ast->type) {
    case AST_TYPE_REF:
        fprintf(outfile, "%s", ast->data.type_ref.name);
        break;
    case AST_VAR_REF:
        fprintf(outfile, "%s", ast->data.var_ref.name);
        break;
    case AST_NUM_EXPR:
        fprintf(outfile, "%d", ast->data.num_expr.value);
        break;
    default:
        fprintf(outfile, "/*Unknown expr type: %d*/0", ast->type);
        break;
    }
}

static void swift_print_type(struct AST *ast, FILE *outfile) {
    if (!ast) {
        return;
    }
    switch (ast->type) {
    case AST_TYPE_REF:
        if (strcmp(ast->data.type_ref.name, "int") == 0) {
            fprintf(outfile, "Int32");
        }
        else if (strcmp(ast->data.type_ref.name, "long") == 0) {
            fprintf(outfile, "Int64");
        }
        else if (strcmp(ast->data.type_ref.name, "float") == 0) {
            fprintf(outfile, "Float32");
        }
        else if (strcmp(ast->data.type_ref.name, "double") == 0) {
            fprintf(outfile, "Float64");
        }
        else if (strcmp(ast->data.type_ref.name, "str") == 0) {
            fprintf(outfile, "String");
        }
        else if (strcmp(ast->data.type_ref.name, "bool") == 0) {
            fprintf(outfile, "Bool");
        }
        else {
            fprintf(outfile, "%s", ast->data.type_ref.name);
        }
        break;
    default:
        fprintf(outfile, "/*Unknown type: %d*/0", ast->type);
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
    case AST_FUNC_DECL:
        while (ast) {
            swift_print_stmt(ast, outfile, indent_level);
            ast = ast->next;
        }
        break;

    case AST_TYPE_REF:
        swift_print_type(ast, outfile);
        if (ast->next) {
            fprintf(outfile, "/*Unexpected next expr node*/");
        }
        break;

    case AST_NUM_EXPR:
    case AST_VAR_REF:
        swift_print_expr(ast, outfile);
        if (ast->next) {
            fprintf(outfile, "/*Unexpected next expr node*/");
        }
        break;

    default:
        fprintf(outfile, "/*Unknown AST type: %d*/", ast->type);
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
