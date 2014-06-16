/*
Return type of expression | AST NoType

1. No support for RECORD
2. Variables/procedures/types share the same name space.
   it's sanity has not been carefully verified.
3. Forward reference is not supported now....
*/

#include <stdlib.h>
#include <assert.h>

#include "ast.h"
#include "scope.h"
#include "routine.h"

/***************************************************************
                          Variables
***************************************************************/
ast *basic_int, *basic_real, *basic_bool, *basic_str;
ast *no_type, *need_infer, *void_type;

int error_count;

#define PRINT_REPR(k)           print_repr(pick_ast(x, k))

#define CHECK_K(k)              check(pick_ast(x, k))
#define CHECK_BY_NAME(k)        check(pick_ast_by_name(x, k))
#define CHECK(x)                check(x)

/***************************************************************
                    Print Representation
***************************************************************/
void print_repr(ast* x) {
    switch (x->tag){
        case int_ast:   break;
        case real_ast:  break;
        case var_ast:   printf("%s", ast_var(x)); break;
        case str_ast:   break;
        case node_ast: {
                switch (x->info.node.tag){
                    case Program:                       P("PROGRAM IS\n");break;
                    case Body:                          P("BEGIN\n");break;
                    case DeclarationBlock:              break;
                    case VariableDeclarationLine:       break;
                    case TypeDecs:                      break;
                    case ProcDecs:                      break;
                    case VariableDeclaration:           P("VAR");break;
                    case TypeDec:                       P("TYPE");break;
                    case ProcDec:                       P("PROCEDURE");break;
                    case NamedType:                     PRINT_REPR(0);break;
                    case ArrayType:                     P("ARRAY OF");break;
                    case RecordType:                    P("RECORD");break;
                    //case NoType:                      P("[No Type]");break;
                    case CompList:                      break;
                    case Comp:                          P(" : ");break;
                    case FormalParamList:               break;
                    case Param:                         P(" : ");break;
                    case AssignStatement:               P(" := ");break;
                    case CallStatement:                 P("(");break;
                    case ReadStatement:                 P("READ");break;
                    case WriteStatement:                P("WRITE");break;
                    case IfStatement:                   P("IF");break;
                    case WhileStatement:                P("WHILE");break;
                    case LoopStatement:                 P("LOOP");break;
                    case ForStatement:                  P("FOR");break;
                    case ExitStatement:                 P("EXIT");break;
                    case ReturnStatement:               P("RETURN");break;
                    case StatementBlock:                break;
                    case ExprList:                      break;
                    case BinOpExp:                      PRINT_REPR(0);break;
                    case UnOpExp:                       PRINT_REPR(0);break;
                    case LvalExp:                       break;
                    case CallExp:                       PRINT_REPR(0);break;
                    case RecordExp:                     break;
                    case ArrayExp:                      PRINT_REPR(0);break;
                    case IntConst:                      PRINT_REPR(0);break;
                    case RealConst:                     PRINT_REPR(0);break;
                    case StringConst:                   PRINT_REPR(0);break;
                    case RecordInitList:                break;
                    case RecordInit:                    break;
                    case ArrayInitList:                 break;
                    case ArrayInit:                     P("OF");break;
                    case LvalList:                      break;
                    case Var:                           PRINT_REPR(0);break;
                    case ArrayDeref:                    break;
                    case RecordDeref:                   break;
                    case Gt:                            P(">");break;
                    case Lt:                            P("<");break;
                    case Eq:                            P("=");break;
                    case Ge:                            P(">=");break;
                    case Le:                            P("<=");break;
                    case Ne:                            P("<>");break;
                    case Plus:                          P("+");break;
                    case Minus:                         P("-");break;
                    case Times:                         P("*");break;
                    case Slash:                         P("/");break;
                    case Divide:                        P(" div ");break;
                    case Module:                        P(" mod ");break;
                    case And:                           P(" and ");break;
                    case Or:                            P(" or ");break;
                    case UPlus:                         P("+");break;
                    case UMinus:                        P("-");break;
                    case Not:                           P(" not ");break;
                    case TypeInferNeeded:               break;
                    case VoidType:                      break;
                    case EmptyStatement:                break;
                    case EmptyExpression:               break;
                };
                break;
        };
        break;
    }
}

