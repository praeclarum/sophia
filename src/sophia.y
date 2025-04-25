%require "2.3"
%pure-parser

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

%start translation_unit

%%

translation_unit:
    | '\n'
    ;

%%
