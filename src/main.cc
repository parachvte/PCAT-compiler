/************************************************************************
*
* File: main.cc
* Testing the PCAT lexer
* Programmer: Leonidas Fegaras, UTA
* Date: 2/9/98
* 
************************************************************************/

static int columnno = 1;
#include "tokens.h"  // define tokens
#include "pcat.yy.c"  // lexical patterns

#include <string>
#include <cstdio>


using namespace std;

extern string keywords[];


int main(int argc, char* arg[]) {
    if (argc > 1)
        yyin = fopen(arg[1], "r");

    for (;;) {
        short n = yylex();
        if (n == EOFF) break;

        // print (Line, Column)
        printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
        // print 
        if (n < 262) {
            // identifier, interger, real, string
            printf("%s: %s\n", keywords[n - 258].c_str(), yytext);
        } else {
            // reversed keywords indeed
            printf("%s\n", keywords[n - 258].c_str());
        }
    }
}