/***************************************************************
                    Print Error Info 
***************************************************************/
void error(ast* x, const char* err_msg) {
    error_count++;
    printf("ERROR: at line %d near \"", x->line_no);
    print_repr(x);
    printf("\" (");
    switch (x->tag) {
        case int_ast:   break;
        case real_ast:  break;
        case var_ast:   printf(" (%s)", ast_var(x));break;
        case str_ast:   break;
        case node_ast:  printf("%s", ast_names[tag(x)]);break;
    }
    printf("): %s\n", err_msg);
    exit(1);
}

/***************************************************************
                    Print Backtrace
***************************************************************/
void back_trace(ast* x) {
    //printf("Current at line %2d near \"",x->line_no);
    printf("at line %2d near \"", x->first_line);
    switch (x->tag) {
        case int_ast:   printf("%d", ast_int(x));break;
        case real_ast:  printf("%f", ast_real(x));break;
        case var_ast:   printf("%s", ast_var(x));break;
        case str_ast:   printf("%s", ast_str(x));break;
        case node_ast:  printf("%s", ast_names[tag(x)]);break;
    }
    printf("\"\n");
}

/***************************************************************
                    Check Main Entry
***************************************************************/
/* 
notes:
    for expr, return type
    for param, return type
    for var, return its decleration
    
*/

#define MAX_SCOPE_SIZE 998
ast* scope_return_type[MAX_SCOPE_SIZE];
int scope_offset[MAX_SCOPE_SIZE]; // offset
int scope_offset_top;
#define SCOPE_PUSH          scope_offset[scope_offset_top++] = 0
#define SCOPE_POP           scope_offset[--scope_offset_top] = 0
#define CURR_LOCAL_OFFSET   scope_offset[scope_offset_top - 1]
#define CURR_RETURN_TYPE    scope_return_type[scope_offset_top - 1]

/* local offset */
int _take_local_offset() {
    CURR_LOCAL_OFFSET -= 4;
    return CURR_LOCAL_OFFSET;
}
#define TAKE_LOCAL_OFFSET   _take_param_offset()

/* Parameter offset */
int param_offset;
int _take_param_offset() {
    param_offset += 4;
    return param_offset;
}
#define TAKE_PARAM_OFFSET   _take_param_offset() 

