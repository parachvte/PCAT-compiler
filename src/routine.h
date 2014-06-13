#ifndef _ACTION_H_
#define _ACTION_H_
    
#include "ast.h"
    
void routine(ast* t);
    
/* Code Style */
void print_ast_code_style(ast* x, int offset);

/* Type Check */
int type_check(ast* x);

/* Code Gen */
void code_gen(ast* x);

#endif
