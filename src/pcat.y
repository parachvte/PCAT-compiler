%{

    #include <stdlib.h>

    #include "y.tab.h"
    #include "ast.h"
    #include "routine.h"

    extern int yylineno;
    extern char *yytext;

    //extern void routine(ast* prog);
    extern int yylex();
    /* parse error */
    void yyerror ( char* s ) {
        printf("\n*** %s (line : %d, token: '%s')\n", s, yylineno, yytext);
    };
    /* Rewrite */
    ast* my_mk_node(const ast_kind tag, ast_list* args, YYLTYPE *yylloc) {
        ast* res = (ast*) malloc(sizeof(ast));
        res->tag = node_ast;
        res->info.node.tag = tag;
        res->info.node.arguments = args;

        res->first_line = yylloc->first_line;
        res->first_column = yylloc->first_column;
        res->last_line = yylloc->last_line;
        res->last_column = yylloc->last_column;
        return res;
    };

%}

%union {
//  char*                  Tstring;
    struct ast*            Tast;
    struct ast_list*       Tast_list;
}

%token ID INTEGERT REALT STRINGT
       PROGRAM IS BEGINT END VAR TYPE PROCEDURE ARRAY RECORD
       IN OUT READ WRITE IF THEN ELSE ELSIF WHILE DO LOOP
       FOR EXIT RETURN TO BY AND OR NOT OF DIV MOD
       LPAREN  RPAREN LBRACKET RBRACKET LBRACE RBRACE COLON DOT
       SEMICOLON COMMA ASSIGN PLUS MINUS STAR SLASH BACKSLASH EQ
       NEQ LT LE GT GE LABRACKET RABRACKET EOFF ERROR NEG // vitual token


/* 1. Program */
%type <Tast> program
%type <Tast> body
/* 2. Declaration */
%type <Tast> declaration_block
%type <Tast_list> declaration_list
%type <Tast> declaration
/* 3. Variables */
%type <Tast_list> var_decl_list 
%type <Tast_list> var_decl
%type <Tast> var_decl_type 
/* 4. Types */
%type <Tast_list> type_decl_list
%type <Tast> type_decl
%type <Tast> typename
%type <Tast> type
%type <Tast_list> component_list
%type <Tast> component
/* 5. Procedure */
%type <Tast_list> procedure_decl_list
%type <Tast> procedure_decl
%type <Tast> procedure_decl_type
%type <Tast> formal_params
%type <Tast_list> fp_section_list
%type <Tast_list> fp_section
/* 6. L-value */
%type <Tast_list> lvalue_list
%type <Tast> lvalue
/* 7. Statement */
%type <Tast> statement_block
%type <Tast_list> statement_list
%type <Tast> statement
%type <Tast_list> statement_elsif_list
%type <Tast> statement_else
%type <Tast> statement_by
%type <Tast> actual_params
%type <Tast> write_params
%type <Tast_list> write_expr_list
%type <Tast> write_expr
/* 8. Expression */
%type <Tast_list> expression_list
%type <Tast> expression
%type <Tast_list> record_inits_list
%type <Tast> record_inits
%type <Tast_list> array_init_list
%type <Tast> array_inits
%type <Tast> array_init
%type <Tast> number
%type <Tast_list> identifier_list
%type <Tast> identifier
%type <Tast> string

// Precedences
// http://www.haskell.org/happy/doc/html/sec-Precedences.html
%nonassoc    ASSIGN
%left        OR
%left        AND
%nonassoc    EQ NEQ
%nonassoc    LT LE GT GE
%left        PLUS MINUS
%left        STAR DIV MOD
%left        NEG
%nonassoc    LBRACKET DOT


%%

start:
    program {
        //print_ast($1, 0);
        struct ast* prog = $1;
        routine(prog);
    }
;
/* 1. Program */
program:
    PROGRAM IS body SEMICOLON { $$ = my_mk_node(Program, cons($3, NULL), &@$); }