/** Check */
ast* check(ast* x) {
    //back_trace(x);
    
    ast* result = no_type;
    ast* decl;
    if (x == NULL) {
        printf("[!EMPTY!]\n");
        return result;
    }
    switch (x->tag) {
        case int_ast:
            result = basic_int;
            break;
        case real_ast:
            result = basic_real;
            break;
        case var_ast:
            decl = lookup(ast_var(x), NULL);
            if (decl) result = decl;
            break;
        case str_ast:
            result = basic_str;
            break;
        case node_ast : {
            ast_list* l;           // loop variable
            char* id;              // identifier
            ast *t0, *t1, *t2, *t3;   // temp
            int level;
            ast *nx;
            ast *ap,*fp;
            ast_list * lap,*lfp;
            ast_list *ail;
            ast *array_elem_type;
            switch (x->info.node.tag){
                case Program:
                    begin_scope();
                    SCOPE_PUSH;
                    CURR_RETURN_TYPE = void_type;
                    CHECK_BY_NAME("body");
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    SCOPE_POP; 
                    end_scope();
                    break;
                case Body:
                    CHECK_BY_NAME("declarations-list");
                    CHECK_BY_NAME("statements-list");
                    result=no_type; 
                    break;
                case DeclarationBlock:
                    // order:
                    // 1. type  () TODO: check forwrading...
                    // 2. var   (forwarding type)
                    // 3. proc  (forwarding type/var)
                    FOREACH(x) if (tag(ELEML)==TypeDecs) CHECK(ELEML);
                    FOREACH(x) if (tag(ELEML)==VariableDeclarationLine) CHECK(ELEML);
                    FOREACH(x) if (tag(ELEML)==ProcDecs) CHECK(ELEML);
                    break;
                case VariableDeclarationLine:
                    FOREACH(x) CHECK(ELEML);
                    break;
                case TypeDecs:
                    FOREACH(x) CHECK(ELEML);
                    break;
                case ProcDecs:
                    FOREACH(x) CHECK(ELEML);
                    break;
                case VariableDeclaration:
                    // append level/offset
                    append_ast(x,mk_int(CUR_LEVEL));
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));

                    // real works
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    t1 = CHECK_BY_NAME("type");
                    t2 = CHECK_BY_NAME("expression");
                    if (t1==need_infer)
                        if (t2==no_type)
                            error(x,"Cannot infer the type");
                        else
                        // set t1
                            x->info.node.arguments->next->elem = t2;
                    else
                        if (t2==no_type)
                            ;
                        else
                            if ( t1 != t2 )
                                error(x,"Type conflict");
                    decl = lookup(id,&level);
                    if ( decl && !(level<CUR_LEVEL) ){
                        error(x,"Name conflict");
                    }else
                        insert( id, x );

                    break;
                case TypeDec:
                    id = ast_var(pick_ast_by_name(x,"ID"));

                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,CHECK_BY_NAME("type"));
                    }else
                        error(x,"Name conflict");
                    break;
                case ProcDec:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    decl = lookup(id,&level);
                    if ( !decl ){
                        insert(id,x);
                    }else
                        error(x,"Name conflict");

                    t2 = CHECK_BY_NAME("type");  //check return type
                    begin_scope();
                    SCOPE_PUSH;
                    CURR_RETURN_TYPE = t2;
                    // append level
                    append_ast(x,mk_int(CUR_LEVEL));
                    CHECK_BY_NAME("formal-param-list");  //check formal list, adding to scope
                    CHECK_BY_NAME("body");  //check procedure body
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    SCOPE_POP;
                    end_scope();
                    break;
                case NamedType:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    if ( !strcmp(id,"INT") || !strcmp(id,"INTEGER") || !strcmp(id,"basic_int") )
                        result = basic_int;
                    else if ( !strcmp(id,"REAL") || !strcmp(id,"basic_real") )
                        result = basic_real;
                    else if ( !strcmp(id,"BOOLEAN") || !strcmp(id,"basic_bool") )
                        result = basic_bool;
                    else{
                        nx = lookup( id, NULL );
                        if (!nx)
                            error(nx,"Type Name not found");
                        result = nx;
                    }
                    break;
                case ArrayType:
                    t0 = CHECK_BY_NAME("type"); // check inner element's type
                    if ( t0 == no_type )
                        ;
                    else
                        result = x;
                    break;
                case RecordType:
                    // Not Implemented!
                    break;
                /*case NoType:
                    result = no_type;
                    break;*/
                case CompList:
                    // Not Implemented!
                    break;
                case Comp:
                    // Not Implemented!
                    break;
                case FormalParamList:
                    param_offset = 8;       // first(8) reserved for static links
                    FOREACH(x) CHECK(ELEML);   // check each param
                    break;
                case Param:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    decl = lookup(id,&level);
                    if ( decl && !(level < CUR_LEVEL ) ){
                        error(x,"Name conflict is not allowed");
                        printf("%d %d\n",level,CUR_LEVEL);
                    }else{
                        t1 = CHECK_K(1);
                        insert(id,x);
                        result = t1;
                    }
                    append_ast(x,mk_int(CUR_LEVEL));
                    append_ast(x,mk_int(TAKE_PARAM_OFFSET));
                    break;
                case AssignStatement:
                    t0 = CHECK_BY_NAME("lvalue");
                    t1 = CHECK_BY_NAME("expression");
                    if ( t0==no_type || t1 == no_type )
                        ;
                    else if ( t0 != t1 )
                        error(x,"Assgining between diffrent types is not allowed");   
                    break;
                case CallStatement:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = pick_ast_by_name(x,"expression-list");
                        fp = pick_ast_by_name(decl,"formal-param-list");
                        // lap -> list of ap
                        // lfp -> list of fp
                        lap = args(ap);
                        lfp = args(fp);
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = CHECK(ELEM(lap));
                            t1 = CHECK(pick_ast_by_name(ELEM(lfp),"type"));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                    }

                    append_ast(x,result);
                    append_ast(x,mk_int(CUR_LEVEL-pick_ast_by_name(decl,"level")->info.integer));
                    break;
                case ReadStatement:
                    FOREACH(pick_ast_by_name(x,"lvalue-list")){
                        t0 = CHECK(ELEML);

                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str )
                            error(x,"In READ statement only basic types (INT/REAL) are allowed");
                    }
                    break;
                case WriteStatement:
                    FOREACH(pick_ast_by_name(x,"expression-list")){
                        t0 = CHECK(ELEML);
                        if ( t0 != basic_int && 
                             t0 != basic_real &&
                             t0 != basic_str &&
                             t0 != basic_bool ){
                            error(x,"In WRITE statement only basic types (INT/REAL), STRING and BOOL are allowed");
                        }
                    }
                    break;
                case IfStatement:
                    t0 = CHECK_BY_NAME("expression");
                    if ( t0 != basic_bool )
                        error(x,"Condition in IF must be of BOOLEAN type");
                    CHECK_BY_NAME("statement");
                    CHECK_BY_NAME("statement-else");
                    break;
                case WhileStatement:
                    t0 = CHECK_BY_NAME("expression");
                    if ( t0 != basic_bool )
                        error(x,"Condition in WHILE must be of BOOLEAN type");
                    CHECK_BY_NAME("statement");
                    break;
                case LoopStatement:
                    CHECK_BY_NAME("statement");
                    break;
                case ForStatement:
                    // For I = a to b by c do e
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    t1 = CHECK_BY_NAME("expression-from");
                    t2 = CHECK_BY_NAME("expression-to");
                    t3 = CHECK_BY_NAME("expression-by");

                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Unknow index variable in FOR");
                    else if (t1 != basic_int)
                        error(x,"Start of range in FOR must be of INT type");
                    else if (t2 != basic_int)
                        error(x,"End of range in FOR must be of INT type");
                    else if (t3 != basic_int)
                        error(x,"Step of range in FOR must be of INT type");

                    CHECK_BY_NAME("statement");

                    append_ast(x,pick_ast_by_name(decl,"offset"));
                    break;
                case ExitStatement:
                    break;
                case ReturnStatement:
                    t1 = CHECK_BY_NAME("expression");
                    t2 = CURR_RETURN_TYPE;
                    if ( t1 != t2 )
                        error(x,"This type (maybe EMPTY-TYPE) conflicts with PROCEDURE declaration");
                    break;
                case StatementBlock:                    
                    FOREACH(x) CHECK(ELEML); 
                    break;
                case ExprList: 
                    FOREACH(x) CHECK(ELEML);
                    break;
                /*
                    For expression, return no-type if something wrong or some 
                        component is of no-type, which means I couldn't
                        handle the type of this expression.
                */
                case BinOpExp:
                    t1 = CHECK_BY_NAME("expression-left");
                    t2 = CHECK_BY_NAME("expression-right");

                    if ( t1==no_type || t2 == no_type )
                        ;
                    else if ( ( t1!=basic_int && t1!=basic_real && t1!=basic_bool ) ||
                              ( t2!=basic_int && t2!=basic_real && t2!=basic_bool ) )
                        error(x,"Non-basic type couldn't be used for binary operation");
                    else{
                        switch( tag(pick_ast_by_name(x,"binop")) ){
                            case Plus: case Minus: case Times:
                                if ( ( t1==basic_int || t1==basic_real ) &&
                                     ( t2==basic_int || t2==basic_real ) ){
                                    if ( t1==basic_int && t2==basic_int )
                                        result = basic_int;
                                    else
                                        result = basic_real;
                                }else
                                    error(x,"Binary arithmic operation expects INTEGER or REAL type on both sides");
                                break;
                            case Divide:
                            case Module:
                                if ( t1==basic_int && t2==basic_int )
                                    result = basic_int;
                                else
                                    error(x,"Binary arithmic operation expects INTEGER type on both sides");
                                break;
                            case Slash:
                                if ( ( t1==basic_int || t1==basic_real ) &&
                                     ( t2==basic_int || t2==basic_real ) )
                                    result = basic_real;
                                else
                                    error(x,"Binary arithmic operation expects INTEGER or type on both sides");
                                break;
                            case Gt: case Lt: case Eq: case Ge: case Le: case Ne:
                                if ( ( t1==basic_int || t1==basic_real ) &&
                                     ( t2==basic_int || t2==basic_real ) )
                                    result = basic_bool;
                                else
                                    error(x,"Binary Comparation operation expects INTEGER or REAL type on both sides");
                                break;
                            case And: case Or:
                                if ( t1==basic_bool && t2==basic_bool )
                                    result = basic_bool;
                                else
                                    error(x,"Binary boolean operation expects BOOLEAN type on both sides");
                                break;
                            default:
                                break;
                        }
                    }
                    
                    append_ast(x,result);
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case UnOpExp:
                    t1 = CHECK_BY_NAME("expression");                   
                    if ( t1==no_type )
                        ; 
                    else if ( t1!=basic_int && t1!=basic_real && t1!=basic_bool )
                        error(x,"Non-basic type couldn't be used for unary operation");
                    else{
                        switch( tag(pick_ast_by_name(x,"unop")) ){
                            case UPlus: case UMinus:
                                if ( t1==basic_int || t1==basic_real ){
                                    if ( t1==basic_int )
                                        result = basic_int;
                                    else
                                        result = basic_real;
                                }else
                                    error(x,"Unary arithmic operation expects INTEGER or REAL type");                      
                                break;
                            case Not:
                                if ( t1==basic_bool )
                                    result = basic_bool;
                                else
                                    error(x,"Unary boolean operation expects BOOLEAN type");
                                break;
                            default:
                                break;
                        }
                    }

                    append_ast(x,result);
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case LvalExp:
                    result = CHECK_BY_NAME("lvalue");
                    append_ast(x,result);
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case CallExp:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the called procedure");
                    else{
                        //ap -> actual parameters
                        //fp -> formal paramaters
                        ap = pick_ast_by_name(x,"expression-list");
                        fp = pick_ast_by_name(decl,"formal-param-list");
                        // lap -> list of ap
                        // lfp -> list of fp
                        lap = args(ap);
                        lfp = args(fp);
                        for(;lap && lfp; lap=lap->next, lfp=lfp->next ){
                            t0 = CHECK(ELEM(lap));
                            t1 = CHECK(pick_ast_by_name(ELEM(lfp),"type"));
                            if ( t0 != t1 )
                                error(x,"Formal and actual parameters don't match");
                        }
                        if ( lap && !lfp )
                            error(x,"Too many actual parameters");
                        if ( !lap && lfp )
                            error(x,"Need more actual parameters");
                        // type of procedure
                        result = CHECK( pick_ast_by_name(decl,"type") );
                    }

                    append_ast(x,result);
                    append_ast(x,mk_int(CUR_LEVEL-ast_int(pick_ast_by_name(decl,"level"))));
                    append_ast(x,mk_int(TAKE_LOCAL_OFFSET));
                    break;
                case RecordExp:
                    error(x,"RecordExp checking not implement");
                    break;
                case ArrayExp:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    decl = lookup(id,&level);
                    if ( !decl )
                        error(x,"Cannot find the ARRAY constructor's type");


                    array_elem_type = CHECK( pick_ast_by_name(decl,"type") );

                    ail = args(pick_ast_by_name(x,"array-init-list"));
                    for(;ail;ail=ail->next){
                        t0 = CHECK( pick_ast_by_name(ELEM(ail),"expression-count") );
                        t1 = CHECK( pick_ast_by_name(ELEM(ail),"expression-instance") );
                        if ( t0 != basic_int )
                            error(x,"Counter in ARRAY constructor must be of INT type");
                        if ( t1 != array_elem_type )
                            error(x,"Type in constructor and type in ARRAY definition don't match");
                    }

                    result = decl;
                    break;
                case IntConst:
                    result = basic_int;
                    append_ast(x,result);
                    break;
                case RealConst:
                    result = basic_real;
                    append_ast(x,result);
                    break;
                case StringConst:
                    result = basic_str;
                    append_ast(x,result);
                    break;
                case RecordInitList:
                    break;
                case RecordInit:
                    break;
                case ArrayInitList:
                    break;
                case ArrayInit:
                    break;
                case LvalList:
                    result = CHECK_K(0); // todo: right?
                    break;
                case Var:
                    id = ast_var(pick_ast_by_name(x,"ID"));
                    if ( !strcmp(id,"TRUE") || !strcmp(id,"FALSE") ){
                        result = basic_bool;
                        append_ast(x,result);
                    }else{
                        decl = lookup(id,&level);
                        if ( !decl )
                            error(x,"Unknow variable name");
                        else{
                            result = CHECK( pick_ast_by_name(decl,"type") );
                        }
                        append_ast(x,result);
                        append_ast(x,mk_int(CUR_LEVEL-ast_int(pick_ast_by_name(decl,"level"))));
                        append_ast(x,mk_int(ast_int(pick_ast_by_name(decl,"offset"))));
                    }

                    break;
                case ArrayDeref:
                    t0 = CHECK_BY_NAME("lvalue");
                    t1 = CHECK_BY_NAME("expression");

                    if ( t1 == no_type )
                        ;
                    else if (t1 != basic_int)
                        error(x,"Array index can only be of INT type");
                    else if (t0 == no_type)
                        ;
                    else if (t0->info.node.tag != ArrayType)
                        error(x,"Array dereference can only be apply to ARRAY type");
                    else
                        result = check( pick_ast_list(t0->info.node.arguments,0) );
                    break;
                case RecordDeref:
                    break;
                /* binary/unary operator wouldn't be used here */
                case Gt:
                case Lt:
                case Eq:
                case Ge:
                case Le:
                case Ne:
                case Plus:
                case Minus:
                case Times:
                case Slash:
                case Divide:
                case Module:
                case And:
                case Or:
                case UPlus:
                case UMinus:
                case Not:
                    assert(0);
                    break;
                case TypeInferNeeded:
                    result = need_infer;
                    break;
                case VoidType:
                    result = void_type;
                    break;
                case EmptyStatement:
                    break;
                case EmptyExpression:
                    result = void_type;
                    break;
            }
            break;
        }
    }
    return result;
}

/**
 * Initialize basic types/global variables
 */
void type_check_init() {
    basic_int  = mk_node(NamedType, cons(mk_var("basic_int"),  NULL));
    basic_real = mk_node(NamedType, cons(mk_var("basic_real"), NULL));
    basic_bool = mk_node(NamedType, cons(mk_var("basic_bool"), NULL));
    basic_str  = mk_node(NamedType, cons(mk_var("basic_str"),  NULL));
    need_infer = mk_node(NamedType, cons(mk_var("need_infer"), NULL));
    void_type  = mk_node(NamedType, cons(mk_var("void_type"),  NULL));
    no_type    = mk_node(NamedType, cons(mk_var("no_type"),    NULL));

    error_count = 0;
    scope_init();    
    scope_offset_top = 0; // @todo
}

/**
 * Interface for outside call
 */
int type_check(ast* x) {
    printf("====================Type Checking==================\n");
    type_check_init();
    check(x);
    if (error_count)
        printf("Type Checking ERROR\n");
    else
        printf("Type Checking SUCCESSFULLY\n");
    return error_count;
}
