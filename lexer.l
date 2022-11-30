%{
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mem.h"
#include "parser.h"
%}

/* Stop the error "undefined reference to 'yywrap'" from Flex */
%option noyywrap

/* Grammar rules
    [ \t] -- Match space or tab

    [0-9]+(\.[0-9]+)?  -- Match any combination of numbers. Includes decimals.

    [a-zA-Z0-9]+ -- Match any combination of letters (lower and upper case) and numbers.

    . -- Match any remaining character.
*/

%%
[ \t]
[0-9]+(\.[0-9]+)?       { yylval.num = atof(yytext); return NUMBER; }
"("			{ return L_BRACKET; }
")"			{ return R_BRACKET; }
"/"                     { return DIV; }
"*"                     { return MUL; }
"+"                     { return ADD; }
"-"                     { return SUB; }
"="                     { return EQUALS; }
[\n]                    { return EOL; }
. 																		{ yyerror("Undefined symbol"); exit(1); }
%%
