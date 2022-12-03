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
\*[A-Za-z_]+([A-Za-z_0-9]+)?    {yylval.index = get_variable(&yytext[1]); return TAKE_VALUE;}
\&[A-Za-z_]+([A-Za-z_0-9]+)?    {yylval.index = get_variable(&yytext[1]); return TAKE_POINTER;}
[A-Za-z_]+([A-Za-z_0-9]+)?      { yylval.str = strdup(yytext); return VARIABLE;}
\"(.*)\"                        { yylval.str = strdup(yytext); return STRING;}
0x+([0-9a-fA-F]+)               { yylval.num = (int)strtol(yytext, NULL, 0); return NUMBER;}
[0-9]+(\.[0-9]+)?               { yylval.num = atof(yytext); return NUMBER; }
"("                         	{ return L_BRACKET; }
")"         			{ return R_BRACKET; }
"/"                     { return DIV; }
"*"                     { return MUL; }
"+"                     { return ADD; }
"-"                     { return SUB; }
"="                     { return EQUALS; }
","                     { return COMMA; }
[\n]                    { return EOL; }
.                       { yyerror("Undefined symbol"); exit(1); }
%%
