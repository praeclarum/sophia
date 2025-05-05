#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"

#define MAX_ID_LEN 1024

struct VM;

struct LexState {
    FILE *infile;
};

struct LexState *lex_begin(FILE *infile) {
    struct LexState *state = calloc(1, sizeof(struct LexState));
    if (!state) {
        fprintf(stderr, "Error: Could not allocate memory for lex state\n");
        return NULL;
    }
    state->infile = infile;
    return state;
}

void lex_end(struct LexState *state) {
    if (state) {
        fclose(state->infile);
        free(state);
    }
}

int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, struct LexState *state) {
    int c;
    char id[MAX_ID_LEN];
    int id_len = 0;
    FILE *infile = state->infile;
    while ((c = fgetc(infile)) == ' ' || c == '\t') {
        ++llocp->last_column;
    }
    llocp->first_line = llocp->last_line;
    llocp->first_column = llocp->last_column;

    // fprintf(stderr, "Lexing char: 0x%x ('%c')\n", c, c);

    if (c == EOF) {
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
