/************************************************************************
*
* File: main.cc
* Testing the PCAT lexer
* Programmer: Leonidas Fegaras, UTA
* Date: 2/9/98
* 
************************************************************************/

#include <stdio.h>

extern FILE* yyin;

void yyparse();

int main(int argc, char* arg[]) {
    if (argc > 1)
        yyin = fopen(arg[1], "r");

    yyparse();

    return 0;
}
