#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"

#define MAX_ID_LEN 1024

struct VM;

struct LexState {
    FILE *file;
    int indent_depth;
    int last_indent;
    int bol;
    int eof;
};

struct LexState *lex_begin(FILE *infile) {
    struct LexState *state = calloc(1, sizeof(struct LexState));
    if (!state) {
        fprintf(stderr, "Error: Could not allocate memory for lex state\n");
        return NULL;
    }
    state->file = infile;
    state->bol = 1;
    return state;
}

void lex_end(struct LexState *state) {
    if (state) {
        free(state);
    }
}

int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, struct LexState *state) {
    int id_len = 0;
    FILE *infile = state->file;
    char id[MAX_ID_LEN];
    int c = 0;
    int new_indent = 0;
    id[0] = '\0';
    llocp->first_line = llocp->last_line;
    llocp->first_column = llocp->last_column;
    if (state->eof) {
        if (state->indent_depth > 0) {
            state->indent_depth--;
            return DEDENT;
        }
        return 0;
    }
    c = fgetc(infile);
    if (c == EOF) {
        state->eof = 1;
        if (state->indent_depth > 0) {
            state->indent_depth--;
            return DEDENT;
        }
        return 0;
    }
    if (c == '\r') {
        c = fgetc(infile);
        if (c != '\n') {
            ungetc(c, infile);
        }
        ++llocp->last_line;
        llocp->last_column = 0;
        state->bol = 1;
        return EOL;
    }
    if (c == '\n') {
        ++llocp->last_line;
        llocp->last_column = 0;
        state->bol = 1;
        return EOL;
    }
    if (c == '\t') {
        ++llocp->last_column;
        return '\t';
    }
    if (state->bol && c == ' ') {
        state->bol = 0;
        while (c == ' ') {
            ++llocp->last_column;
            ++new_indent;
            c = fgetc(infile);
        }
        if (new_indent > state->last_indent) {
            ungetc(c, infile);
            state->last_indent = new_indent;
            state->indent_depth++;
            return INDENT;
        }
        else if (new_indent < state->last_indent) {
            ungetc(c, infile);
            state->last_indent = new_indent;
            if (state->indent_depth > 0) {
                state->indent_depth--;
            }
            return DEDENT;
        }
        else {
            ++llocp->last_column;
        }
    }
    else {
        state->bol = 0;
        while (c == ' ') {
            ++llocp->last_column;
            c = fgetc(infile);
        }
    }

    if (c == EOF) {
        state->eof = 1;
        if (state->indent_depth > 0) {
            state->indent_depth--;
            return DEDENT;
        }
        return 0;
    }

    if (isdigit(c)) {
        lvalp->num = c - '0';
        ++llocp->last_column;
        while (isdigit(c = fgetc(infile))) {
            ++llocp->last_column;
            lvalp->num = lvalp->num * 10 + c - '0';
        }
        ungetc(c, infile);
        return NUMBER;
    }

    if (isalpha(c) || c == '_') {
        id[id_len++] = c;
        while ((c = fgetc(infile)) && (isalnum(c) || c == '_')) {
            ++llocp->last_column;
            if (id_len < MAX_ID_LEN - 1) {
                id[id_len++] = c;
            }
        }
        ungetc(c, infile);
        id[id_len] = '\0';
        if (strcmp(id, "class") == 0) {
            return CLASS;
        }
        lvalp->str = strdup(id);
        return IDENTIFIER;
    }

    if (c == '\n') {
        ++llocp->last_line;
        llocp->last_column = 0;
        return EOL;
    }

    ++llocp->last_column;
    return c;
}

void yyerror(YYLTYPE *llocp, struct VM *vm, FILE *infile, char const *message) {
    fprintf(stderr, "%s at line %d, column %d\n", message, llocp->first_line, llocp->first_column);
    (void)infile;
    (void)vm;
}
