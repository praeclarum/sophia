#pragma once

enum ASTType {
    AST_CLASS_DECL = 1,
};

struct AST {
    enum ASTType type;
    union {
        struct {
            char *name;
            struct AST *args;
        } class_decl;
    } data;
    struct AST *next;
};

struct AST *ast_new(enum ASTType type);
void ast_free(struct AST *ast);
void ast_print(struct AST *ast, int indent);
