#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "routine.h"
#include "ast.h"


FILE *code_out, *data_out, *frame_out;

#define GEN_K(k)          gen(pick_ast(x, k))
#define GEN_BY_NAME(k)    gen(pick_ast_by_name(x, k))
#define GEN(x)            gen(x)

char *main_entry_name = "MainEntry";
char *routine_prefix = "";

/* Labels */
int label_count = 0;
char *make_label() {
    label_count++;

    char *p = malloc(30);
    sprintf(p, "label_%d", label_count);
    return p;
}

void load_int(ast* x, char* reg) {
    int level_diff, offset;
    int i;
    char* src = malloc(100);
    char* lc;
    switch (tag(x)) {
        case LvalExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(src, "%d(%%ebp)", offset);   
            fprintf(code_out, "\t movl %s, %s\n", src, reg);
            break;
        case Var:            
            if (!strcmp(ast_var(pick_ast(x,0)), "TRUE")) {
                src = "$1";
                level_diff = 0;
            } else if (!strcmp(ast_var(pick_ast(x, 0)), "FALSE")) {
                src = "$0";
                level_diff = 0;
            } else {
                offset = ast_int(pick_ast_by_name(x, "offset"));
                sprintf(src, "%d(%%ebp)", offset);                
                level_diff = ast_int(pick_ast_by_name(x, "level-diff")); 
                assert(level_diff >= 0);
            }

            if (level_diff == 0)
                fprintf(code_out, "\t movl %s, %s\n", src, reg);
            else{
                fprintf(code_out, "\t movl 8(%%ebp), %%edx\n");
                for (i = 0; i < level_diff - 1; i++)
                    fprintf(code_out, "\t movl 8(%%edx), %%edx\n");
                sprintf(src, "%d(%%edx)", offset);   
                fprintf(code_out, "\t movl %s, %s\n", src, reg);
            }
            break;
        case BinOpExp:
        case UnOpExp: 
        case CallExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(src, "%d(%%ebp)", offset);
            fprintf(code_out, "\t movl %s, %s\n", src, reg);
            break;
        case IntConst:
            fprintf(code_out, "\t movl $%d, %s\n", ast_int(pick_ast_by_name(x, "INTEGER")), reg);
            break;
        case RealConst:
            fprintf(code_out, "\t movl $%d, %s\n", ast_real_repr(pick_ast_by_name(x, "REAL")), reg);
            break;
        case StringConst:
            lc = make_label();
            fprintf(data_out, "\n"
                    "\t .section .rodata\n"
                    "%s:\n"
                    "\t .string \"%s\\0\"\n"
                    , lc, ast_str(pick_ast_by_name(x, "STRING")));
            fprintf(code_out, "\t movl $%s, %s\n", lc, reg);
            break;
        default:
            assert(0); // shouln't be here
            break;
    }
}

void store_int(char* reg, ast* x) {
    int level_diff, offset;
    int i;
    char* dst = malloc(100);
    switch (tag(x)) {
        case LvalExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(dst, "%d(%%ebp)", offset);   
            fprintf(code_out, "\t movl %s, %s\n", reg, dst);
            break;
        case Var:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(dst, "%d(%%ebp)", offset);            
            level_diff = ast_int(pick_ast_by_name(x, "level-diff")); 
            assert(level_diff >= 0);
            if (level_diff == 0)
                fprintf(code_out, "\t movl %s, %s\n", reg, dst);
            else{
                fprintf(code_out, "\t movl 8(%%ebp), %%edx\n");
                for (i = 0; i < level_diff-1; i++ )
                    fprintf(code_out, "\t movl 8(%%edx), %%edx\n");
                sprintf(dst, "%d(%%edx)", offset);   
                fprintf(code_out, "\t movl %s, %s\n", reg, dst);
            }
            break;
        case BinOpExp:
        case UnOpExp:
        case CallExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(dst, "%d(%%ebp)", offset);       
            fprintf(code_out, "\t movl %s, %s\n", reg, dst);
            break;
        default:
            assert(0); // shouln't be here
            break;
    }
}

