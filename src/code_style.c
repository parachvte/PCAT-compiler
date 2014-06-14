/**
 * Code Style Generate
 */

#include "routine.h"

#define P                   printf
#define NEXT_OFFSET         (offset + 4)
#define FOREACH             for (l = args(x); l; l = l->next)
#define ELEM                l->elem

#define PO()                print_ast_code_style_offset(offset)

// prefix S means `select`, so `SPRINT` means `select and print`
#define SPRINT(k)           print_ast_code_style(pick_ast(x, k), offset);
#define SPRINT_NEXT(k)      print_ast_code_style(pick_ast(x, k), NEXT_OFFSET);
#define PRINT(t)            print_ast_code_style(t, offset);
#define PRINT_NEXT(t)       print_ast_code_style(t, NEXT_OFFSET);

#define EACHPRINT()         FOREACH PRINT(l->elem) 
#define EACHPRINT_NEXT()    FOREACH PRINT_NEXT(l->elem) 

#define SEPLIST(sep)        do {                    \
                                i = 0;              \
                                FOREACH {           \
                                    if (i > 0) {    \
                                        P(sep);     \
                                        P(" ");     \
                                    }               \
                                    PRINT(l->elem); \
                                    i++;            \
                                }                   \
                            } while (0)
#define RECORD_LINE()       do {                        \
                                x->line_no = line_no;   \
                            } while (0)


int line_no = 0;

void print_ast_code_style_offset(int offset) {
    printf("%04d|", ++line_no);
    for (int i = 0; i < offset; i++) P(" ");
}

