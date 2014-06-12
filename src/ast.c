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
    strncpy(res->info.variable, x + 1, strlen(x) - 2);
    return res;
};

// rewrite pcat.y
ast* mk_node (const ast_kind tag, ast_list* args) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = node_ast;
    res->info.node.tag = tag;
    res->info.node.arguments = args;
    return res;
};


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


ast_list* reverse( ast_list* r ) {
    return rev(r,null);
};

ast_list* join(ast_list* a, ast_list* b) {
    ast_list* res = NULL;
    for(; a != NULL && a->elem != NULL; a = a->next)
        res = cons(a->elem, res);
    for(; b != NULL && b->elem != NULL; b = b->next)
        res = cons(b->elem, res);
    res = reverse(res);
    return res;
}

ast_list* append(ast_list* a, ast* b) {
    return reverse(cons(b, reverse(a)));
}

/* Access */
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
int get_comp_id(ast* a, char* name) {
    assert( a->tag == node_ast );
    int b = 0;
    #define r(a) do{ if(strcmp(name,a)==0){return b;}; b++; }while(0)
    #define die assert(0)
    switch ( tag(a) ){
        case Program:   r("body");r("local-offset"); die;
        case Body:   r("declarations-list");r("statements-list"); die;
        case DeclarationBlock:
        case VariableDeclarationLine:
        case TypeDecs:
        case ProcDecs:
            die;
        case VariableDeclaration:    r("ID");r("type");r("expression");r("level");r("offset"); die;
        case TypeDec:   r("ID");r("type"); die;
        case ProcDec:   r("ID");r("formal-param-list");r("type");r("body");r("level");r("local-offset"); die;
        case NamedType:  r("ID"); die;
        case ArrayType:  r("type"); die;
        case RecordType: r("component-list"); die;
        //case NoType:
        case CompList: die;
        case Comp:      r("ID");r("type"); die;
        case FormalParamList: die;
        case Param:     r("ID");r("type");r("level");r("offset");die;
        case StatementBlock:     die;
        case AssignStatement:  r("lvalue");r("expression"); die;
        case CallStatement:    r("ID");r("expression-list");r("type");r("level-diff"); die;
        case ReadStatement:    r("lvalue-list"); die;
        case WriteStatement:   r("expression-list"); die;
        case IfStatement:      r("expression");r("statement");r("statement-else"); die;
        case WhileStatement:   r("expression");r("statement"); die;
        case LoopStatement:    r("statement"); die;
        case ForStatement:     r("ID");r("expression-from");r("expression-to");r("expression-by");r("statement");r("offset"); die;
        case ExitStatement:    die;
        case ReturnStatement:     r("expression"); die;
        case ExprList:  die;
        case BinOpExp:  r("binop");r("expression-left");r("expression-right");r("type");r("offset"); die;
        case UnOpExp:   r("unop");r("expression");r("type");r("offset"); die;
        case LvalExp:   r("lvalue");r("type");r("offset"); die;
        case CallExp:   r("ID");r("expression-list");r("type");r("level-diff");r("offset"); die;
        case RecordExp: r("ID");r("record-init-list"); die;
        case ArrayExp:  r("ID");r("array-init-list"); die;
        case IntConst:  r("INTEGER");r("type"); die;
        case RealConst: r("REAL");r("type"); die;
        case StringConst: r("STRING");r("type"); die;
        case RecordInitList: die;
        case RecordInit:r("ID");r("expression"); die;
        case ArrayInitList: die;
        case ArrayInit: r("expression-count");r("expression-instance"); die;
        case LvalList:  die;
        case Var:       r("ID");r("type");r("level-diff");r("offset"); die;
        case ArrayDeref:r("lvalue");r("expression"); die;
        case RecordDeref:r("lvalue");r("ID"); die;
        default: die;
    }
}


/* Print */
ast_list *rstack[10];

void print_offset(int offset) {
    for (int i = 1; i <= offset; i++) {
        if (!rstack[i]) {
            printf("    ");
        } else
        if (i == offset) {
            printf("*---");
        } else {
            printf("|   ");
        }
    }
}

int print_line_no;