;
body:
    declaration_block BEGINT statement_block END { $$ = my_mk_node(Body, cons($1, cons($3, NULL)), &@$); }
;
/* 2. Declaration */
declaration_block:
    declaration_list { $$ = my_mk_node(DeclarationBlock, reverse($1), &@$);}
;
declaration_list:
    declaration_list declaration { $$ = cons($2, $1); }
|   { $$ = NULL; }
;
declaration:
    VAR var_decl_list  { $$ = my_mk_node(VariableDeclarationLine, $2, &@$); }
|   TYPE type_decl_list  { $$ = my_mk_node(TypeDecs, reverse($2), &@$); }
|   PROCEDURE procedure_decl_list { $$ = my_mk_node(ProcDecs, reverse($2), &@$); }
;
/* 3. Variables */
var_decl_list:
    var_decl_list var_decl { $$ = join($1, $2); }
|   { $$ = NULL; }
;
var_decl:
    identifier identifier_list var_decl_type ASSIGN expression SEMICOLON {
        struct ast_list* id_list = cons($1, reverse($2)); $$ = NULL; 
        struct ast_list* var_list = NULL;
        while (id_list !=  NULL) {
            var_list = cons(my_mk_node(VariableDeclaration, cons(id_list->elem, cons($3, cons($5, NULL))), &@$), var_list );
            id_list = id_list->next;
        }
        $$ = reverse(var_list);
    }
;
var_decl_type:
    COLON typename { $$ = $2; }
|   { $$ = my_mk_node(TypeInferNeeded, NULL, &@$); } /* spacial case */
;
/* 4. Types */
type_decl_list:
    type_decl_list type_decl { $$ = cons($2, $1); }
|   { $$ = NULL; }
;
type_decl:
    identifier IS type SEMICOLON { $$ = my_mk_node(TypeDec, cons($1, cons($3, NULL)), &@$); }
;
typename:
    identifier { $$ = my_mk_node(NamedType, cons($1, NULL), &@$); }
;
type:
    ARRAY OF typename { $$ = my_mk_node(ArrayType, cons($3, NULL), &@$); }
|   RECORD component component_list END { $$ = my_mk_node(RecordType, cons(my_mk_node(CompList, cons($2, reverse($3)), &@$), NULL), &@$); }
;
component_list:
    component_list component { $$ = cons($2, $1); }
|   { $$ = NULL; }
;
component:
    identifier COLON typename SEMICOLON { $$ = my_mk_node(Comp, cons($1, cons($3, NULL)), &@$); }
;
/* 5. Procedure */
procedure_decl_list:
    procedure_decl_list procedure_decl { $$ = cons($2, $1); }
|   { $$ = NULL; }
;
procedure_decl:
    identifier formal_params procedure_decl_type IS body SEMICOLON {
        $$ = my_mk_node(ProcDec, cons($1, cons($2, cons($3, cons($5, NULL)))), &@$);
    }
;
procedure_decl_type:
    COLON typename  { $$ = $2; }
|   { $$ = my_mk_node(VoidType, NULL, &@$); }
;
formal_params:
    LPAREN fp_section fp_section_list RPAREN   { $$ = my_mk_node(FormalParamList, join($2, $3), &@$); }
|   LPAREN RPAREN { $$ = my_mk_node(FormalParamList, NULL, &@$); }
;
fp_section_list:
    fp_section_list SEMICOLON fp_section  { $$ = join($1, $3); }
|   { $$ = NULL; }
;
fp_section:
    identifier identifier_list COLON typename {  
        struct ast_list* id_list = cons($1, reverse($2)); $$ = NULL; 
        struct ast_list* fp_list = NULL;
        while (id_list !=  NULL) {
            fp_list = cons(my_mk_node(Param, cons(id_list->elem, cons($4, NULL)), &@$), fp_list);
            id_list = id_list->next;
        }
        $$ = reverse(fp_list);
    }
