#pragma once

#include <stdio.h>

struct VM;

struct VM *vm_new();
void vm_free(struct VM *vm);

int vm_eval_file(struct VM *vm, FILE *infile);

int vm_repl(struct VM *vm);
int vm_translate(struct VM *vm, const char *output_file);
