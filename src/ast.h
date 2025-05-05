#pragma once

enum ASTType {
    AST_CLASS_DECL = 1,
    AST_UNRESOLVED_TYPE_REF,
};

struct AST {
    enum ASTType type;
    union {
        struct {
            char *name;
            struct AST *base_class;
            struct AST *members;
        } class_decl;
        struct {
            char *name;
        } unresolved_type_ref;
    } data;
    struct AST *next;
};

struct AST *ast_new(enum ASTType type);
void ast_free(struct AST *ast);

void ast_print(struct AST *ast, int indent);

struct AST *ast_new_class_decl(const char *name, struct AST *base_class, struct AST *members, int first_line);

