#pragma once

enum ASTType {
    AST_CLASS_DECL = 1,
    AST_UNRESOLVED_TYPE_REF,
    AST_VAR_DECL,
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
        } unresolved_type_ref;
        struct {
            char *name;
            struct AST *value;
        } var_decl;
        struct {
            int value;
        } num_expr;
    } data;
    struct AST *next;
};

struct AST *ast_new(enum ASTType type);
void ast_free(struct AST *ast);

void ast_append(struct AST *parent, struct AST *child);

void ast_print(struct AST *ast, FILE *outfile, int indent_level);

struct AST *ast_new_class_decl(const char *name, struct AST *base_class, struct AST *members, int first_line);
struct AST *ast_new_var_decl(const char *name, struct AST *value, int first_line);
struct AST *ast_new_num_expr(int value, int first_line);

