/**
 * Code Style Generate
 */

#include "routine.h"

#define next_offset     (offset + 4)
#define gopi(k)         print_ast_code_style(pick_ast(x, k), next_offset);
#define gop(k)          print_ast_code_style(pick_ast(x, k), offset);
#define goi(t)          print_ast_code_style(t, next_offset);
#define go(t)           print_ast_code_style(t, offset);
#define mo()            print_ast_code_style_offset(offset)
#define FOREACH         for (l = args(x); l; l = l->next)
#define ELEM            l->elem
#define EACHGO          FOREACH go(ELEM) 
#define EACHGOI         FOREACH goi(ELEM) 
#define p               printf
#define SEPLIST(sep)    do {                    \
                            i=0;                \
                            FOREACH {           \
                                if (i > 0) {    \
                                    p(sep);     \
                                    p(" ");     \
                                }               \
                                go(ELEM);       \
                                i += 1;         \
                            }                   \
                        } while (0)
#define record_line     do {                        \
                            x->line_no = line_no;   \
                        } while (0)


int line_no = 0;

void print_ast_code_style_offset(int offset) {
    printf("%03d |", ++line_no);
    for (int i = 0; i < offset; i++) p(" ");
}

void print_ast_code_style(ast* x, int offset) {
    if (x == NULL) {
        printf("[!EMPTY!]\n"); // should not reach here
        return;
    }
    record_line;
    switch (tag(x)) {
        case int_ast:  printf("%d", ast_int(x));  break;
        case real_ast: printf("%f", ast_real(x)); break;
        case var_ast:  printf("%s", ast_var(x));  break;
        case str_ast:  printf("\"%s\"", ast_str(x));  break;
        case node_ast: {
            struct ast_list* l;
            int i;
            switch (tag(x)) {
                case Program:
                    printf("====|==============================================\n");
                    mo();record_line;p("PROGRAM IS\n");
                    gop(0);
                    printf("====|==============================================\n");
                    break;
                case Body:
                    gopi(0);
                    mo();record_line;p("BEGIN\n");
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
                    mo();record_line;p("VAR ");gop(0);p(" : ");gop(1);p(" = ");gop(2);p(";\n");
                    break;
                case TypeDec:
                    mo();record_line;p("TYPE ");gop(0);p(" is ");gop(1);p(";\n");
                    break;
                case ProcDec:
                    mo();record_line;p("PROCEDURE ");gop(0);p(" (");gop(1);p(") : ");gop(2);p("\n");
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
                    mo();record_line;gop(0);p(" := ");gop(1);p(";\n");
                    break;
                case CallStatement:
                    mo();record_line;gop(0);p("(");gop(1);p(")");p(";\n");
                    break;
                case ReadStatement:
                    mo();record_line;p("READ(");gop(0);p(")");p(";\n");
                    break;
                case WriteStatement:
                    mo();record_line;p("WRITE(");gop(0);p(")");p(";\n");
                    break;
                case IfStatement:
                    mo();record_line;p("IF ");gop(0);p(" THEN ");p("\n");
                    gopi(1);
                    mo();p("ELSE");p("\n");
                    gopi(2);
                    mo();p("END");p(";\n");
                    break;
                case WhileStatement:
                    mo();record_line;p("WHILE ");gop(0);p(" DO\n");
                    gopi(1);
                    mo();p("END");p(";\n");
                    break;
                case LoopStatement:
                    mo();record_line;p("LOOP ");p("\n");
                    gopi(0);
                    mo();p("END");p(";\n");
                    break;
                case ForStatement:
                    mo();record_line;p("FOR ");gop(0);p(" := ");gop(1);p(" TO ");gop(2);p(" BY ");gop(3);p(" DO");p("\n");
                    gopi(4);
                    mo();p("END");p(";\n");
                    break;
                case ExitStatement:
                    mo();record_line;p("EXIT");p(";\n");
                    break;
                case ReturnStatement:
                    mo();record_line;p("RETURN ");gop(0);p(";\n");
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
                    mo();record_line;p("[Empty Statement]");p(";\n");
                    break;
                case EmptyExpression:
                    p("[Empty Expression]");
                    break;
            }
            break;
        };
    }
}

