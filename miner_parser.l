%option noyywrap yylineno
%{
#include <iostream>
#include "miner_parser.tab.h"

extern "C" 
{
    int yylex();
    void switch_string(const char *s);
}

using namespace std;
%}

%%
[ \t\n]          ;
\"[^\"]*\"       {yylval.string_value = yytext; return TOKEN_STRING; }
[a-zA-Z]+        {yylval.string_value = yytext; return TOKEN_KEY; }
-*[0-9]+         {yylval.int_value = atoi(yytext); return TOKEN_INTEGER; }
"{"              {return TOKEN_STRUCT_START; }
"}"              {return TOKEN_STRUCT_END; }
"="              {return TOKEN_EQUAL_SIGN; }
";"              {return TOKEN_SEMICOLON; }
".."             {return TOKEN_PAIR_DELIMITER; }
"("              {return TOKEN_LEFT_PARENTHESIS; }
")"              {return TOKEN_RIGHT_PARENTHESIS; }
","              {return TOKEN_COMMA; }
#.*$             ;
%%

void switch_string(const char *s)
{
    yy_scan_string(s);
}

