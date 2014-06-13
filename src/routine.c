/**
* Parsing Routine
* File   : routine.c
* Author : Ryannnnnnn
*/

#include "routine.h"

void routine(ast* prog) {
    // print Abstract Semantic Tree
    print_ast_init();
    print_ast(prog, 0);
    print_ast_finish();

    // Code style
    print_ast_code_style(prog, 0);

    // Type check 
    int res = type_check(prog);
    if (res) {
        return;
    }

    // Code gen
    code_gen(prog);
}
