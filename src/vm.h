#pragma once

#include <stdio.h>
#include "ast.h"

int yyparse(FILE *infile);

struct VM {
    struct AST *ast;
};

struct VM *vm_new();
void vm_free(struct VM *vm);

int vm_eval_file(struct VM *vm, FILE *infile);

int vm_repl(struct VM *vm);
