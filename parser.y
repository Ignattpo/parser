%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include "func.h"
#include "parser.h"

extern long variable_values[100];
extern int variable_set[100];

/* Flex functions */
extern int yylex(void);
extern void yyterminate();
extern FILE* yyin;
void yyerror(const char *s);
%}

%union {
    int index;
    long num;
    const char *str;
}

%token<num> NUMBER
%token<num> L_BRACKET R_BRACKET COMMA
%token<num> DIV MUL ADD SUB EQUALS
%token<num> EOL
%token<index> TAKE_POINTER
%token<index> TAKE_VALUE
%token<str> VARIABLE
%token<str> STRING


%type<num> program_input
%type<num> line
%type<num> expr
%type<num> calculation
%type<num> assignment
%type<num> function
%type<num> arg

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
    | calculation EOL  { printf("=%x\n",$1); }
    ;

calculation:
         assignment
        | function
        | expr
        ;
expr:
          SUB expr          { $$ = -$2; }
        | NUMBER            { $$ = $1; }
        | VARIABLE          {int i = get_variable($1); if (i < 0) { yyerror("Not initialised variable"); exit(1); } else $$ = variable_values[i]; }
        | expr DIV expr     { if ($3 == 0) { yyerror("Cannot divide by zero"); exit(1); } else $$ = $1 / $3; }
        | expr MUL expr     { $$ = $1 * $3; }
        | L_BRACKET expr R_BRACKET { $$ = $2; }
        | expr ADD expr     { $$ = $1 + $3; }
        | expr SUB expr     { $$ = $1 - $3; }
        | expr POW expr     { $$ = pow($1, $3); }
        | TAKE_POINTER      { if ($1 < 0) { yyerror("Not initialised variable"); exit(1); } else $$ = (int*)&variable_values[$1]; }
        | TAKE_VALUE        { if ($1 < 0) { yyerror("Not initialised variable"); exit(1); } else $$ = *(int*)variable_values[$1];}
        ;

assignment:
      VARIABLE EQUALS calculation {int i = add_variable($1); $$ = set_variable(i, $3); }
      ;

function:
       VARIABLE L_BRACKET R_BRACKET                                                 { printf("FUNCTION %s\n",$1);}
      | VARIABLE L_BRACKET arg R_BRACKET                                            { printf("FUNCTION %s(%s)\n",$1,$3);}
      | VARIABLE L_BRACKET arg COMMA arg R_BRACKET                                  { printf("FUNCTION %s\n",$1);}
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA R_BRACKET                  { printf("FUNCTION %s\n",$1);}
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA arg R_BRACKET              { printf("FUNCTION %s\n",$1);}
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA arg COMMA arg R_BRACKET    { printf("FUNCTION %s\n",$1);}
      ;
arg:
      expr
      | STRING { $$ = $1;}
      ;

%%

/* Display error messages */
void yyerror(const char *s)
{
  printf("ERROR: %s\n", s);
}

