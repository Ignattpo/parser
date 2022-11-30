%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "func.h"
#include "parser.h"

extern double variable_values[100];
extern int variable_set[100];

/* Flex functions */
extern int yylex(void);
extern void yyterminate();
void yyerror(const char *s);
extern FILE* yyin;
%}


%union {
    int index;
    double num;
}

%token<num> NUMBER
%token<num> L_BRACKET R_BRACKET
%token<num> DIV MUL ADD SUB EQUALS
%token<num> EOL

%type<num> program_input
%type<num> line
%type<num> expr
%type<num> calculation

/* Set operator precedence, follows BODMAS rules. */
%left SUB
%left ADD
%left MUL
%left DIV
%left POW
%left L_BRACKET R_BRACKET

%%
program_input:
    | program_input line
    ;

line:
    EOL                { printf("Please enter a calculation:\n"); }
    | calculation EOL  { printf("=%.2f\n",$1); }
    ;

calculation:
          expr
        ;
expr:
          SUB expr          { $$ = -$2; }
        | NUMBER            { $$ = $1; }
        | expr DIV expr     { if ($3 == 0) { yyerror("Cannot divide by zero"); exit(1); } else $$ = $1 / $3; }
        | expr MUL expr     { $$ = $1 * $3; }
        | L_BRACKET expr R_BRACKET { $$ = $2; }
        | expr ADD expr     { $$ = $1 + $3; }
        | expr SUB expr     { $$ = $1 - $3; }
        | expr POW expr     { $$ = pow($1, $3); }
        ;

%%

    /* Entry point */


/* Display error messages */
void yyerror(const char *s)
{
    printf("ERROR: %s\n", s);
}