;
/* 6. L-value */
lvalue_list:
    lvalue_list COMMA lvalue     { $$ = cons($3, $1); }       
|   { $$ = NULL; }
;
lvalue:
    identifier  { $$ = my_mk_node(Var, cons($1, NULL), &@$); }
|   lvalue LBRACKET expression RBRACKET  { $$ = my_mk_node(ArrayDeref, cons($1, cons($3, NULL)), &@$); }
|   lvalue DOT identifier   { $$ = my_mk_node(RecordDeref, cons($1, cons($3, NULL)), &@$); }
;
/* 7. Statement */
statement_block:
    statement_list { $$ = my_mk_node(StatementBlock, reverse($1), &@$); }
;
statement_list:
    statement_list statement { $$ = cons($2, $1); }
|   { $$ = NULL; }
;
statement:
    lvalue ASSIGN expression SEMICOLON  { $$ = my_mk_node(AssignStatement, cons($1, cons($3, NULL)), &@$); }
|   identifier actual_params SEMICOLON  { $$ = my_mk_node(CallStatement, cons($1, cons($2, NULL)), &@$); }
|   READ LPAREN lvalue lvalue_list RPAREN SEMICOLON    { $$ = my_mk_node(ReadStatement, cons($3, reverse($4)), &@$); }
|   WRITE write_params SEMICOLON        { $$ = my_mk_node(WriteStatement, cons($2, NULL), &@$); }
|   IF expression THEN statement_block statement_elsif_list statement_else END SEMICOLON {
        struct ast* if_ast = my_mk_node(IfStatement, cons($2, cons($4, cons(NULL, NULL))), &@$);
        struct ast* current_if = if_ast;
        struct ast_list* middle_list = reverse($5);
        for( ; middle_list != NULL; middle_list = middle_list->next) {
            current_if->info.node.arguments->next->next = cons(middle_list->elem, NULL);
            current_if = current_if->info.node.arguments->next->next->elem;
        }
        current_if->info.node.arguments->next->next = cons($6, NULL);
        $$ = if_ast;
    }
|   WHILE expression DO statement_block END SEMICOLON     { $$ = my_mk_node(WhileStatement, cons($2, cons($4, NULL)), &@$); }
|   LOOP statement_block END SEMICOLON   { $$ = my_mk_node(LoopStatement, cons($2, NULL), &@$); }
|   FOR identifier ASSIGN expression TO expression statement_by DO statement_block END SEMICOLON { 
        $$ = my_mk_node(ForStatement, cons($2, cons($4, cons($6, cons($7, cons($9, NULL))))), &@$);
    }
|   EXIT SEMICOLON                      { $$ = my_mk_node(ExitStatement, NULL, &@$); }
|   RETURN expression SEMICOLON       { $$ = my_mk_node(ReturnStatement, cons($2, NULL), &@$); }
;
statement_elsif_list:
    statement_elsif_list ELSIF expression THEN statement_block { 
        $$ = cons(my_mk_node(IfStatement, cons($3, cons($5, cons(NULL, NULL))), &@$), $1);
    }
|   { $$ = NULL; }
;
statement_else:
    ELSE statement_block { $$ = $2; }
|   { $$ = my_mk_node(EmptyStatement, NULL, &@$);}
;
statement_by:
    BY expression { $$ = $2; }
|   { $$ = my_mk_node(IntConst, cons(mk_int(1), NULL), &@$);}
;
actual_params:
    LPAREN expression expression_list RPAREN  { $$ = my_mk_node(ExprList, cons($2, reverse($3)), &@$); }
|   LPAREN RPAREN  { $$ = my_mk_node(ExprList, NULL, &@$); }
;
write_params:
    LPAREN write_expr write_expr_list RPAREN { $$ = my_mk_node(ExprList, cons($2, reverse($3)), &@$); }
