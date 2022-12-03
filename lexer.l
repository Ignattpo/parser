%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "parser.h"
%}

%option noyywrap

%%
[ \t]
\*[A-Za-z_]+([A-Za-z_0-9]+)?    {yylval.index = get_variable(yytext); return TAKE_VALUE;}
\&[A-Za-z_]+([A-Za-z_0-9]+)?    {yylval.index = get_variable(yytext); return TAKE_POINTER;}
[A-Za-z_]+([A-Za-z_0-9]+)?      { yylval.index = add_variable(yytext); return VARIABLE;}
0x+([0-9a-fA-F]+)               { yylval.num = (int)strtol(yytext, NULL, 0); return NUMBER;}
[0-9]+(\.[0-9]+)?               { yylval.num = atof(yytext); return NUMBER; }
"("                         	{ return L_BRACKET; }
")"         			{ return R_BRACKET; }
"/"                     { return DIV; }
"*"                     { return MUL; }
"+"                     { return ADD; }
"-"                     { return SUB; }
"="                     { return EQUALS; }
[\n]                    { return EOL; }
.                       { yyerror("Undefined symbol"); exit(1); }
%%
