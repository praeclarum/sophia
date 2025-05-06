#pragma once

enum ASTType {
    AST_CLASS_DECL = 1,
    AST_TYPE_REF,
    AST_VAR_DECL,
    AST_VAR_REF,
    AST_FUNC_DECL,
    AST_NUM_EXPR,
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
        } type_ref;
        struct {
            char *name;
            struct AST *type;
            struct AST *value;
            int mutable;
        } var_decl;
        struct {
            char *name;
        } var_ref;
        struct {
            char *name;
            struct AST *parameters;
            struct AST *type;
            struct AST *value;
        } func_decl;
        struct {
            int value;
        } num_expr;
    } data;
    struct AST *next;
};

void ast_free(struct AST *ast);

void ast_append(struct AST *parent, struct AST *child);

void ast_print(struct AST *ast, FILE *outfile, int indent_level);

struct AST *ast_new_class_decl(const char *name, struct AST *base_class, struct AST *members, int first_line);
struct AST *ast_new_type_ref(const char *name, int first_line);
struct AST *ast_new_var_decl(const char *name, struct AST *type, struct AST *value, int mutable, int first_line);
struct AST *ast_new_var_ref(const char *name, int first_line);
struct AST *ast_new_func_decl(const char *name, struct AST *parameters, struct AST *type, struct AST *value, int first_line);
struct AST *ast_new_num_expr(int value, int first_line);