void make_offset(int offset) { // code style use
    print_line_no++;
    printf("%3d | ", print_line_no);
    
    int i;
    for (i = 0; i < offset; i++) putchar(' ');
}

void print_ast_list(ast_list* r, int offset) { // print ast use
    if (r == null)
        return;
    //print_offset(offset);
    print_ast(r->elem, offset);
    print_ast_list(r->next, offset);
};


void print_ast(ast* x, int offset) {
    if (offset) {
        print_offset(offset);
        rstack[offset] = rstack[offset]->next;
    }
    switch (x->tag) {
        case int_ast: printf("Integer(%d)\n", x->info.integer); break;
        case real_ast: printf("Real(%f)\n", x->info.real); break;
        case var_ast: printf("Var(%s)\n", x->info.variable); break;
        case str_ast: printf("String(%s)\n", x->info.string); break;
        case node_ast: {
            printf("%s [%d:%d-%d:%d]\n", ast_names[x->info.node.tag], x->first_line,
                x->first_column, x->last_line, x->last_column);
            rstack[offset + 1] = x->info.node.arguments;
            print_ast_list(x->info.node.arguments, offset + 1);
            break;
        };
    };
};


/* code style */

void print_ast_code_style(ast* x) {
    print_line_no = 0;
    _print_ast_code_style(x, 0);
}

