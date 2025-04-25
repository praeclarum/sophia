#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parse.h"

int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, FILE *infile) {
    int c;
    while ((c = fgetc(infile)) == ' ' || c == '\t') {
        ++llocp->last_column;
    }
    llocp->first_line = llocp->last_line;
    llocp->first_column = llocp->last_column;

    // fprintf(stderr, "Lexing char: 0x%x ('%c')\n", c, c);

    if (isdigit(c)) {
        *lvalp = c - '0';
        ++llocp->last_column;
        while (isdigit(c = fgetc(infile))) {
            ++llocp->last_column;
            *lvalp = *lvalp * 10 + c - '0';
        }
        ungetc(c, infile);
        return NUM;
    }

    if (c == EOF) {
        return 0;
    }

    if (c == '\n') {
        ++llocp->last_line;
        llocp->last_column = 0;
        return EOL;
    }

    ++llocp->last_column;
    return c;
}

void yyerror(YYLTYPE *llocp, FILE *infile, char const *message) {
    fprintf(stderr, "%s at line %d, column %d\n", message, llocp->first_line, llocp->first_column);
    exit(EXIT_FAILURE);
    (void)infile;
}
