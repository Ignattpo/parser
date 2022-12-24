%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "variables.h"
#include "command.h"
#include "parser.h"
#include "func.h"
%}

%option noyywrap
%option yylineno

%%
[ \t]
"resolv"                        {
                                  yylval.data.parser=parser;
                                  return RESOLV;
                                }
"assert"                        {
                                  yylval.data.parser=parser;
                                  yylval.data.num = yylineno-1;
                                  return ASSERT;
                                }
\*[A-Za-z_]+([A-Za-z_0-9]+)?    {
                                  yylval.data.parser=parser;
                                  yylval.data.index = get_variable(&parser->variables, &yytext[1]);
                                  return TAKE_VALUE;
                                }
\&[A-Za-z_]+([A-Za-z_0-9]+)?    {
                                  yylval.data.parser=parser;
                                  yylval.data.index = get_variable(&parser->variables, &yytext[1]);
                                  return TAKE_POINTER;
                                }
[A-Za-z_]+([A-Za-z_0-9]+)?      {
                                  yylval.data.parser=parser;
                                  yylval.data.str = strdup(yytext);
                                  return VARIABLE;
                                }
\"([^"\\]|\\.|\\\n)*\"          {
                                  yylval.data.str = get_string(yytext);
                                  return STRING;
                                }
0x+([0-9a-fA-F]+)               {
                                  yylval.data.num = (long)strtol(yytext, NULL, 0);
                                  return NUMBER;
                                }
[0-9]+(\.[0-9]+)?               {
                                  yylval.data.num = atol(yytext);
                                  return NUMBER;
                                }
"=="                            { return EQ; }
"!="                            { return NE; }
">="                            { return GE; }
">"                             { return GT; }
"<="                            { return LE; }
"<"                             { return LT; }
"!"                             { return NOT; }
"||"                            { return OR; }
"&&"                            { return AND; }
"|"                             { return OR_BIT; }
"&"                             { return AND_BIT; }
"("                         	{ return L_BRACKET; }
")"                     		{ return R_BRACKET; }
"/"                             { return DIV; }
"*"                             { return MUL; }
"+"                             { return ADD; }
"-"                             { return SUB; }
"="                             { return ASSIGN; }
","                             { return COMMA; }
";"                             { yylval.data.parser=parser; return SEMICOLON; }
[\n]                            { yylval.data.parser=parser; return EOL; }
.                               {
                                  char err[255];
                                  sprintf(err, "Undefined symbol at line %d %s\n",yylineno-1,&yytext[1]);
                                  send(parser->socket, err, strlen(err), 0);
                                  return 0;
                                }
%%