void load_float(ast* x) {
    ast* t; // type
    int level_diff, offset;
    int i;
    char *opr, *opr_load_int = "fildl", *opr_load_float = "flds";
    char* src = malloc(100);
    char* lc;
    switch (tag(x)) {
        case LvalExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(src, "%d(%%ebp)", offset);
            opr = opr_load_float;
            fprintf(code_out, "\t %s %s\n", opr, src);
            break;
        case Var:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(src, "%d(%%ebp)", offset);             
            level_diff = ast_int(pick_ast_by_name(x, "level-diff")); 
            assert(level_diff >= 0);

            if (!strcmp(ast_str(pick_ast_by_name(x, "type")), "basic_int"))
                opr = opr_load_int;
            else if (!strcmp(ast_str(pick_ast_by_name(x, "type")), "basic_int"))
                opr = opr_load_float;
            else 
                assert(0);

            if (level_diff == 0)
                fprintf(code_out, "\t %s %s\n", opr, src);
            else{
                fprintf(code_out, "\t movl 8(%%ebp), %%edx\n");
                for(i = 0; i < level_diff - 1; i++)
                    fprintf(code_out, "\t movl 8(%%edx), %%edx\n");
                sprintf(src, "%d(%%edx)", offset);   
                fprintf(code_out, "\t %s %s\n", opr, src);
            }
            break;
        case BinOpExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            t = pick_ast_by_name(x, "type");
            if (!strcmp(ast_str(pick_ast_by_name(t, "ID")), "basic_int"))
                opr = opr_load_int;
            else if (!strcmp(ast_str(pick_ast_by_name(t, "ID")), "basic_real"))
                opr = opr_load_float;
            else
                assert(0);
            sprintf(src, "%d(%%ebp)", offset);
            fprintf(code_out, "\t %s %s\n", opr, src);
            break;
        case UnOpExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            t = pick_ast_by_name(x, "type");
            if (!strcmp(ast_str(pick_ast_by_name(t, "ID")), "basic_int"))
                opr = opr_load_int;
            else if (!strcmp(ast_str(pick_ast_by_name(t, "ID")), "basic_real"))
                opr = opr_load_float;
            else
                assert(0);
            sprintf(src, "%d(%%ebp)", offset);
            fprintf(code_out, "\t %s %s\n", opr, src);
            break;
        case CallExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            opr = opr_load_float;
            sprintf(src, "%d(%%ebp)", offset); 
            fprintf(code_out, "\t %s %s\n", opr, src);
            break;
        case IntConst:
            lc = make_label();
            fprintf(data_out, "\n"
                    "\t .section .rodata\n"
                    "\t .align 4 \n"
                    "%s:\n"
                    "\t .long %d\n", lc, ast_int(pick_ast(x, 0)));
            fprintf(code_out, "\t fildl %s\n", lc);
            break;
        case RealConst:
            lc = make_label();
            fprintf(data_out, "\n"
                    "\t .section .rodata\n"
                    "\t .align 4 \n"
                    "%s:\n"
                    "\t .long %d\n", lc, ast_real_repr(pick_ast(x, 0)));
            fprintf(code_out, "\t flds %s\n", lc);
            break;
        default:
            assert(0); // shouln't be here
            // other tag shouldn't call load_int
            break;
    }
}

void store_float(ast* x) {
    int level_diff, offset;
    int i;
    char *opr = "fstps";
    char *dst = malloc(100);
    switch(tag(x)) {
        case LvalExp:
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(dst, "%d(%%ebp)", offset);
            fprintf(code_out, "\t %s %s\n", opr, dst);
            break;
        case Var:
            level_diff = ast_int(pick_ast_by_name(x, "level-diff"));
            assert(level_diff >= 0);
            offset = ast_int(pick_ast_by_name(x, "offset"));
            sprintf(dst, "%d(%%ebp)", offset);
            if (level_diff == 0)
                fprintf(code_out, "\t %s %s\n", opr, dst);
            else {
                fprintf(code_out, "\t movl 8(%%ebp), %%edx\n");
                for(i = 0; i < level_diff - 1; i++)
                    fprintf(code_out, "\t movl 8(%%edx), %%edx\n");
                sprintf(dst, "%d(%%edx)", offset);
                fprintf(code_out, "\t %s %s\n", opr, dst);
            }
            break;
        case BinOpExp:
        case UnOpExp:
        case CallExp:
            if (tag(x) == BinOpExp) offset = ast_int(pick_ast(x, 4));
            if (tag(x) == UnOpExp)  offset = ast_int(pick_ast(x, 3));
            if (tag(x) == CallExp)  offset = ast_int(pick_ast(x, 3));
            fprintf(code_out, "\t %s %d(%%ebp)\n", opr, offset);
            break;
        case IntConst:
            fprintf(code_out, "\t fildl $%d\n", ast_int(pick_ast(x, 1)));
            break;
        case RealConst:
            fprintf(code_out, "\t flds $%d\n", ast_int(pick_ast(x, 1)));
        default:
            assert(0); // shouln't be here
            break;
    }
}

/** Before code gen */
void code_gen_init() {
    code_out = fopen("code.s", "w");
    data_out = fopen("data.s", "w");
    frame_out = fopen("frame.out", "w");
    label_count = 0;
}

