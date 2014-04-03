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

    bool success = true;
    for (;;) {
        short n = yylex();
        if (n == EOFF) break;
        if (n == ERROR) {
            success = false;
            // break;
        }
    }
    if (success) {
        printf("==============================\n");
        printf("Lexical scanning successful.\n");
    }
}
