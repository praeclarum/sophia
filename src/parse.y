%require "2.3"
%pure-parser
%error-verbose

%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "vm.h"
#include "lex.h"
#include "parse.h"
%}

%parse-param {struct VM *vm}
%parse-param {struct LexState *lexstate}
%lex-param   {struct LexState *lexstate}

%{
int yylex(YYSTYPE *lvalp, YYLTYPE *llocp, struct LexState *lexstate);
void yyerror(YYLTYPE *llocp, struct VM *vm, struct LexState *lexstate, char const *);
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
%token INDENT DEDENT

%start translation_unit

%%

translation_unit
    : statement
    | translation_unit EOL statement
    {
        (void)yynerrs;
        (void)@1.first_line;
    }
    ;

statement
    : class_declaration
    | variable_declaration
    |
    ;

class_declaration
    : CLASS IDENTIFIER
    {
        vm_eval_ast(vm, ast_new_class_decl($2, NULL, NULL, @1.first_line));
    }
    | CLASS IDENTIFIER '=' EOL INDENT class_body DEDENT
    {
        vm_eval_ast(vm, ast_new_class_decl($2, NULL, NULL, @1.first_line));
    }
    ;

class_body
    : statement
    | class_body EOL statement
    ;

variable_declaration
    : IDENTIFIER '=' NUMBER
    {
    }
    ;

%%
