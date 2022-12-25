%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <dlfcn.h>

#include "command.h"

#include "parser.h"

#include "lexer.h"

/* Flex functions */
extern void yyterminate();
extern FILE* yyin;
void* get_ptr_func(const char *s);
%}

%code provides {
   #define YY_DECL \
       int yylex(struct parser_command_t* parser)
   YY_DECL;

   void yyerror(struct parser_command_t* parser,const char *s);
}

%param { struct parser_command_t* parser }

%union {
    struct{
        struct parser_command_t* parser;
        union{
                int index;
                long num;
                const char *str;
        }
    }data;
}

%token<data.num> NUMBER
%token<data.num> L_BRACKET R_BRACKET COMMA
%token<data.num> DIV MUL ADD SUB ASSIGN
%token<data.parser->socket> EOL
%token<data.parser->socket> SEMICOLON
%token<data> TAKE_POINTER
%token<data> TAKE_VALUE
%token<data> VARIABLE
%token<data.str> STRING
%token<data> ASSERT
%token<data.num> EQ NE GE GT LE LT NOT OR AND OR_BIT AND_BIT
%token<data> RESOLV READ WRITE MEM_DUMP MEM_WRITE MEM_DUMP_TO_FILE


%type<data.num> program_input
%type<data.num> line
%type<data.num> expr
%type<data.num> calculation
%type<data.num> assignment
%type<data.num> function
%type<data.num> arg
%type<data.num> assert
%type<data.num> boolean

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
    EOL                          { char output[255];\
                                   sprintf(output, "Please enter a command:\n");\
                                   send($1, output, strlen(output), 0);
                                 }
    | calculation EOL
    | calculation SEMICOLON EOL  {char output[255];\
                                  sprintf(output, "0x%lx %ld\n",$1, $1);\
                                  send($2, output, strlen(output), 0);\
                                 }
    | calculation SEMICOLON      {char output[255];\
                                  sprintf(output, "0x%lx %ld\n",$1, $1);\
                                  send($2, output, strlen(output), 0);\
                                 }
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
                                                char output[255];\
                                                sprintf(output, "Assert occurred at line %ld\n", $1.num);\
                                                yyerror($1.parser,output);\
                                                return 0;
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
        | NUMBER            { $$ = $1;}
        | VARIABLE          {int i = get_variable(&$1.parser->variables,$1.str);\
                             if (i < 0) {\
                                 yyerror($1.parser,"Not initialised variable\n");\
                                 return 0;\
                             } else $$ = $1.parser->variables.values[i];\
                            }
        | expr DIV expr     { if ($3 == 0) {\
                                return 0; \
                                } else $$ = $1 / $3;\
                            }
        | expr MUL expr     { $$ = $1 * $3; }
        | expr TAKE_VALUE   { if ($2.index < 0) {\
                                yyerror($2.parser,"Not initialised variable\n");\
                                return 0;\
                              } else $$ = $1 * $2.parser->variables.values[$2.index];\
                             }
        | L_BRACKET expr R_BRACKET { $$ = $2; }
        | expr ADD expr     { $$ = $1 + $3; }
        | expr SUB expr     { $$ = $1 - $3; }
        | TAKE_POINTER      { if ($1.index < 0) {\
                                yyerror($1.parser,"Not initialised variable\n");\
                                return 0;\
                               } else $$ = (int*)&$1.parser->variables.values[$1.index];\
                            }
        | TAKE_VALUE        { if ($1.index < 0) { \
                                    yyerror($1.parser,"Not initialised variable\n");\
                                    return 0;\
                                } else $$ = *(int*)$1.parser->variables.values[$1.index];\
                            }
        | boolean
        ;

assignment:
      VARIABLE ASSIGN calculation {int i = add_variable(&$1.parser->variables,$1.str);\
                                   $$ = set_variable(&$1.parser->variables,i, $3); }
      | TAKE_VALUE ASSIGN calculation { if ($1.index < 0) {\
                                        yyerror($1.parser,"Not initialised variable\n");\
                                        return 0; \
                                       } else *(int*)$1.parser->variables.values[$1.index]=$3;\
                                      }
      ;