void print_ast_code_style(ast* x, int offset) {
    if (x == NULL) {
        printf("[!EMPTY!]\n"); // should not reach here
        return;
    }
    RECORD_LINE();
    switch (x->tag) {
        case int_ast:
            printf("%d", ast_int(x));
            break;
        case real_ast:
            printf("%f", ast_real(x));
            break;
        case var_ast:
            printf("%s", ast_var(x));
            break;
        case str_ast:
            printf("\"%s\"", ast_str(x));
            break;
        case node_ast: {
            struct ast_list* l;
            int i;
            switch (tag(x)) {
                case Program:
                    printf("===================================================\n");
                    PO();RECORD_LINE();P("PROGRAM IS\n");
                    SPRINT(0);
                    printf("===================================================\n");
                    break;
                case Body:
                    SPRINT_NEXT(0);
                    PO();RECORD_LINE();P("BEGIN\n");
                    SPRINT_NEXT(1);
                    PO();P("END;\n");
                    break;
                case DeclarationBlock:
                    EACHPRINT();
                    break;
                case VariableDeclarationLine:
                case TypeDecs:
                case ProcDecs:
                    EACHPRINT();
                    break;
                case VariableDeclaration:
                    PO();RECORD_LINE();P("VAR ");SPRINT(0);P(" : ");SPRINT(1);P(" = ");SPRINT(2);P(";\n");
                    break;
                case TypeDec:
                    PO();RECORD_LINE();P("TYPE ");SPRINT(0);P(" is ");SPRINT(1);P(";\n");
                    break;
                case ProcDec:
                    PO();RECORD_LINE();P("PROCEDURE ");SPRINT(0);P(" (");SPRINT(1);P(") : ");SPRINT(2);P("\n");
                    SPRINT(3);
                    break;
                case NamedType:
                    SPRINT(0);
                    break;
                case ArrayType:
                    P("ARRAY OF ");SPRINT(0);
                    break;
                case RecordType:
                    P("RECORD\n");
                    SPRINT_NEXT(0);
                    PO();P("END;\n");
                    break;
                //case NoType:
                //    P("[No Type]");
                //    break;
                case CompList:
                    EACHPRINT_NEXT();
                    break;
                case Comp:
                    PO();SPRINT(0);P(" : ");SPRINT(1);P(";\n");
                    break;
                case FormalParamList:
                    SEPLIST(",");
                    break;
                case Param:
                    SPRINT(0);P(" : ");SPRINT(1);
                    break;
                case AssignStatement:
                    PO();RECORD_LINE();SPRINT(0);P(" := ");SPRINT(1);P(";\n");
                    break;
                case CallStatement:
                    PO();RECORD_LINE();SPRINT(0);P("(");SPRINT(1);P(")");P(";\n");
                    break;
                case ReadStatement:
                    PO();RECORD_LINE();P("READ(");SPRINT(0);P(")");P(";\n");
                    break;
                case WriteStatement:
                    PO();RECORD_LINE();P("WRITE(");SPRINT(0);P(")");P(";\n");
                    break;
                case IfStatement:
                    PO();RECORD_LINE();P("IF ");SPRINT(0);P(" THEN ");P("\n");
                    SPRINT_NEXT(1);
                    PO();P("ELSE");P("\n");
                    SPRINT_NEXT(2);
                    PO();P("END");P(";\n");
                    break;
                case WhileStatement:
                    PO();RECORD_LINE();P("WHILE ");SPRINT(0);P(" DO\n");
                    SPRINT_NEXT(1);
                    PO();P("END");P(";\n");
                    break;
                case LoopStatement:
                    PO();RECORD_LINE();P("LOOP ");P("\n");
                    SPRINT_NEXT(0);
                    PO();P("END");P(";\n");
                    break;
                case ForStatement:
                    PO();RECORD_LINE();P("FOR ");SPRINT(0);P(" := ");SPRINT(1);P(" TO ");SPRINT(2);P(" BY ");SPRINT(3);P(" DO");P("\n");
                    SPRINT_NEXT(4);
                    PO();P("END");P(";\n");
                    break;
                case ExitStatement:
                    PO();RECORD_LINE();P("EXIT");P(";\n");
                    break;
                case ReturnStatement:
                    PO();RECORD_LINE();P("RETURN ");SPRINT(0);P(";\n");
                    break;
                case StatementBlock:
                    EACHPRINT();
                    break;
                case ExprList:
                    SEPLIST(",");
                    break;
                case BinOpExp:
                    P("(");SPRINT(1);P(" ");SPRINT(0);P(" ");SPRINT(2);P(")");
                    break;
                case UnOpExp:
                    P("(");SPRINT(0);P(" ");SPRINT(1);P(")");                 
                    break;
                case LvalExp:
                    SPRINT(0);
                    break;
                case CallExp:
                    SPRINT(0);P("(");SPRINT(1);P(")");
                    break;
                case RecordExp:
                    SPRINT(0);P("{");SPRINT(1);P("}");
                    break;
                case ArrayExp:
                    SPRINT(0);P("[<");SPRINT(1);P(">]");
                    break;
                case IntConst:
                case RealConst:
                case StringConst:
                    SPRINT(0);
                    break;
                case RecordInitList:
                    SEPLIST(";");
                    break;
                case RecordInit:
                    SPRINT(0);P(" := ");SPRINT(1);
                    break;
                case ArrayInitList:
                    SEPLIST(",");
                    break;
                case ArrayInit:
                    SPRINT(0);P(" OF ");SPRINT(1);
                    break;
                case LvalList:
                    SEPLIST(",");
                    break;
                case Var:
                    SPRINT(0);
                    break;
                case ArrayDeref:
                    SPRINT(0);P("[");SPRINT(1);P("]");
                    break;
                case RecordDeref:
                    SPRINT(0);P(".");SPRINT(1);
                    break;
                case Gt:
                    P(">");
                    break;
                case Lt:
                    P("<");
                    break;
                case Eq:
                    P("=");
                    break;
                case Ge:
                    P(">=");
                    break;
                case Le:
                    P("<=");
                    break;
                case Ne:
                    P("<>");
                    break;
                case Plus:
                    P("+");
                    break;
                case Minus:
                    P("-");
                    break;
                case Times:
                    P("*");
                    break;
                case Slash:
                    P("/");
                    break;
                case Divide:
                    P(" div ");
                    break;
                case Module:
                    P(" mod ");
                    break;
                case And:
                    P(" and ");
                    break;
                case Or:
                    P(" or ");
                    break;
                case UPlus:
                    P("+");
                    break;
                case UMinus:
                    P("-");
                    break;
                case Not:
                    P(" not ");
                    break;
                case TypeInferNeeded:
                    P("[Type Infer Needed]");
                    break;
                case VoidType:
                    P("[Void]");
                    break;
                case EmptyStatement:
                    PO();RECORD_LINE();P("[Empty Statement]");P(";\n");
                    break;
                case EmptyExpression:
                    P("[Empty Expression]");
                    break;
            }
            break;
        }
    }
}
