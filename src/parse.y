%require "2.3"
%pure-parser
%error-verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "vm.h"
#include "parse.h"
%}

%parse-param {struct VM *vm}
%parse-param {FILE *infile}
%lex-param   {FILE *infile}

%{
int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, FILE *infile);
void yyerror(YYLTYPE *llocp, struct VM *vm, FILE *infile, char const *);
%}

%union {
    char *str;
    int num;
}

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
%left ',' ':' ';'

%token NUMBER
%token<str> IDENTIFIER
%token EOL
%token CLASS

%start translation_unit

%%

translation_unit
    : statement
    | translation_unit statement
    {
        (void)yynerrs;
        (void)@1.first_line;
    }
    ;

statement
    : class_declaration
    ;

class_declaration
    : CLASS IDENTIFIER EOL
    {
        vm_eval_ast(vm, ast_new_class_decl($2, NULL, NULL));
    }
    ;


%%
