%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#include "func.h"

#include "parser.h"
#include "lexer.h"


extern long variable_values[100];
extern int variable_set[100];

/* Flex functions */
extern void yyterminate();
extern FILE* yyin;

void* get_ptr_func(const char *s);
%}

%code provides {
   #define YY_DECL \
       int yylex(int a)
   YY_DECL;

   void yyerror(int a,const char *s);
}

%parse-param { int a }
%lex-param { int a}

%union {
    int index;
    long num;
    const char *str;
}

%token<num> NUMBER
%token<num> L_BRACKET R_BRACKET COMMA
%token<num> DIV MUL ADD SUB ASSIGN
%token<num> EOL SEMICOLON
%token<index> TAKE_POINTER
%token<index> TAKE_VALUE
%token<str> VARIABLE
%token<str> STRING
%token<str> ASSERT
%token<num> EQ NE GE GT LE LT NOT OR AND OR_BIT AND_BIT


%type<num> program_input
%type<num> line
%type<num> expr
%type<num> calculation
%type<num> assignment
%type<num> function
%type<num> arg
%type<num> assert
%type<num> boolean

/* Set operator precedence, follows BODMAS rules. */
%left SUB
%left ADD
%left MUL
%left DIV
%left OR OR_BIT
%left AND AND_BIT
%left NOT
%left L_BRACKET R_BRACKET

%%
program_input:
    | program_input line
    ;

line:
    EOL                          { printf("Please enter a calculation:\n"); }
    | calculation EOL
    | calculation SEMICOLON EOL  {printf("0x%x %d\n",$1, $1);}
    | calculation SEMICOLON      {printf("0x%x %d\n",$1, $1);}
    ;

calculation:
         assignment
        | function
        | expr
        | assert
        ;

assert:
        ASSERT L_BRACKET expr R_BRACKET {
                                            if(!$3){
                                                printf("Assert occurred at line %d\n", $1);
                                                exit(1);
                                            }
                                        }
         ;

boolean:
        expr EQ expr            { $$ =($1 == $3) ? true: false; }
        | expr NE expr          { $$ =($1 != $3) ? true: false; }
        | expr GE expr          { $$ =($1 >= $3) ? true: false; }
        | expr GT expr          { $$ =($1 > $3) ? true: false; }
        | expr LE expr          { $$ =($1 <= $3) ? true: false; }
        | expr LT expr          { $$ =($1 < $3) ? true: false; }
        | expr OR expr          { $$ =($1 || $3) ? true: false; }
        | expr AND expr         { $$ =($1 && $3) ? true: false; }
        | expr OR_BIT expr      { $$ =($1 | $3) ? true: false; }
        | expr AND_BIT expr     { $$ =($1 & $3) ? true: false; }
        | NOT expr              { $$ = !$2; }
        ;

expr:
          SUB expr          { $$ = -$2; }
        | NUMBER            { $$ = $1; }
        | VARIABLE          {int i = get_variable($1); if (i < 0) { yyerror(13,"VARIABLE Not initialised variable"); exit(1); } else $$ = variable_values[i]; }
        | expr DIV expr     { if ($3 == 0) { yyerror(13,"Cannot divide by zero"); exit(1); } else $$ = $1 / $3; }
        | expr MUL expr     { $$ = $1 * $3; }
        | expr TAKE_VALUE   {if ($2 < 0) { yyerror(13,"Not initialised variable"); exit(1); } else $$ = $1 * variable_values[$2];}
        | L_BRACKET expr R_BRACKET { $$ = $2; }
        | expr ADD expr     { $$ = $1 + $3; }
        | expr SUB expr     { $$ = $1 - $3; }
        | TAKE_POINTER      { if ($1 < 0) { yyerror(13,"Not initialised variable"); exit(1); } else $$ = (int*)&variable_values[$1]; }
        | TAKE_VALUE        { if ($1 < 0) { yyerror(13,"Not initialised variable"); exit(1); } else $$ = *(int*)variable_values[$1];}
        | boolean
        ;

assignment:
      VARIABLE ASSIGN calculation {int i = add_variable($1); $$ = set_variable(i, $3); }
      | TAKE_VALUE ASSIGN calculation { if ($1 < 0) { yyerror(13,"Not initialised variable"); exit(1); } else *(int*)variable_values[$1]=$3;}
      ;

function:
       VARIABLE L_BRACKET R_BRACKET                                                 {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1);
                                                                                      if (!ptr_func ) {
                                                                                          exit(1);
                                                                                      }
                                                                                      $$ = ptr_func();
                                                                                    }
      | VARIABLE L_BRACKET arg R_BRACKET                                            {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1);
                                                                                      if (!ptr_func ) {
                                                                                          exit(1);
                                                                                      }
                                                                                      $$ = ptr_func($3);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg R_BRACKET                                  {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1);
                                                                                      if (!ptr_func ) {
                                                                                          exit(1);
                                                                                      }
                                                                                      $$ = ptr_func($3,$5);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg R_BRACKET                        {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1);
                                                                                      if (!ptr_func ) {
                                                                                          exit(1);
                                                                                      }
                                                                                      $$ = ptr_func($3,$5,$7);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA arg R_BRACKET              {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1);
                                                                                      if (!ptr_func ) {
                                                                                          exit(1);
                                                                                      }
                                                                                      $$ = ptr_func($3,$5,$7,$9);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA arg COMMA arg R_BRACKET    {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1);
                                                                                      if (!ptr_func ) {
                                                                                          exit(1);
                                                                                      }
                                                                                      $$ = ptr_func($3,$5,$7,$9,$11);
                                                                                    }
      ;
arg:
      expr
      | STRING { $$ = $1;}
      | function
      ;

%%

void* get_ptr_func(const char *s)
{
    void* (*ptr_func)() = NULL;
    ptr_func = dlsym(NULL, s);
    void* error = dlerror();
    if (error) {
         printf("ERROR: Not found function %s\n", s);
        return NULL;
    }
    return ptr_func;
}

/* Display error messages */
void yyerror(int a,const char *s)
{
  printf("ERROR: %s\n", s);
}

