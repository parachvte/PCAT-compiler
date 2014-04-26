/********************************************************************************
*
* File: ast.c
* The abstract syntax trees for the calculator
* Programmer: Leonidas Fegaras, UTA
* Date: 12/17/97
*
*
* Modified by Ryan, in order to ignore the gc lib.
*
********************************************************************************/


#include "ast.h"


ast* mk_int ( const long x ) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = int_ast;
    res->info.integer = x;
    return res;
};


ast* mk_real ( const double x ) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = real_ast;
    res->info.real = x;
    return res;
};


ast* mk_var ( const char* x ) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = var_ast;
    res->info.variable = (char*) malloc(strlen(x)+1);
    strcpy(res->info.variable,x);
    return res;
};


ast* mk_str ( const char* x ) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = str_ast;
    res->info.variable = (char*) malloc(strlen(x)+1);
    strcpy(res->info.variable,x);
    return res;
};

/* rewrite on pcat.y
ast* mk_node (const ast_kind tag, ast_list* args) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = node_ast;
    res->info.node.tag = tag;
    res->info.node.arguments = args;
    return res;
};*/


ast_list* cons ( ast* e, ast_list* r ) {
    ast_list* res = (ast_list*) malloc(sizeof(ast_list));
    res->elem = e;
    res->next = r;
    return res;
};


short length ( ast_list* r ) {
    short i = 0;
    for(; r != null; r=r->next) i++;
    return i;
};


ast_list* rev ( ast_list* r, ast_list* s ) {
    if (r == null)
        return s;
    return rev(r->next,cons(r->elem,s));
};


ast_list* reverse ( ast_list* r ) {
    return rev(r,null);
};


void print_ast_list ( ast_list* r ) {
    if (r == null)
        return;
    printf(" ");
    print_ast(r->elem);
    print_ast_list(r->next);
};


void print_ast ( ast* x ) {
    switch (x->tag) {
        case int_ast: printf("%d", x->info.integer); break;
        case real_ast: printf("%f", x->info.real); break;
        case var_ast: printf("%s", x->info.variable); break;
        case str_ast: printf("\"%s\"", x->info.string); break;
        case node_ast: {
                           printf("(%s",ast_names[x->info.node.tag]);
                           print_ast_list(x->info.node.arguments);
                           printf(")");
                           break;
                       };
    };
};
