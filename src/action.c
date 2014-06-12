/**
 * Action.c
 *
 */

#include "action.h"

#include "type.h"
#include "gen.h"

void action(ast* prog) {
    printf("---------------AST-----------------:\n");
    print_ast(prog, 0);
    print_ast_code_style(prog);
    
    int res = typecheck(prog);
    if (res) {
      return;
    }

    gen_code(prog);
}
