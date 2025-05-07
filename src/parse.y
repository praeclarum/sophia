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
    struct AST *ast;
}

%left '='
/* %nonassoc ASSIGN_ADD ASSIGN_SUB ASSIGN_MUL ASSIGN_DIV ASSIGN_MOD
%left OR
%left AND
%left EQ NE
%left '<' '>' LE GE */
%left '+' '-'
%left '*' '/' '%'
/* %left UMINUS */
%right '^'
%left '.' '(' '['
%left ',' ':' ';'

%token<num> NUMBER
%token<str> IDENTIFIER
%token EOL
%token CLASS
%token INDENT DEDENT
%token VAR
%token FOR IF ELSE RETURN
%token CLOSED_RANGE

%type<ast> vm_statement
%type<ast> class_declaration class_statements class_statement class_base
%type<ast> variable_declaration
%type<ast> function_declaration function_parameters function_parameter
%type<ast> statement statements
%type<ast> expression
%type<ast> type_ref

%start vm_statements

%%

vm_statements
    : vm_statement
    {
        vm_eval_ast(vm, $1);
    }
    | vm_statements vm_statement
    {
        vm_eval_ast(vm, $2);
        (void)yynerrs;
    }
    ;

vm_statement
    : class_declaration
    | variable_declaration EOL
    | EOL
    {
        $$ = NULL;
    }
    ;

class_declaration
    : CLASS IDENTIFIER class_base EOL
    {
        $$ = ast_new_class_decl($2, $3, NULL, @1.first_line);
    }
    | CLASS IDENTIFIER class_base '=' EOL INDENT class_statements DEDENT
    {
        $$ = ast_new_class_decl($2, $3, $7, @1.first_line);
    }
    ;

class_base
    : ':' type_ref
    {
        $$ = $2;
    }
    |
    {
        $$ = NULL;
    }
    ;

class_statements
    : class_statement
    | class_statements class_statement
    {
        ast_append($1, $2);
    }
    ;

class_statement
    : variable_declaration EOL
    | function_declaration EOL
    | EOL
    {
        $$ = NULL;
    }
    ;

variable_declaration
    : IDENTIFIER '=' expression
    {
        $$ = ast_new_var_decl($1, NULL, $3, 0, @1.first_line);
    }
    | IDENTIFIER ':' type_ref '=' expression
    {
        $$ = ast_new_var_decl($1, $3, $5, 0, @1.first_line);
    }
    | VAR IDENTIFIER '=' expression
    {
        $$ = ast_new_var_decl($2, NULL, $4, 1, @1.first_line);
    }
    | VAR IDENTIFIER ':' type_ref '=' expression
    {
        $$ = ast_new_var_decl($2, $4, $6, 1, @1.first_line);
    }
    ;

function_declaration
    : IDENTIFIER '(' ')' ':' type_ref '=' expression
    {
        $$ = ast_new_func_decl($1, NULL, $5, $7, @1.first_line);
    }
    | IDENTIFIER '(' ')' ':' type_ref '=' EOL INDENT statements DEDENT
    {
        $$ = ast_new_func_decl($1, NULL, $5, $9, @1.first_line);
    }
    | IDENTIFIER '(' function_parameters ')' ':' type_ref '=' expression
    {
        $$ = ast_new_func_decl($1, NULL, $6, $8, @1.first_line);
    }
    | IDENTIFIER '(' function_parameters ')' ':' type_ref '=' EOL INDENT statements DEDENT
    {
        $$ = ast_new_func_decl($1, NULL, $6, $10, @1.first_line);
    }
    ;

function_parameters
    : function_parameter
    | function_parameters ',' function_parameter
    {
        ast_append($1, $3);
    }
    ;

function_parameter
    : variable_declaration
    ;

statement
    : FOR IDENTIFIER '=' expression CLOSED_RANGE expression EOL INDENT statements DEDENT
    {
        $$ = $4;
    }
    | IF expression EOL INDENT statements DEDENT
    {
        $$ = $2;
    }
    | IF expression EOL INDENT statements DEDENT ELSE EOL INDENT statements DEDENT
    {
        $$ = $2;
    }
    | RETURN expression EOL
    {
        $$ = $2;
    }
    | variable_declaration EOL
    | expression EOL
    ;

statements
    : statement
    | statements statement
    {
        ast_append($1, $2);
    }
    ;

expression
    : NUMBER
    {
        $$ = ast_new_num_expr($1, @1.first_line);
    }
    | IDENTIFIER
    {
        $$ = ast_new_var_ref($1, @1.first_line);
    }
    /* | expression IF expression ELSE expression
    {
    } */
    ;

type_ref
    : IDENTIFIER
    {
        $$ = ast_new_type_ref($1, @1.first_line);
    }
    ;

%%

void print_token(int token, FILE *out) {
    fprintf(out, "%s", yytname[YYTRANSLATE(token)]);
}
