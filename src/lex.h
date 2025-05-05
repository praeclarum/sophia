#pragma once

#include <stdio.h>

struct LexState;

struct LexState *lex_begin(FILE *infile);
void lex_end(struct LexState *state);
