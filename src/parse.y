%require "2.3"
%pure-parser
%error-verbose

%{
#include <stdio.h>
#include <stdlib.h>
%}

%parse-param {FILE *infile}
%lex-param   {FILE *infile}

%{
#include "parse.h"
int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, FILE *infile);
void yyerror(YYLTYPE *llocp, FILE *infile, char const *);
%}

%left '='
%nonassoc ASSIGN_ADD ASSIGN_SUB ASSIGN_MUL ASSIGN_DIV ASSIGN_MOD
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE
%left '+' '-'
%left '*' '/' '%'
%left UMINUS
%right '^'
%left '.' '(' '['
%left ',' ';'

%token NUM
%token EOL

%start translation_unit

%%

translation_unit:
    | '\n'
    {
        (void)yynerrs;
        (void)@1.first_line;
    }
    ;

%%