|   LPAREN RPAREN { $$ = my_mk_node(EmptyExpression, NULL, &@$); }
;
write_expr_list:
    write_expr_list COMMA write_expr { $$ = cons($3, $1); } 
|   { $$ = NULL;}
;
write_expr:
    string  { $$ = $1; }
|   expression { $$ = $1; }
;
/* 8. Expression */
expression_list:
    expression_list COMMA expression  { $$ = cons($3, $1); }
|   { $$ = NULL; }
;
expression:
    number                         { $$ = $1; }
|   lvalue                         { $$ = my_mk_node(LvalExp, cons($1, NULL), &@$); }
|   LPAREN expression RPAREN       { $$ = $2; }
|   PLUS expression                { $$ = my_mk_node(UnOpExp, cons(my_mk_node(UPlus, NULL, &@$), cons($2, NULL)), &@$); }
|   MINUS expression %prec NEG     { $$ = my_mk_node(UnOpExp, cons(my_mk_node(UMinus, NULL, &@$), cons($2, NULL)), &@$); }
|   NOT expression                 { $$ = my_mk_node(UnOpExp, cons(my_mk_node(Not, NULL, &@$), cons($2, NULL)), &@$); }
|   expression PLUS expression     { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Plus, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression MINUS expression    { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Minus, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression STAR expression     { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Times, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression SLASH expression    { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Slash, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression DIV expression      { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Divide, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression MOD expression      { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Module, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression OR expression       { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Or, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression AND expression      { $$ = my_mk_node(BinOpExp, cons(my_mk_node(And, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression EQ expression       { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Eq, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression NEQ expression      { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Ne, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression LT expression       { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Lt, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression LE expression       { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Le, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression GT expression       { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Gt, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   expression GE expression       { $$ = my_mk_node(BinOpExp, cons(my_mk_node(Ge, NULL, &@$), cons($1, cons($3, NULL))), &@$); }
|   identifier actual_params       { $$ = my_mk_node(CallExp, cons($1, cons($2, NULL)), &@$); }
|   identifier record_inits        { $$ = my_mk_node(RecordExp, cons($1, cons($2, NULL)), &@$); }
|   identifier array_inits         { $$ = my_mk_node(ArrayExp, cons($1, cons($2, NULL)), &@$); }
;
record_inits_list:
    record_inits_list SEMICOLON identifier ASSIGN expression { $$ = cons(my_mk_node(RecordInit, cons($3, cons($5, NULL)), &@$), $1); }
|   { $$ = NULL; }
;
record_inits:
    LBRACE identifier ASSIGN expression record_inits_list RBRACE {
        $$ = my_mk_node(RecordInitList, cons(my_mk_node(RecordInit, cons($2, cons($4, NULL)), &@$), reverse($5)), &@$);
    }
;
array_inits:
    LABRACKET array_init array_init_list RABRACKET { $$ = my_mk_node(ArrayInitList, cons($2, reverse($3)), &@$); }
;
array_init_list:
    array_init_list COMMA array_init { $$ = cons($3, $1); }
|   { $$ = NULL; }
;
array_init:
    expression { $$ = my_mk_node(ArrayInit, cons($1, NULL), &@$); }
|   expression OF expression { $$ = my_mk_node(ArrayInit, cons($1, cons($3, NULL)), &@$); }
;
number:
    INTEGERT { $$ = my_mk_node(IntConst, cons(mk_int(atoi(yytext)), NULL), &@$); }
|   REALT { $$ = my_mk_node(RealConst, cons(mk_real(atof(yytext)), NULL), &@$); }
;
identifier_list:
    identifier_list COMMA identifier { $$ = cons($3, $1); }
|   { $$ = NULL; }
;
identifier:
    ID { $$ = mk_var(yytext); }
;
string:
    STRINGT { $$ = my_mk_node(StringConst, cons(mk_str(yytext), NULL), &@$); }
;

%%
