#include <stdio.h>
#include <stdlib.h>
#include "parse.h"

int yylex(YYSTYPE *lvalp, YYLTYPE *llocp) {
    (void) lvalp;
    (void) llocp;
    return 0;
}

void yyerror(char const *message) {
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}