void _print_ast_code_style(ast* x, int curr_offset) {
#define next_offset    (curr_offset+2)
#define gopi(k)        _print_ast_code_style( pick_ast(x,k), next_offset );
#define gop(k)         _print_ast_code_style( pick_ast(x,k), curr_offset );
#define goi(t)         _print_ast_code_style( t, next_offset );
#define go(t)          _print_ast_code_style( t, curr_offset );
#define mo()           make_offset(curr_offset)
#define FOREACH        for(l=args(x);l;l=l->next)
#define ELEM           l->elem
#define EACHGO         FOREACH go(ELEM) 
#define EACHGOI        FOREACH goi(ELEM) 
#define p              printf
#define SEPLIST(sep)   i=0; FOREACH{ if ( i > 0 ){p(sep);p(" ");}go(ELEM); i += 1; }     
#define recline        x->line_no = print_line_no
    if ( x == NULL )
        printf("[!EMPTY!]\n");
    else{
        recline;
        switch (x->tag){
            case int_ast:  printf("%d",x->info.integer); break;
            case real_ast: printf("%f",x->info.real); break;
            case var_ast:  printf("%s",x->info.variable); break;
            case str_ast:  printf("\"%s\"",x->info.string); break;
            case node_ast : {
                struct ast_list* l;
                int i;
                switch (x->info.node.tag){
                    case Program:
                        printf("====+==============================================\n");
                        mo();recline;p("PROGRAM IS\n");
                        gop(0);
                        printf("====+==============================================\n");
                        break;
                    case Body:
                        gopi(0);
                        mo();recline;p("BEGIN\n");
                        gopi(1);
                        mo();p("END;\n");
                        break;
                    case DeclarationBlock:
                        EACHGO;
                        break;
                    case VariableDeclarationLine:
                    case TypeDecs:
                    case ProcDecs:
                        EACHGO;
                        break;
                    case VariableDeclaration:
                        mo();recline;p("VAR ");gop(0);p(" : ");gop(1);p(" = ");gop(2);p(";\n");
                        break;
                    case TypeDec:
                        mo();recline;p("TYPE ");gop(0);p(" is ");gop(1);p(";\n");
                        break;
                    case ProcDec:
                        mo();recline;p("PROCEDURE ");gop(0);p(" (");gop(1);p(") : ");gop(2);p("\n");
                        gop(3);
                        break;
                    case NamedType:
                        gop(0);
                        break;
                    case ArrayType:
                        p("ARRAY OF ");gop(0);
                        break;
                    case RecordType:
                        p("RECORD\n");
                        gopi(0);
                        mo();p("END;\n");
                        break;
                    //case NoType:
                    //    p("[No Type]");
                    //    break;
                    case CompList:
                        EACHGOI;
                        break;
                    case Comp:
                        mo();gop(0);p(" : ");gop(1);p(";\n");
                        break;
                    case FormalParamList:
                        SEPLIST(",");
                        break;
                    case Param:
                        gop(0);p(" : ");gop(1);
                        break;
                    case AssignStatement:
                        mo();recline;gop(0);p(" := ");gop(1);p(";\n");
                        break;
                    case CallStatement:
                        mo();recline;gop(0);p("(");gop(1);p(")");p(";\n");
                        break;
                    case ReadStatement:
                        mo();recline;p("READ(");gop(0);p(")");p(";\n");
                        break;
                    case WriteStatement:
                        mo();recline;p("WRITE(");gop(0);p(")");p(";\n");
                        break;
                    case IfStatement:
                        mo();recline;p("IF ");gop(0);p(" THEN ");p("\n");
                        gopi(1);
                        mo();p("ELSE");p("\n");
                        gopi(2);
                        mo();p("END");p(";\n");
                        break;
                    case WhileStatement:
                        mo();recline;p("WHILE ");gop(0);p(" DO\n");
                        gopi(1);
                        mo();p("END");p(";\n");
                        break;
                    case LoopStatement:
                        mo();recline;p("LOOP ");p("\n");
                        gopi(0);
                        mo();p("END");p(";\n");
                        break;
                    case ForStatement:
                        mo();recline;p("FOR ");gop(0);p(" := ");gop(1);p(" TO ");gop(2);p(" BY ");gop(3);p(" DO");p("\n");
                        gopi(4);
                        mo();p("END");p(";\n");
                        break;
                    case ExitStatement:
                        mo();recline;p("EXIT");p(";\n");
                        break;
                    case ReturnStatement:
                        mo();recline;p("RETURN ");gop(0);p(";\n");
                        break;
                    case StatementBlock:
                        EACHGO;
                        break;
                    case ExprList:
                        SEPLIST(",");
                        break;
                    case BinOpExp:
                        p("(");gop(1);p(" ");gop(0);p(" ");gop(2);p(")");
                        break;
                    case UnOpExp:
                        p("(");gop(0);p(" ");gop(1);p(")");                 
                        break;
                    case LvalExp:
                        gop(0);
                        break;
                    case CallExp:
                        gop(0);p("(");gop(1);p(")");
                        break;
                    case RecordExp:
                        gop(0);p("{");gop(1);p("}");
                        break;
                    case ArrayExp:
                        gop(0);p("[<");gop(1);p(">]");
                        break;
                    case IntConst:
                    case RealConst:
                    case StringConst:
                        gop(0);
                        break;
                    case RecordInitList:
                        SEPLIST(";");
                        break;
                    case RecordInit:
                        gop(0);p(" := ");gop(1);
                        break;
                    case ArrayInitList:
                        SEPLIST(",");
                        break;
                    case ArrayInit:
                        gop(0);p(" OF ");gop(1);
                        break;
                    case LvalList:
                        SEPLIST(",");
                        break;
                    case Var:
                        gop(0);
                        break;
                    case ArrayDeref:
                        gop(0);p("[");gop(1);p("]");
                        break;
                    case RecordDeref:
                        gop(0);p(".");gop(1);
                        break;

                    case Gt:p(">");break;
                    case Lt:p("<");break;
                    case Eq:p("=");break;
                    case Ge:p(">=");break;
                    case Le:p("<=");break;
                    case Ne:p("<>");break;
                    case Plus:p("+");break;
                    case Minus:p("-");break;
                    case Times:p("*");break;
                    case Slash:p("/");break;
                    case Divide:p(" div ");break;
                    case Module:p(" mod ");break;
                    case And:p(" and ");break;
                    case Or:p(" or ");break;
                    case UPlus:p("+");break;
                    case UMinus:p("-");break;
                    case Not:p(" not ");break;

                    case TypeInferNeeded:
                        p("[Type Inference Needed]");
                        break;
                    case VoidType:
                        p("[Void Type]");
                        break;
                    case EmptyStatement:
                        mo();recline;p("[Empty Statement]");p(";\n");
                        break;
                    case EmptyExpression:
                        p("[Empty Expression]");
                        break;
                }
                break;
            };
        }
        // record the line number for current ast node
    }
}

