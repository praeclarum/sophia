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
    if (!vm || !ast) {
        fprintf(stderr, "Error: Invalid VM or AST\n");
        return 1;
    }
    vm->ast = ast;
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
    ssize_t read;

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

int vm_translate(struct VM *vm, const char *output_file) {
    (void)vm;
    FILE *outfile = fopen(output_file, "w");
    if (!outfile) {
        fprintf(stderr, "Error: Could not open file %s for writing\n", output_file);
        return 1;
    }
    fprintf(outfile, "// Translation not implemented yet\n");
    fclose(outfile);
    return 0;
}
