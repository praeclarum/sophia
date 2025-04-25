#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, FILE *infile) {
    (void)lvalp;
    (void)llocp;
    (void)infile;
    return 0;
}

void yyerror(YYLTYPE *llocp, FILE *infile, char const *message) {
    fprintf(stderr, "%s at line %d, column %d\n", message, llocp->first_line, llocp->first_column);
    exit(EXIT_FAILURE);
    (void)infile;
}
