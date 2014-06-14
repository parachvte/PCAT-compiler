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
#include <assert.h>

/*================================================
                Make AST Nodes
================================================*/
ast* mk_int(const long x) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = int_ast;
    res->info.integer = x;
    return res;
};


ast* mk_real(const double x) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = real_ast;
    res->info.real = x;
    return res;
};


ast* mk_var(const char* x) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = var_ast;
    res->info.variable = (char*) malloc(strlen(x)+1);
    strcpy(res->info.variable,x);
    return res;
};


ast* mk_str(const char* x) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = str_ast;
    res->info.variable = (char*) malloc(strlen(x)+1);
    strncpy(res->info.variable, x + 1, strlen(x) - 2);
    return res;
};

// which rewrite in pcat.y to my_mk_node()
ast* mk_node(const ast_kind tag, ast_list* args) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = node_ast;
    res->info.node.tag = tag;
    res->info.node.arguments = args;
    return res;
};

/*================================================
                Operations on AST 
================================================*/
ast_list* cons(ast* e, ast_list* r) {
    ast_list* res = (ast_list*) malloc(sizeof(ast_list));
    res->elem = e;
    res->next = r;
    return res;
};


short length(ast_list* r) {
    short i = 0;
    for ( ; r != null; r=r->next) i++;
    return i;
};


ast_list* rev(ast_list* r, ast_list* s) {
    if (r == null)
        return s;
    return rev(r->next, cons(r->elem, s));
};


ast_list* reverse(ast_list* r) {
    return rev(r, null);
};

ast_list* join(ast_list* a, ast_list* b) {
    ast_list* res = NULL;
    for ( ; a != NULL && a->elem != NULL; a = a->next)
        res = cons(a->elem, res);
    for ( ; b != NULL && b->elem != NULL; b = b->next)
        res = cons(b->elem, res);
    res = reverse(res);
    return res;
}

ast_list* append(ast_list* a, ast* b) {
    return reverse(cons(b, reverse(a)));
}

/*================================================
            Access AST properties
================================================*/
int tag(ast* a) {
    return a->info.node.tag;
}

int ast_real_repr(ast* a) {
    float fv = a->info.real;
    int iv = *((int *)(void *)(&fv));
    return iv;
}

ast_list* args(ast* a) {
    return a->info.node.arguments;
}

ast* pick_ast_list(ast_list* a, int k) {
    while (k--) a = a->next;
    return a->elem;
} 

ast* pick_ast(ast* a, int k) {
    return pick_ast_list(args(a), k);
}

ast* pick_ast_comp(ast* a, char* name) {
    return pick_ast(a, get_comp_id(a, name));
}

void append_ast(ast* a, ast* b) {
    a->info.node.arguments = append(a->info.node.arguments, b);
}

#define r(a) do{                \
    if (strcmp(name,a) == 0) {  \
        return b;               \
    }                           \
    b++;                        \
}while (0)

#define die assert(0)

int get_comp_id(ast* a, char* name) {
    assert(a->tag == node_ast);
    int b = 0;
    switch (tag(a)) {
        case Program:               r("body");r("local-offset"); die;
        case Body:                  r("declarations-list");r("statements-list"); die;
        case DeclarationBlock:
        case VariableDeclarationLine:
        case TypeDecs:
        case ProcDecs:              die;
        case VariableDeclaration:   r("ID");r("type");r("expression");r("level");r("offset"); die;
        case TypeDec:               r("ID");r("type"); die;
        case ProcDec:               r("ID");r("formal-param-list");r("type");r("body");r("level");r("local-offset"); die;
        case NamedType:             r("ID"); die;
        case ArrayType:             r("type"); die;
        case RecordType:            r("component-list"); die;
        //case NoType:
        case CompList:              die;
        case Comp:                  r("ID");r("type"); die;
        case FormalParamList:       die;
        case Param:                 r("ID");r("type");r("level");r("offset");die;
        case StatementBlock:        die;
        case AssignStatement:       r("lvalue");r("expression"); die;
        case CallStatement:         r("ID");r("expression-list");r("type");r("level-diff"); die;
        case ReadStatement:         r("lvalue-list"); die;
        case WriteStatement:        r("expression-list"); die;
        case IfStatement:           r("expression");r("statement");r("statement-else"); die;
        case WhileStatement:        r("expression");r("statement"); die;
        case LoopStatement:         r("statement"); die;
        case ForStatement:          r("ID");r("expression-from");r("expression-to");r("expression-by");r("statement");r("offset"); die;
        case ExitStatement:         die;
        case ReturnStatement:       r("expression"); die;
        case ExprList:              die;
        case BinOpExp:              r("binop");r("expression-left");r("expression-right");r("type");r("offset"); die;
        case UnOpExp:               r("unop");r("expression");r("type");r("offset"); die;
        case LvalExp:               r("lvalue");r("type");r("offset"); die;
        case CallExp:               r("ID");r("expression-list");r("type");r("level-diff");r("offset"); die;
        case RecordExp:             r("ID");r("record-init-list"); die;
        case ArrayExp:              r("ID");r("array-init-list"); die;
        case IntConst:              r("INTEGER");r("type"); die;
        case RealConst:             r("REAL");r("type"); die;
        case StringConst:           r("STRING");r("type"); die;
        case RecordInitList:        die;
        case RecordInit:            r("ID");r("expression"); die;
        case ArrayInitList:         die;
        case ArrayInit:             r("expression-count");r("expression-instance"); die;
        case LvalList:              die;
        case Var:                   r("ID");r("type");r("level-diff");r("offset"); die;
        case ArrayDeref:            r("lvalue");r("expression"); die;
        case RecordDeref:           r("lvalue");r("ID"); die;
        default:                    die;
    }
}

/*================================================
                AST Printing 
================================================*/
ast_list *rstack[100];
FILE *tree_file;

void print_ast_init() {
    tree_file = fopen("ast.out", "w");
}

void print_ast_finish() {
    fclose(tree_file);
}

void print_offset(int offset) {
    for (int i = 1; i <= offset; i++) {
        if (!rstack[i]) {
            fprintf(tree_file, "    ");
        } else
        if (i == offset) {
            fprintf(tree_file, "*---");
        } else {
            fprintf(tree_file, "|   ");
        }
    }
}

void print_ast_list(ast_list* r, int offset) { // print ast use
    if (r == null)
        return;
    print_ast(r->elem, offset);
    print_ast_list(r->next, offset);
};


void print_ast(ast* x, int offset) {
    if (offset) {
        print_offset(offset);
        rstack[offset] = rstack[offset]->next;
    }
    switch (x->tag) {
        case int_ast: fprintf(tree_file, "Integer(%d)\n", x->info.integer); break;
        case real_ast: fprintf(tree_file, "Real(%f)\n", x->info.real); break;
        case var_ast: fprintf(tree_file, "Var(%s)\n", x->info.variable); break;
        case str_ast: fprintf(tree_file, "String(%s)\n", x->info.string); break;
        case node_ast: {
            fprintf(tree_file, "%s [%d:%d-%d:%d]\n", ast_names[x->info.node.tag], x->first_line, x->first_column, x->last_line, x->last_column);
            rstack[offset + 1] = x->info.node.arguments;
            print_ast_list(x->info.node.arguments, offset + 1);
            break;
        };
    };
};