/** After code gen */
void code_gen_finish() {
    fclose(code_out);
    fclose(data_out);
    fclose(frame_out);

    // Combination of code.s and data.s
    FILE *fi, *fo = fopen("pcat.s", "w");
    char ch;

    fi = fopen("code.s", "r");
    while ((ch = fgetc(fi)) != EOF) fputc(ch, fo);
    fi = fopen("data.s", "r");
    while ((ch = fgetc(fi)) != EOF) fputc(ch, fo);

    fclose(fi);
    fclose(fo);
}

// To generate intermediate code
void gen(ast* x) {
    if (x == NULL) return;
    switch (x->tag) {
        case int_ast:
        case real_ast:
        case var_ast:
        case str_ast:
            break;
        case node_ast: {
            ast_list *l, *le;
            ast *t,*t1,*t2;
            char *l2,*l3,*l4;
            int i;
            int level_diff;
            ast *var, *el;
            ast *fp;
            ast_list *lfp;
            int counter;
            switch (x->info.node.tag) {
                case Program: 
                    fprintf(code_out, "\t .text\n");
                    fprintf(code_out, "\t .globl %s%s\n", routine_prefix, main_entry_name);

                    fprintf(code_out, "\n");
                    fprintf(code_out, "%s%s:\n", routine_prefix, main_entry_name);  // name for main
                    fprintf(code_out, "\t pushl %%ebp\n\t movl %%esp, %%ebp\n");// prologue
                    fprintf(code_out, "\t subl $%d, %%esp\n", -ast_int(pick_ast_by_name(x, "local-offset"))); // local-offset is negative, so need a '-'
                    fprintf(code_out, "\t andl $-16, %%esp\n");
                    GEN_BY_NAME("body"); // body
                    fprintf(code_out, "\t leave\n\t ret\n");// epilogue
                    // sub-routine
                    FOREACH(pick_ast_by_name(pick_ast_by_name(x, "body"), "declarations-list"))
                        if (tag(ELEML) == ProcDecs) GEN(ELEML); 

                    fprintf(frame_out, "Frame for routine \"%s\"\n", "Main Routine");
                    fprintf(frame_out, "\tformal parameters:\n");
                    fprintf(frame_out, "\t\t(not applicable for Main Routine)\n");
                    fprintf(frame_out, "\tlocal variables:\n");
                    counter = 0;
                    FOREACH(pick_ast_by_name(pick_ast_by_name(x, "body"), "declarations-list"))
                        if (tag(ELEML) == VariableDeclaration)
                            for(le = args(ELEML); le; le = le->next) {
                                fprintf(frame_out, "\t\t%-20s @ (%%esp%d)\n",
                                        ast_str(pick_ast_by_name(ELEM(le), "ID")),
                                        ast_int(pick_ast_by_name(ELEM(le), "offset")));
                                counter++;
                            }
                    if (counter == 0)
                        fprintf(frame_out, "\t\t(No local variables)\n");
                    fprintf(frame_out, "\tframe size:\n");
                    fprintf(frame_out, "\t\tstack allocated = %d bytes\n", -ast_int(pick_ast_by_name(x, "local-offset")));
                    fprintf(frame_out, "\n");
                    break;
                case Body:
                    GEN_BY_NAME("declarations-list");
                    GEN_BY_NAME("statements-list"); 
                    break;
                case DeclarationBlock:
                    // subroutine
                    //FOREACH(x) if (tag(ELEML)==ProcDecs) GEN(ELEML);
                    FOREACH(x) if (tag(ELEML) == VariableDeclarationLine) GEN(ELEML);
                    break;
                case VariableDeclarationLine:
                    FOREACH(x) GEN(ELEML);
                    break;
                case TypeDecs:
                    // Shouldn't reach here
                    assert(0);
                    break;       
                case ProcDecs:
                    FOREACH(x) GEN(ELEML);
                    break;
                case VariableDeclaration:
                    var = mk_node(AssignStatement,
                                  cons(mk_node(Var,
                                               cons(pick_ast_by_name(x,"ID"),
                                                    cons(pick_ast_by_name(x,"type"),
                                                         cons(mk_int(0),
                                                              cons(pick_ast_by_name(x, "offset"), NULL))))),
                                       cons(pick_ast_by_name(x,"expression"), NULL)));
                    gen(var);
                    break;
                case TypeDec:    
                    // Shouldn't reach here
                    assert(0);            
                    break;
                case ProcDec:
                    fprintf(code_out, "\n");
                    fprintf(code_out, "%s%s:\n", routine_prefix, ast_str(pick_ast_by_name(x, "ID")));  
                    fprintf(code_out, "\t pushl %%ebp\n\t movl %%esp, %%ebp\n"); // prologue
                    fprintf(code_out, "\t subl $%d, %%esp\n", -ast_int(pick_ast_by_name(x, "local-offset"))); // local-offset is negative, so need a '-'
                    fprintf(code_out, "\t andl $-16, %%esp\n");
                    // name for sub-routine
                    GEN_BY_NAME("body"); // generate code for body   
                    fprintf(code_out, "\t leave\n\t ret\n");// epilogue 

                    // sub-routine
                    FOREACH(pick_ast_by_name(pick_ast_by_name(x,"body"),"declarations-list"))
                        if (tag(ELEML)==ProcDecs) GEN(ELEML); 


                    fprintf(frame_out, "Frame for routine \"%s\"\n", ast_str(pick_ast_by_name(x, "ID")));
                    fprintf(frame_out, "\tformal parameters:\n");
                    fprintf(frame_out, "\t\t%-20s @ (%%esp+8)\n", "[static link]");
                    fp = pick_ast_by_name(x, "formal-param-list");
                    lfp = args(fp);
                    for( ; lfp; lfp = lfp->next)
                        fprintf(frame_out, "\t\t%-20s @ (%%esp+%d)\n",
                                ast_str(pick_ast_by_name(ELEM(lfp), "ID")),
                                ast_int(pick_ast_by_name(ELEM(lfp), "offset")));
                    fprintf(frame_out, "\tlocal variables:\n");
                    counter = 0;
                    FOREACH(pick_ast_by_name(pick_ast_by_name(x, "body"), "declarations-list"))
                        if (tag(ELEML) == VariableDeclarationLine)
                            for (le = args(ELEML); le; le = le->next) {
                                fprintf(frame_out, "\t\t%-20s @ (%%esp%d)\n",
                                        ast_str(pick_ast_by_name(ELEM(le), "ID")),
                                        ast_int(pick_ast_by_name(ELEM(le), "offset")));
                                counter++;
                            }
                    if (counter == 0)
                        fprintf(frame_out, "\t\t(No local variables)\n");
                    fprintf(frame_out, "\tframe size:\n");
                    fprintf(frame_out, "\t\tstack allocated = %d bytes\n", -ast_int(pick_ast_by_name(x, "local-offset")));
                    fprintf(frame_out, "\n");
                    break;
                case NamedType:
                    // Shouldn't reach here
                    assert(0);
                    break;
                case ArrayType:
                    // Shouldn't reach here
                    assert(0);
                    break;
                case RecordType:
                    // Not Implemented!
                    // Shouldn't reach here
                    assert(0); 
                    break;
                //case NoType: 
                //    assert(0); 
                //    break;
                case CompList:
                    // Not Implemented!
                    // Shouldn't reach here
                    assert(0);
                    break;
                case Comp:
                    // Not Implemented!
                    // Shouldn't reach here
                    assert(0);
                    break;
                case FormalParamList:
                    // Shouldn't reach here
                    assert(0);
                    break;
                case Param:
                    // Shouldn't reach here
                    assert(0);
                    break;
                case AssignStatement:
                    GEN_BY_NAME("expression");
                    // this works for both INTEGER and REAL
                    load_int(pick_ast_by_name(x, "expression"), "%eax");
                    store_int("%eax", pick_ast_by_name(x, "lvalue"));
                    break;
                case CallStatement:
                    // synchronize with CallExp
                    FOREACH(pick_ast_by_name(x, "expression-list"))
                        gen(ELEML);
                    // parameters
                    el = pick_ast_by_name(x, "expression-list");
                    for(int i = length(args(el)) - 1; i >= 0; i--) {
                        load_int(pick_ast(el, i), "%eax");
                        fprintf(code_out, "\t pushl %%eax\n");
                    }
                    // static link
                    level_diff = ast_int(pick_ast_by_name(x, "level-diff"));
                    if (level_diff == -1)
                        fprintf(code_out, "\t pushl %%ebp\n");
                    else {
                        fprintf(code_out, "\t movl 8(%%ebp), %%edx\n");
                        for (i = 0; i < level_diff; i++)
                            fprintf(code_out, "\t movl 8(%%edx), %%edx\n");
                        fprintf(code_out, "\t pushl %%edx\n");
                    }
                    fprintf(code_out, "\t call %s%s\n", routine_prefix, ast_str(pick_ast_by_name(x, "ID")));
                    fprintf(code_out, "\t addl $%d, %%esp\n", 4 + 4 * length(args(pick_ast_by_name(x, "expression-list"))));
                    break;
                case ReadStatement:
                    FOREACH(pick_ast_by_name(x, "lvalue-list")) {
                        char* t_name = ast_str(pick_ast_by_name(pick_ast_by_name(l->elem, "type"), "ID"));
                        if (!strcmp(t_name, "basic_int"))
                            fprintf(code_out, "\t call %sread_int\n", routine_prefix);
                        else if (!strcmp(t_name, "basic_real"))
                            fprintf(code_out, "\t call %sread_real\n", routine_prefix);
                        else
                            assert(0);
                        store_int("%eax", l->elem);
                    }
                    break;
                case WriteStatement:
                    FOREACH(pick_ast_by_name(x, "expression-list")) {
                        GEN(l->elem);
                        load_int(l->elem, "%eax");
                        fprintf(code_out, "\t push %%eax\n");

                        char* t_name = ast_str(pick_ast_by_name(pick_ast_by_name(l->elem, "type"), "ID"));
                        if (!strcmp(t_name, "basic_int"))
                            fprintf(code_out, "\t call %sprint_int\n", routine_prefix);
                        else if (!strcmp(t_name, "basic_real"))
                            fprintf(code_out, "\t call %sprint_real\n", routine_prefix);
                        else if (!strcmp(t_name, "basic_str"))
                            fprintf(code_out, "\t call %sprint_str\n", routine_prefix);
                        else if (!strcmp(t_name, "basic_bool"))
                            fprintf(code_out, "\t call %sprint_bool\n", routine_prefix);
                        else
                            assert(0);

                        fprintf(code_out, "\t addl $4, %%esp\n");
                    }
                    fprintf(code_out, "\t call %sprint_line\n", routine_prefix);
                    break;
                case IfStatement:
                    /*
                        If A then B else C:

                        [get A]
                        cmpl $0,A
                        jne L2
                        C
                        jmp L3:
                        L2: 
                        B
                        L3:
                     */
                    l2 = make_label(); l3 = make_label();

                    GEN_BY_NAME("expression");
                    load_int(pick_ast_by_name(x, "expression"), "%eax");
                    fprintf(code_out, "\t cmpl $0, %%eax\n");
                    fprintf(code_out, "\t jne %s\n", l2);
                    GEN_BY_NAME("statement-else");
                    fprintf(code_out, "\t jmp %s\n", l3);
                    fprintf(code_out, "%s:\n", l2);
                    GEN_BY_NAME("statement");
                    fprintf(code_out, "%s:\n", l3);
                    break;
                case WhileStatement:
                    /*
                        while A do B

                        L2:
                        [get A]
                        cmpl $0,A
                        je L3
                        B
                        jmp L2;
                        L3:
                     */
                    l2 = make_label(); l3 = make_label();
                    fprintf(code_out, "%s:\n", l2);
                    GEN_BY_NAME("expression");
                    load_int(pick_ast(x, 0), "%eax");
                    fprintf(code_out, "\t cmpl $0, %%eax\n");
                    fprintf(code_out,"\t je %s\n", l3);
                    GEN_BY_NAME("statement");
                    fprintf(code_out, "\t jmp %s\n", l2);
                    fprintf(code_out, "%s:\n", l3);
                    break;
                case LoopStatement:
                    /*
                        Loop B:

                        L2:
                        B
                        jmp L2;
                     */
                    l2 = make_label();
                    fprintf(code_out, "%s:\n", l2);
                    GEN_BY_NAME("statement");
                    fprintf(code_out, "\t jmp %s\n", l2);
                    break;
                case ForStatement:
                    /*
                        For A := B to C by D do E:
                        ================================
                        A := B
                        while A <= C
                        E
                        A = A + E
                        ====================
                        [A := B]
                        L2:
                        [test A,C]
                        jg L3
                        E
                        [ A := A + D
                        jmp L2
                        L3;
                        ====================
                     */
                    l2 = make_label(); l3 = make_label();
                    var = mk_node(Var,
                                  cons(pick_ast_by_name(x,"ID"),
                                       cons(NULL,
                                            cons(mk_int(0),
                                                 cons(pick_ast_by_name(x, "offset"), NULL)))));

                    gen(mk_node(AssignStatement,
                                cons(var,
                                     cons(pick_ast_by_name(x, "expression-from"), NULL))));
                    fprintf(code_out, "%s:\n", l2);
                    GEN_BY_NAME("expression-to");
                    load_int(var, "%eax");
                    load_int(pick_ast_by_name(x, "expression-to"), "%ecx");
                    fprintf(code_out, "\t cmpl %%ecx, %%eax\n");
                    fprintf(code_out, "\t jg %s\n", l3);
                    GEN_BY_NAME("statement");
                    GEN_BY_NAME("expression-by");
                    load_int(var, "%eax");
                    load_int(pick_ast_by_name(x, "expression-by"), "%ecx");
                    fprintf(code_out, "\t addl %%ecx, %%eax\n");
                    store_int("%eax", var);
                    fprintf(code_out, "\t jmp %s\n", l2);
                    fprintf(code_out, "%s:\n", l3);
                    break;
                case ExitStatement:
                    fprintf(code_out, "\t leave\n\t ret\n"); //epilogue
                    break;
                case ReturnStatement:
                    if (tag(pick_ast_by_name(x, "expression")) == EmptyExpression);
                    else {
                        GEN_BY_NAME("expression");
                        load_int(pick_ast_by_name(x, "expression"), "%eax");
                    }
                    fprintf(code_out, "\t leave\n\t ret\n");// epilogue
                    break;
                case StatementBlock:
                    FOREACH(x) gen(ELEML);
                    break;
                case ExprList:
                    assert(0);
                    break;
                    /*
                       For expression, return no-type if something wrong or some
                       component is of no-type, which means I couldn't
                       handle the type of this expression.
                    */
                case BinOpExp:
                    // result type
                    t = pick_ast_by_name(x,"type");
                    t1 = pick_ast_by_name(pick_ast_by_name(x, "expression-left"), "type");
                    t2 = pick_ast_by_name(pick_ast_by_name(x, "expression-right"), "type");

                    if (tag(pick_ast_by_name(x, "binop")) == Plus ||
                        tag(pick_ast_by_name(x,"binop")) == Minus ||
                        tag(pick_ast_by_name(x,"binop")) == Times) {
                        // Arithmic, integer/real
                        // value of two sub-expr
                        GEN_K(1);
                        GEN_K(2);
                        if (!strcmp(ast_str(pick_ast(t, 0)), "basic_int")) {
                            load_int(pick_ast(x, 1), "%eax");
                            load_int(pick_ast(x, 2), "%ecx");

                            if (tag(pick_ast_by_name(x, "binop")) == Plus)
                                fprintf(code_out, "\t addl %%ecx, %%eax\n");
                            else if (tag(pick_ast_by_name(x, "binop")) == Minus) {
                                fprintf(code_out, "\t subl %%ecx, %%eax\n");
                            } else if (tag(pick_ast_by_name(x, "binop")) == Times)
                                fprintf(code_out, "\t imull %%ecx, %%eax\n");
                            else
                                assert(0); // shouldn't be here!
                            store_int("%eax", x);
                        } else if (!strcmp(ast_str(pick_ast(t, 0)), "basic_real")) {
                            load_float(pick_ast(x, 1));
                            load_float(pick_ast(x, 2));

                            if (tag(pick_ast_by_name(x, "binop")) == Plus)
                                fprintf(code_out, "\t faddp %%st, %%st(1)\n");
                            else if (tag(pick_ast_by_name(x, "binop")) == Minus) {
                                fprintf(code_out, "\t fsubrp %%st, %%st(1)\n");
                            } else if (tag(pick_ast_by_name(x, "binop")) == Times)
                                fprintf(code_out, "\t fmulp %%st, %%st(1)\n");
                            else
                                assert(0); // shouldn't be here!
                            store_float(x);
                        } else
                            assert(0); // shouldn't be here!
                    } else
                    if (tag(pick_ast_by_name(x, "binop")) == Divide ||
                        tag(pick_ast_by_name(x, "binop")) == Module) {
                        // Arithmic, integer
                        // value of two sub-expr
                        GEN_K(1);
                        GEN_K(2);
                        if (!strcmp(ast_str(pick_ast(t, 0)), "basic_int")) {
                            load_int(pick_ast(x, 1), "%eax");
                            load_int(pick_ast(x, 2), "%ecx");

                            if (tag(pick_ast_by_name(x, "binop")) == Divide) {
                                fprintf(code_out, "\t cltd\n");
                                fprintf(code_out, "\t idivl %%ecx\n");
                            } else if (tag(pick_ast_by_name(x, "binop")) == Module) {
                                fprintf(code_out, "\t cltd\n");
                                fprintf(code_out, "\t idivl %%ecx\n");
                                fprintf(code_out, "\t movl %%edx, %%eax\n");
                            } else
                                assert(0); // shouldn't be here!

                            store_int("%eax", x);
                        } else
                            assert(0); // shouldn't be here!
                    } else if (tag(pick_ast_by_name(x, "binop")) == Slash) {
                        // Arithmic, real
                        // value of two sub-expr
                        GEN_K(1);
                        GEN_K(2);
                        if (!strcmp(ast_str(pick_ast(t, 0)),"basic_real")) {
                            load_float(pick_ast(x, 1));
                            load_float(pick_ast(x, 2));

                            fprintf(code_out, "\t fdivrp %%st, %%st(1)\n");

                            store_float(x);
                        }else
                            assert(0); // shouldn't be here!
                    } else if (tag(pick_ast_by_name(x, "binop")) == Gt ||
                               tag(pick_ast_by_name(x, "binop")) == Lt ||
                               tag(pick_ast_by_name(x, "binop")) == Eq ||
                               tag(pick_ast_by_name(x, "binop")) == Ge ||
                               tag(pick_ast_by_name(x, "binop")) == Le ||
                               tag(pick_ast_by_name(x, "binop")) == Ne) {
                        // Comparation
                        // value of two sub-expr
                        GEN_K(1);
                        GEN_K(2);
                        if (!strcmp(ast_str(pick_ast(t1, 0)), "basic_int") &&
                                !strcmp(ast_str(pick_ast(t2, 0)), "basic_int")) {
                            load_int(pick_ast(x, 1), "%eax");
                            load_int(pick_ast(x, 2), "%ecx");

                            fprintf(code_out, "\t cmpl %%ecx, %%eax\n");
                            switch (tag(pick_ast_by_name(x, "binop"))) {
                                case Gt: fprintf(code_out, "\t setg %%al\n"); break;
                                case Lt: fprintf(code_out, "\t setl %%al\n"); break;
                                case Eq: fprintf(code_out, "\t sete %%al\n"); break;
                                case Ge: fprintf(code_out, "\t setge %%al\n"); break;
                                case Le: fprintf(code_out, "\t setle %%al\n"); break;
                                case Ne: fprintf(code_out, "\t setne %%al\n"); break;
                                default: break;
                            }
                            fprintf(code_out, "\t movzbl	%%al, %%eax\n");

                            store_int("%eax", x);
                        } else {
                            // order to push into float stack is essential (and wired) here!
                            load_float(pick_ast(x, 1));
                            load_float(pick_ast(x, 2));

                            fprintf(code_out, "\t fucomip %%st(1), %%st\n");
                            fprintf(code_out, "\t fstp %%st(0)\n");

                            switch (tag(pick_ast_by_name(x, "binop"))) {
                                case Gt: fprintf(code_out, "\t setg %%al\n"); break;
                                case Lt: fprintf(code_out, "\t setl %%al\n"); break;
                                case Eq: fprintf(code_out, "\t sete %%al\n"); break;
                                case Ge: fprintf(code_out, "\t setge %%al\n"); break;
                                case Le: fprintf(code_out, "\t setle %%al\n"); break;
                                case Ne: fprintf(code_out, "\t setne %%al\n"); break;
                                default: break;
                            }
                            fprintf(code_out, "\t movzbl	%%al, %%eax\n");

                            store_int("%eax", x);
                        }
                    } else if (tag(pick_ast_by_name(x, "binop")) == And ||
                               tag(pick_ast_by_name(x, "binop")) == Or) {
                        // Boolean operation: and / or
                        // value of two sub-expr
                        if (tag(pick_ast_by_name(x, "binop")) == And) {
                            l2 = make_label(); l3 = make_label();
                            GEN_K(1);
                            load_int(pick_ast(x, 1), "%eax");
                            fprintf(code_out, "\t cmpl $0, %%eax\n");
                            fprintf(code_out, "\t je %s\n",l2);
                            GEN_K(2);
                            load_int(pick_ast(x, 2), "%eax");
                            fprintf(code_out, "\t cmpl $0, %%eax\n");
                            fprintf(code_out, "\t je %s\n", l2);
                            fprintf(code_out, "\t movl $1, %%eax\n");
                            fprintf(code_out, "\t jmp %s\n", l3);
                            fprintf(code_out, "%s:\n", l2);
                            fprintf(code_out, "\t movl $0, %%eax\n");
                            fprintf(code_out, "%s:\n", l3);
                            store_int("%eax", x);
                        } else {
                            l2 = make_label(); l3 = make_label(); l4 = make_label();

                            GEN_K(1);
                            load_int(pick_ast(x, 1), "%eax");
                            fprintf(code_out, "\t cmpl $0, %%eax\n");
                            fprintf(code_out,"\t jne %s\n",l2);  
                            GEN_K(2);
                            load_int(pick_ast(x,2),"%eax");
                            fprintf(code_out,"\t cmpl $0, %%eax\n");
                            fprintf(code_out,"\t je %s\n",l3);
                            fprintf(code_out,"%s:\n",l2);
                            fprintf(code_out,"\t movl $1, %%eax\n");       
                            fprintf(code_out,"\t jmp %s\n",l4);                      
                            fprintf(code_out,"%s:\n",l3);       
                            fprintf(code_out,"\t movl $0, %%eax\n");                 
                            fprintf(code_out,"%s:\n",l4);
                            store_int("%eax",x);
                        }
                    }
                    break;
                case UnOpExp:                    
                    // result type
                    t = pick_ast_by_name(x,"type");
                    t1 = pick_ast_by_name(pick_ast_by_name(x,"expression"),"type");                 
                    GEN_K(1);
                    if ( tag(pick_ast_by_name(x,"unop")) == UPlus ||
                            tag(pick_ast_by_name(x,"unop")) == UMinus ){
                        if ( !strcmp(ast_str(pick_ast(t,0)),"basic_int") ){
                            load_int(pick_ast(x,1),"%eax");

                            if ( tag(pick_ast_by_name(x,"unop")) == UPlus )
                                ;
                            else if ( tag(pick_ast_by_name(x,"unop")) == UMinus )
                                fprintf(code_out,"\t negl %%eax\n");

                            store_int("%eax",x);
                        }else if ( !strcmp(ast_str(pick_ast(t,0)),"basic_real") ){
                            load_float(pick_ast(x,1));

                            if ( tag(pick_ast_by_name(x,"unop")) == UPlus )
                                ;
                            else if ( tag(pick_ast_by_name(x,"unop")) == UMinus )
                                fprintf(code_out,"\t fchs\n");

                            store_float(x);
                        }
                    }else if ( tag(pick_ast_by_name(x,"unop")) == Not ){
                        load_int(pick_ast(x,1),"%eax");
                        fprintf(code_out,"\t cmpl $0, %%eax\n");
                        fprintf(code_out,"\t sete %%al\n");
                        fprintf(code_out,"\t movzbl %%al, %%eax\n");
                        store_int("%eax",x);
                    }
                    break;
                case LvalExp:
                    load_int(pick_ast_by_name(x,"lvalue"),"%eax");
                    store_int("%eax",x);
                    break;
                case CallExp:                
                    // synchronize with CallSt
                    FOREACH(pick_ast_by_name(x,"expression-list"))
                        gen(ELEML);                    
                    // parameters
                    el = pick_ast_by_name(x,"expression-list");
                    for( int i = length(args(el))-1; i >= 0; i-- ){
                        load_int(pick_ast(el,i),"%eax");
                        fprintf(code_out,"\t pushl %%eax\n");
                    }
                    // static link
                    level_diff = ast_int( pick_ast_by_name(x,"level-diff") );
                    if ( level_diff == -1 )
                        fprintf(code_out,"\t pushl %%ebp\n");
                    else{
                        fprintf(code_out,"\t movl 8(%%ebp), %%edx\n");
                        for(i = 0; i < level_diff; i++ )
                            fprintf(code_out, "\t movl 8(%%edx), %%edx\n");
                        fprintf(code_out,"\t pushl %%edx\n");
                    }
                    fprintf(code_out,"\t call %s%s\n",routine_prefix,ast_str(pick_ast_by_name(x,"ID")));
                    fprintf(code_out,"\t addl $%d, %%esp\n",4+4*length(args(pick_ast_by_name(x,"expression-list"))));

                    // store return
                    store_int("%eax",x);
                    break;
                case RecordExp:
                    /*
                    // Not Implemented !!
                     */
                    break;
                case ArrayExp:   
                    assert(0); // shouln't be here
                    break;
                case IntConst:
                    // do nothing
                    break;
                case RealConst:
                    // do nothing
                    break;
                case StringConst:
                    // do nothing
                    break;
                case RecordInitList:
                    assert(0); // shouln't be here
                    break;
                case RecordInit:
                    assert(0); // shouln't be here
                    break;
                case ArrayInitList:
                    assert(0); // shouln't be here
                    break;
                case ArrayInit:
                    assert(0); // shouln't be here
                    break;
                case LvalList:
                    assert(0); // shouln't be here
                    break;
                case Var:                 
                    // do nothing
                    break;
                case ArrayDeref:
                    assert(0); // shouln't be here
                    /*
                    // Not Implemented !!
                     */                
                    break;
                case RecordDeref:
                    assert(0); // shouln't be here
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
                    assert(0); // shouln't be here      
                    break;

                case TypeInferNeeded:                
                    assert(0); // shouln't be here
                    break;
                case VoidType:                
                    assert(0); // shouln't be here
                    break;
                case EmptyStatement:
                    // do nothing
                    break;
                case EmptyExpression:
                    // do nothing
                    break;
            }
            break;
            }
    }
}

void code_gen(ast* x) {
    code_gen_init();
    gen(x);
    code_gen_finish();
}
