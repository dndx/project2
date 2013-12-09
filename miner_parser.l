%option noyywrap yylineno
%{
#include <iostream>
#include "miner_parser.tab.h"

extern "C" 
{
    int yylex();
    void switch_string(const char *s); /* switch the string buffer to scan */
}

using namespace std;
%}

%%
[ \t\n]          ; /* skip spaces */
\"[^\"]*\"       {yylval.string_value = yytext; return TOKEN_STRING; } /* string */
[a-zA-Z]+        {yylval.string_value = yytext; return TOKEN_KEY; } /* struct key */
-*[0-9]+         {yylval.int_value = atoi(yytext); return TOKEN_INTEGER; } /* integer */
"{"              {return TOKEN_STRUCT_START; } /* start of struct */
"}"              {return TOKEN_STRUCT_END; } /* end of struct */
"="              {return TOKEN_EQUAL_SIGN; } /* equal sign */
";"              {return TOKEN_SEMICOLON; } /* semicolon */
".."             {return TOKEN_PAIR_DELIMITER; } /* delimiter for integer pair */
"("              {return TOKEN_LEFT_PARENTHESIS; } /* left parenthesis */
")"              {return TOKEN_RIGHT_PARENTHESIS; } /* right parenthesis */
","              {return TOKEN_COMMA; } /* comma */
#.*$             ; /* skip comments */
%%

/*
 * switch the string buffer to scan
 */
void switch_string(const char *s)
{
    yy_scan_string(s);
}

