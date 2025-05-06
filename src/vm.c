#include <stdlib.h>
#include <stdio.h>

#include "ast.h"
#include "lex.h"
#include "vm.h"

struct VM {
    struct AST *ast;
};

struct VM *vm_new() {
    struct VM *vm = calloc(1, sizeof(struct VM));
    if (!vm) {
        fprintf(stderr, "Error: Could not allocate memory for VM\n");
    }
    return vm;
}

void vm_free(struct VM *vm) {
    if (!vm) {
        return;
    }
    if (vm->ast) {
        ast_free(vm->ast);
    }
    free(vm);
}

int vm_eval_ast(struct VM *vm, struct AST *ast) {
    if (!vm) {
        fprintf(stderr, "Error: Invalid VM\n");
        return 1;
    }
    if (vm->ast) {
        ast_append(vm->ast, ast);
    }
    else {
        vm->ast = ast;
    }
    return 0;
}

int yyparse(struct VM *vm, struct LexState *lex);

int vm_eval_file(struct VM *vm, FILE *infile) {
    (void)vm;
    struct LexState *lex = lex_begin(infile);
    int r = yyparse(vm, lex);
    lex_end(lex);
    return r;
}

int vm_repl(struct VM *vm) {
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;

    printf("sophia> ");
    while ((read = getline(&line, &len, stdin)) != -1) {
        if (read > 1) {
            FILE *infile = fmemopen(line, read, "r");
            if (!infile) {
                fprintf(stderr, "Error: Could not open memory stream\n");
                free(line);
                return 1;
            }
            vm_eval_file(vm, infile);
            fclose(infile);
        }
        printf("sophia> ");
    }
    free(line);
    return 0;
}

typedef int (*vm_translate_func)(struct AST *ast, FILE *outfile);

int translate_ast(struct AST *ast, FILE *outfile);
int translate_generic(struct AST *ast, FILE *outfile);
int translate_swift(struct AST *ast, FILE *outfile);

int str_endswith(const char *str, const char *suffix);

int vm_translate(struct VM *vm, const char *output_file) {
    int r = 0;
    vm_translate_func translate_func = translate_generic;
    FILE *outfile = fopen(output_file, "w");
    if (!outfile) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", output_file);
        return 1;
    }
    if (str_endswith(output_file, ".ast")) {
        translate_func = translate_ast;
    }
    else if (str_endswith(output_file, ".swift")) {
        translate_func = translate_swift;
    }
    r = translate_func(vm->ast, outfile);
    fclose(outfile);
    return r;
}

int translate_generic(struct AST *ast, FILE *outfile) {
    (void)ast;
    fprintf(outfile, "# Translation not supported for this file type\n");
    return 1;
}

int translate_ast(struct AST *ast, FILE *outfile) {
    ast_print(ast, outfile, 0);
    return 0;
}
