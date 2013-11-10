%{
#include <iostream>
#include <cstring>
#include "miner_type.h"
#include "utils.h"
using namespace std;

Struct *output;

extern "C"
{
    void yyerror(const char *);
    int yylex();
    extern int yylineno;
    void switch_string(const char *str);
}

%}

%code requires { #include "miner_type.h" }

%union {
    int int_value;
    char *string_value;

    Value *value_value;
    List *list_value;
    Struct *struct_value;
    std::pair<int, int> *range_value;
    std::array<int, 3> *triple_value;
}

%token TOKEN_STRING
%token TOKEN_KEY
%token TOKEN_INTEGER
%token TOKEN_STRUCT_START TOKEN_STRUCT_END
%token TOKEN_EQUAL_SIGN
%token TOKEN_SEMICOLON
%token TOKEN_LEFT_PARENTHESIS TOKEN_RIGHT_PARENTHESIS
%token TOKEN_PAIR_DELIMITER
%token TOKEN_COMMA

%type<int_value> TOKEN_INTEGER
%type<string_value> TOKEN_STRING TOKEN_KEY string key
%type<value_value> value
%type<list_value> list
%type<struct_value> struct assignments
%type<range_value> range
%type<triple_value> triple

%start file

%%

file : TOKEN_KEY TOKEN_EQUAL_SIGN struct TOKEN_SEMICOLON {output = $3; }
     ;

string : TOKEN_STRING {
           string tmp($1);
           tmp.erase(0, 1);
           tmp.erase(tmp.size() - 1, 1);
           $$ = strdup(tmp.c_str());
       }
       ;

key : TOKEN_KEY {
        $$ = strdup($1);
    }
    ;

value : string {$$ = new Value($1); free($1); }
      | TOKEN_INTEGER {$$ = new Value($1); }
      | struct {$$ = new Value(move(*$1)); delete $1; }
      | range {$$ = new Value(move(*$1)); delete $1; }
      | triple {$$ = new Value(move(*$1)); delete $1; }
      | list {$$ = new Value(move(*$1)); delete $1; }
      ;

struct : TOKEN_STRUCT_START assignments TOKEN_STRUCT_END {
           $$ = $2;
       }
       ;

assignments : {
                $$ = new Struct();
            }
            |
            assignments key TOKEN_EQUAL_SIGN value TOKEN_SEMICOLON {
                (*$$)[$2] = move(*$4);
                delete $4;
                free($2);
            }
            ;

range : TOKEN_INTEGER TOKEN_PAIR_DELIMITER TOKEN_INTEGER {
          $$ = new pair<int, int>{$1, $3};
      }
      ;

triple : TOKEN_LEFT_PARENTHESIS TOKEN_INTEGER TOKEN_COMMA TOKEN_INTEGER
         TOKEN_COMMA TOKEN_INTEGER TOKEN_RIGHT_PARENTHESIS {
           $$ = new array<int, 3>{{$2, $4, $6}};
       }
       ;

list : TOKEN_LEFT_PARENTHESIS TOKEN_INTEGER TOKEN_COMMA 
       TOKEN_INTEGER TOKEN_RIGHT_PARENTHESIS {
         $$ = new List();
         pair<int, int> tmp{$2, $4};
         $$->push_back(Value(tmp));
     }
     | list TOKEN_COMMA TOKEN_LEFT_PARENTHESIS TOKEN_INTEGER TOKEN_COMMA
       TOKEN_INTEGER TOKEN_RIGHT_PARENTHESIS {
         pair<int, int> tmp{$4, $6};
         $$->push_back(Value(tmp));
     }
     ;

%%

Struct parse_miner_string(string &sin)
{
    switch_string(sin.c_str());

    int ret = yyparse();

    if (ret)
        FATAL("error while parsing miner file");

    Struct tmp = *output;
    delete output;
    return tmp;
}

void yyerror(const char *s)
{
    FATAL("%s on line %d", s, yylineno);
}

