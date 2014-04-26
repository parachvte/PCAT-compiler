%{

    // used in pcat.lex
    extern int yylineno;
    extern char *yytext;

    //#include "pcat.yy.c"
    #include "y.tab.h"
    #include "ast.h"

    /* parse error */
    void yyerror (char* s) {
        printf("*** %s (line : %d, token: %s)\n", s, yylineno, yytext);
    };


    ast* mk_node (const ast_kind tag, ast_list* args, YYLTYPE *yylloc);

%}

%union {
        char*           Tstring;
        struct ast*            Tast;
        struct ast_list*       Tast_list;
}

%token IDENTIFIER INTEGERT REALT STRINGT
       PROGRAM IS BEGINT END VAR TYPE PROCEDURE ARRAY RECORD
       IN OUT READ WRITE IF THEN ELSE ELSIF WHILE DO LOOP
       FOR EXIT RETURN TO BY AND OR NOT OF DIV MOD
       LPAREN  RPAREN LBRACKET RBRACKET LBRACE RBRACE COLON DOT
       SEMICOLON COMMA ASSIGN PLUS MINUS STAR SLASH BACKSLASH EQ
       NEQ LT LE GT GE LABRACKET RABRACKET EOFF ERROR

%type <Tast> program
%type <Tast> body

%%

start: program                  { print_ast($1); }
;
program: PROGRAM IS body SEMICOLON { $$ = mk_node(Program, cons($3, NULL), &@$); } /* @$: location of the whole grouping */
;
body: BEGINT END { $$ = mk_node(Body, NULL, &@$); }
;
/*
body:                 declaration_list BEGINT statement_list END { $$=mk_node(BodyDef,cons($1,cons($3,NULL)),&@$); }
;
declaration_list:     declaration_S { $$=mk_node(DeclareList,reverse($1),&@$);}
;
declaration_S:        declaration_S declaration { $$=cons($2,$1); }
                     | {$$=NULL;}
;
statement_list:       statement_S { $$=mk_node(SeqSt,reverse($1),&@$); }
;
statement_S:          statement_S statement { $$=cons($2,$1); }
                     | {$$=NULL;}
;
declaration:          VAR var_decl_S  { $$=mk_node(VarDecs,$2,&@$); }       // by using join, it's in right order, no need for reverse
                     |TYPE type_decl_S  { $$=mk_node(TypeDecs,reverse($2),&@$); }
                     |PROCEDURE procedure_decl_S { $$=mk_node(ProcDecs,reverse($2),&@$); }
;
var_decl_S:           var_decl_S var_decl { $$=join($1,$2); }
                     | {$$=NULL;}
;
type_decl_S:          type_decl_S type_decl { $$=cons($2,$1); }
                     | {$$=NULL;}
;
procedure_decl_S:     procedure_decl_S procedure_decl { $$=cons($2,$1); }
                     | {$$=NULL;}
;
var_decl:             identifier var_decl_id_S var_decl_type_O ASSIGN expression SEMICOLON
                            {
                                struct ast_list* id_list = cons($1,reverse($2)); $$=NULL; 
                                struct ast_list* var_list = NULL;
                                while( id_list != NULL ){
                                    var_list = cons( mk_node(VarDec,cons(id_list->elem,cons($3,cons($5,NULL))),&@$), var_list );
                                    id_list = id_list->next;
                                }
                                $$=reverse(var_list);
                            }
;
var_decl_id_S:        var_decl_id_S COMMA identifier { $$=cons($3,$1); }
                     | {$$=NULL;}
;
var_decl_type_O:      COLON typename { $$=$2; }
                     | {$$=mk_node(TypeInferenceNeeded,NULL,&@$);}
;
type_decl:            identifier IS type SEMICOLON { $$=mk_node(TypeDec,cons($1,cons($3,NULL)),&@$); }
;
procedure_decl:       identifier formal_params procedure_decl_type_O IS body SEMICOLON { $$=mk_node(ProcDec,cons($1,cons($2,cons($3,cons($5,NULL)))),&@$); }
;
procedure_decl_type_O: COLON typename  { $$=$2; }
                     | {$$=mk_node(VoidType,NULL,&@$);}
;
typename:             identifier { $$=mk_node(NamedTyp,cons($1,NULL),&@$); }
*/

%%

/* rewrite (unused) */
ast* mk_node (const ast_kind tag, ast_list* args, YYLTYPE *yylloc) {
    ast* res = (ast*) malloc(sizeof(ast));
    res->tag = node_ast;
    res->info.node.tag = tag;
    res->info.node.arguments = args;

    res->first_line = yylloc->first_line;
    return res;
};

