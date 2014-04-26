/********************************************************************************
 *
 * File: ast.h
 * The abstract syntax trees for the calculator
 * Programmer: Leonidas Fegaras, UTA
 * Date: 12/17/97
 *
 ********************************************************************************/

#include "malloc.h"
#include <string.h>
#include <stdio.h>


/* Put the names of all the different kinds of ASTs here */

typedef enum {
    Program, Body,

    if_exp, eq_exp, lt_exp, gt_exp, le_exp, ne_exp, ge_exp, plus_exp, minus_exp,
    times_exp, div_exp, or_exp, and_exp, not_exp, call_exp, fnc_def
} ast_kind;

static char* ast_names[] = {
    "if_exp", "eq_exp", "lt_exp", "gt_exp", "le_exp", "ne_exp", "ge_exp", "plus_exp", "minus_exp",
    "times_exp", "div_exp", "or_exp", "and_exp", "not_exp", "call_exp", "fnc_def"
};


/* This is a universal data structure that can capture any AST:
 * The node is an internal node and has a list of children (other ASTs),
 * while the other nodes are leaves       */

typedef struct ast {
    enum { int_ast, real_ast, var_ast, str_ast, node_ast } tag;
    union {
        int           integer;
        double        real;
        char*         variable;
        char*         string;
        struct {
            ast_kind          tag;
            struct ast_list*  arguments;
        } node;
    } info;

    int first_line, first_column, last_line, last_column;
} ast;


typedef struct ast_list { 
    ast*             elem;
    struct ast_list* next;
} ast_list;


/* create an integer AST leaf */
ast* mk_int ( const long x );


/* create a floating point AST leaf */
ast* mk_real ( const double x );


/* create an AST leaf for a name */
ast* mk_var ( const char* x );


/* create a string AST leaf */
ast* mk_str ( const char* x );


/* create an internal AST node */
// rewrite in pcat.y
//ast* mk_node ( const ast_kind tag, ast_list* args );


/* put an AST e in the beginning of the list of ASTs r */
ast_list* cons ( ast* e, ast_list* r );


/* the empty list of ASTs */
#define null NULL


/* size of an AST list */
short length ( ast_list* );

/* reverse the order of ASTs in an AST list */
ast_list* reverse ( ast_list* );


/* printing functions for ASTs */
void print_ast_list ( ast_list* r );

void print_ast ( ast* x );
