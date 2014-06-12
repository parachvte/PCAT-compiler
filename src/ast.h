/********************************************************************************
 *
 * File: ast.h
 * The abstract syntax trees for the calculator
 * Programmer: Leonidas Fegaras, UTA
 * Date: 12/17/97
 *
 ********************************************************************************/
#ifndef _AST_H_
#define _AST_H_

#include "malloc.h"
#include <string.h>
#include <stdio.h>


/* Put the names of all the different kinds of ASTs here */

typedef enum {
    Program, Body, // 1. Program
    DeclarationBlock, VariableDeclarationLine, TypeDecs, ProcDecs, // 2. Declaration
    VariableDeclaration, TypeInferNeeded, Var, // 3. Variables
    TypeDec, NamedType, ArrayType, RecordType, CompList, Comp, // 4. Types
    ProcDec, VoidType, FormalParamList, Param, // 5. Procedure
    LvalList, ArrayDeref, RecordDeref,  // 6. L-Value
    StatementBlock, AssignStatement, CallStatement, // 7. Statement
    ReadStatement, WriteStatement, IfStatement, WhileStatement, LoopStatement, 
    ForStatement, ExitStatement, ReturnStatement, EmptyStatement,
    ExprList, EmptyExpression, LvalExp, ArrayExp, // 8. Expression
    BinOpExp, UnOpExp, CallExp, RecordExp, 
    RecordInitList, RecordInit, ArrayInitList, ArrayInit, 
    Gt, Lt, Eq, 
    Ge, Le, Ne, Plus, Minus, 
    Times, Slash, Divide, Module, And,
    Or, UPlus, UMinus, Not, 
    IntConst, RealConst, StringConst,
/*
    NoType,
    StList, 
*/
} ast_kind;

static char* ast_names[] = {
    "Program", "Body", // 1
    "Declaration Block", "Variable Declaration Line", "Type Declaration List", "Procedure Declaration List",  // 2
    "Varible Declaration", "Type Infer Needed", "Var", // 3
    "Type Declaration", "Named Type", "Array Type", "Record Type", "Component List", "Component", // 4
    "Procedure Declaration", "Void Type", "Formal Parameter List", "Parameter", // 5
    "L-Value List", "Array Deref", "Record Deref", // 6
    "Statement Block", "Assign Statement", "Call Statement", // 7 
    "Read Statement", "Write Statement", "If Statement", "While Statement", "Loop Statement", 
    "For Statement", "Exit Statement", "Return Statement", "Empty Statement",
    "ExprList", "Empty Expression", "L-Value Expression", "Array Expression", // 8
    "Binary Operation Expresssion", "Unary Operation Expression", "Call Expression", "Record Expression", 
    "Record Init List", "Record Init", "Array Init List", "Array Init", 
    "Gt", "Lt", "Eq", 
    "Ge", "Le", "Ne", "Plus", "Minus", 
    "Times", "Slash", "Divide", "Module", "And",
    "Or", "UPlus", "UMinus", "Not",
    "Integer Const", "Real Const", "String Const",
/* 
    "No Type", 
    "Statement List", 
*/
};


/* This is a universal data structure that can capture any AST:
 * The node is an internal node and has a list of children (other ASTs), * while the other nodes are leaves       */

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
    int line_no; // the same as first_line, but set when in code style generation
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
// ast* mk_node ( const ast_kind tag, ast_list* args ); // rewrited
extern ast* mk_node();


/* put an AST e in the beginning of the list of ASTs r */
ast_list* cons ( ast* e, ast_list* r );


/* the empty list of ASTs */
#define null NULL

/* size of an AST list */
short length ( ast_list* );

/* reverse the order of ASTs in an AST list */
ast_list* reverse ( ast_list* );

/* join two AST lists */
ast_list* join(ast_list *, ast_list *);

/* append */
ast_list* append(ast_list* r, ast* e);


/* Access */
int tag(ast* a);
int ast_real_repr(ast *a);

#define ast_int(a) (a->info.integer)
#define ast_var(a) (a->info.variable)
#define ast_str(a) (a->info.string)


ast_list* args(ast* a);
ast* pick_ast_list(ast_list* a, int k);
ast* pick_ast(ast* a, int k);
ast* pick_ast_comp(ast* a, char* name);
void append_ast(ast* a, ast* b);
int get_comp_id(ast* a, char* name);


/* printing functions for ASTs */
void print_ast_list(ast_list* r, int offset);

void print_ast(ast* x, int offset);

#endif