function:
       RESOLV STRING                                                               {resolve_variable($1.parser->socket,$2);}
      | RESOLV expr                                                                {resolve_address($1.parser->socket,$2);}
      | READ VARIABLE STRING                                                       {read_variable($1.parser->socket,$2.str,$3);}
      | READ VARIABLE expr                                                         {read_address($1.parser->socket,$2.str,$3);}
      | WRITE VARIABLE STRING expr                                                 {write_variable($1.parser->socket,$2.str,$3,$4);}
      | WRITE VARIABLE expr expr                                                   {write_address($1.parser->socket,$2.str,$3,$4);}
      | MEM_DUMP STRING expr                                                       {memory_dump_variable($1.parser->socket,$2,$3);}
      | MEM_DUMP expr expr                                                         {memory_dump_address($1.parser->socket,$2,$3);}
      | MEM_WRITE STRING STRING                                                    {memory_write_variable($1.parser->socket,$2,$3);}
      | MEM_WRITE expr STRING                                                      {memory_write_address($1.parser->socket,$2,$3);}
      | MEM_DUMP_TO_FILE STRING expr expr                                          {memory_dump_to_file_address($1.parser->socket,$2,$3,$4);}
      | MEM_DUMP_TO_FILE STRING STRING expr                                        {memory_dump_to_file_variable($1.parser->socket,$2,$3,$4);}


       | VARIABLE L_BRACKET R_BRACKET                                              {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1.str);
                                                                                      if (!ptr_func ) {
                                                                                          char string_out[1024];
                                                                                          sprintf(string_out, "Not found function %s\n",$1.str);
                                                                                          yyerror($1.parser,string_out);
                                                                                          return 0;
                                                                                      }
                                                                                      $$ = ptr_func();
                                                                                    }
      | VARIABLE L_BRACKET arg R_BRACKET                                            {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1.str);
                                                                                      if (!ptr_func ) {
                                                                                          char string_out[1024];
                                                                                          sprintf(string_out, "Not found function %s\n",$1.str);
                                                                                          yyerror($1.parser,string_out);
                                                                                          return 0;
                                                                                      }
                                                                                      $$ = ptr_func($3);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg R_BRACKET                                  {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1.str);
                                                                                      if (!ptr_func ) {
                                                                                          char string_out[1024];
                                                                                          sprintf(string_out, "Not found function %s\n",$1.str);
                                                                                          yyerror($1.parser,string_out);
                                                                                          return 0;
                                                                                      }
                                                                                      $$ = ptr_func($3,$5);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg R_BRACKET                        {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1.str);
                                                                                      if (!ptr_func ) {
                                                                                          char string_out[1024];
                                                                                          sprintf(string_out, "Not found function %s\n",$1.str);
                                                                                          yyerror($1.parser,string_out);
                                                                                          return 0;
                                                                                      }
                                                                                      $$ = ptr_func($3,$5,$7);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA arg R_BRACKET              {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1.str);
                                                                                      if (!ptr_func ) {
                                                                                          char string_out[1024];
                                                                                          sprintf(string_out, "Not found function %s\n",$1.str);
                                                                                          yyerror($1.parser,string_out);
                                                                                          return 0;
                                                                                      }
                                                                                      $$ = ptr_func($3,$5,$7,$9);
                                                                                    }
      | VARIABLE L_BRACKET arg COMMA arg COMMA arg COMMA arg COMMA arg R_BRACKET    {
                                                                                      void* (*ptr_func)() = NULL;
                                                                                      ptr_func = get_ptr_func($1.str);
                                                                                      if (!ptr_func ) {
                                                                                          char string_out[1024];
                                                                                          sprintf(string_out, "Not found function %s\n",$1.str);
                                                                                          yyerror($1.parser,string_out);
                                                                                          return 0;
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
        return NULL;
    }
    return ptr_func;
}

/* Display error messages */
void yyerror(struct parser_command_t* parser,const char *s)
{
  char string_out[1024];
  sprintf(string_out, "ERROR: %s", s);
  send(parser->socket, string_out, strlen(string_out), 0);
}
