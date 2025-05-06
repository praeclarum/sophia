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
    int emitted_trailing_eol;
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

int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, struct LexState *lexstate);
void print_token(int token, FILE *out);

void lex_output_tokens(const char *input_file, const char *output_file) {
    FILE *infile = fopen(input_file, "r");
    if (!infile) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        return;
    }
    FILE *outfile = fopen(output_file, "w");
    if (!outfile) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        fclose(infile);
        return;
    }
    struct LexState *state = lex_begin(infile);
    YYSTYPE lval;
    YYLTYPE lloc;
    int token;
    while ((token = yylex(&lval, &lloc, state)) != 0) {
        print_token(token, outfile);
        fputc('\n', outfile);
    }
    lex_end(state);
    fclose(infile);
    fclose(outfile);
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
        if (!state->emitted_trailing_eol) {
            state->emitted_trailing_eol = 1;
            return EOL;
        }
        return 0;
    }
    c = fgetc(infile);
    while (state->bol) {
        state->bol = 0;
        while (c == ' ') {
            ++llocp->last_column;
            ++new_indent;
            c = fgetc(infile);
        }
        if (c == '\r' || c == '\n') {
            if (c == '\r') {
                c = fgetc(infile);
                if (c != '\n') {
                    ungetc(c, infile);
                }
            }
            state->bol = 1;
            ++llocp->last_line;
            llocp->last_column = 0;
            new_indent = 0;
            c = fgetc(infile);
            continue;
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
    while (c == ' ' || c == '\t') {
        ++llocp->last_column;
        c = fgetc(infile);
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

    if (c == EOF) {
        state->eof = 1;
        if (state->indent_depth > 0) {
            state->indent_depth--;
            return DEDENT;
        }
        if (!state->emitted_trailing_eol) {
            state->emitted_trailing_eol = 1;
            return EOL;
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
        else if (strcmp(id, "var") == 0) {
            return VAR;
        }
        lvalp->str = strdup(id);
        return IDENTIFIER;
    }

    ++llocp->last_column;
    return c;
}

void yyerror(YYLTYPE *llocp, struct VM *vm, FILE *infile, char const *message) {
    fprintf(stderr, "%s at line %d, column %d\n", message, llocp->first_line, llocp->first_column);
    (void)infile;
    (void)vm;
}
