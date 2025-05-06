#pragma once

#include <stdio.h>

struct LexState;

struct LexState *lex_begin(FILE *infile);
void lex_end(struct LexState *state);

void lex_output_tokens(const char *input_file, const char *output_file);
