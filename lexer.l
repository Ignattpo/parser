%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "parser.h"
%}

%option noyywrap
%option yylineno

%%
[ \t]
"assert"                        { yylval.num = yylineno-1; return ASSERT; }
\*[A-Za-z_]+([A-Za-z_0-9]+)?    { yylval.index = get_variable(&yytext[1]); return TAKE_VALUE;}
\&[A-Za-z_]+([A-Za-z_0-9]+)?    { yylval.index = get_variable(&yytext[1]); return TAKE_POINTER;}
[A-Za-z_]+([A-Za-z_0-9]+)?      { yylval.str = strdup(yytext); return VARIABLE;}
\"([^"\\]|\\.|\\\n)*\"          { yylval.str = get_string(yytext); return STRING;}
0x+([0-9a-fA-F]+)               { yylval.num = (int)strtol(yytext, NULL, 0); return NUMBER;}
[0-9]+(\.[0-9]+)?               { yylval.num = atol(yytext); return NUMBER; }
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
";"                             { return SEMICOLON; }
[\n]                            { return EOL; }
.                               { printf("Undefined symbol at line %d",yylineno-1); exit(1); }
%%
