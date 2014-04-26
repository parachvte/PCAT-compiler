/****************************************************
*
* File  : pcat.lex
* Des   : PCAT lexer
* Author: Ryannnnnnn
*
****************************************************/

    /*================== Definition Section =================*/
%{
    #include <stdio.h>
    #include <string.h>

    #include "tokens.h"

    int columnno = 1;

    /* ============= Miscellaneous Macros =============*/
    #define YY_USER_ACTION yy_user_action();

    /* ============== Useful functions ==================
    Maybe move to another file later... by Ryan
    ===================================================*/

    extern void yyerror(char *);

    // user action prior to matched rule's action
    void yy_user_action() {
        columnno += yyleng;
    }

%}

/* Enable global variable yylineno */
%option yylineno
/* Makes the scanner not call yywrap() upon an end-of-file */
%option noyywrap


    /*=================== Regex Patterns ====================*/

    /* basics */
DIGIT [0-9]
LETTERS [a-zA-Z]
LETTERS_USCORE {LETTERS}|_
EXPONENT [Ee][+-]?{DIGIT}+
SPACES [ \t]

    /* matchs '(*' + ([not * not )] | ([not *]+ + ')') | ('*' + [not ')']+)) + '*)', too dirty... */
COMMENT \(\*([^\*\)]|([^\*]+\))|(\*[^\)]+))*\*\)

    /* matchs all reserved keywords */
KEYWORDS PROGRAM|IS|BEGIN|END|VAR|TYPE|PROCEDURE|ARRAY|RECORD|IN|OUT|READ|WRITE|IF|THEN|ELSE|ELSIF|WHILE|DO|LOOP|FOR|EXIT|RETURN|TO|BY|AND|OR|NOT|OF|DIV|MOD

    /* matchs all operators */
OPERATORS \(|\)|\[|\]|\{|\}|:|\.|;|,|:=|\+|\-|\*|\/|\\\\|=|<>|<|<=|>|>=|[<|>]

    /* matchs identifiers & basic types */
IDENTIFIER {LETTERS_USCORE}({LETTERS_USCORE}|{DIGIT})*
INTEGERT {DIGIT}+
REALT ({INTEGERT}\.{INTEGERT}?)|({INTEGERT}?\.{INTEGERT}){EXPONENT}?
STRINGT \"([^\n\"])*\" 


%%
    /*================== Rules Section =======================

    A series of rules of form: pattern action

    How the input is matched:
        http://flex.sourceforge.net/manual/Matching.html
    A match returns character pointer "yytext" and its length "yyleng".
    ========================================================*/

    /* Comments Start Condition */
{COMMENT} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Comment    : %s\n", yytext);
}
    /* Keywords */
{KEYWORDS} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Keyword    : %s\n", yytext);

    for (int i = 0; i < NUM_KEYWORD; i++)
        if (!strcmp(keywords[i], yytext)) {
            return IDENTIFIER + i;
        }
}
    /* Operators .. combine with Keywords? */
{OPERATORS} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Operator   : %s\n", yytext);
    for (int i = 0; i < NUM_KEYWORD; i++)
        if (!strcmp(keywords[i], yytext)) {
            return IDENTIFIER + i;
        }
}
    /* Indentifier & basic types */
{IDENTIFIER} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Identifier : %s\n", yytext);
    return IDENTIFIER;
}
{INTEGERT} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Integer    : %s\n", yytext);
    return INTEGERT;
}
{REALT} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Real       : %s\n", yytext);
    return REALT;
}
{STRINGT} {
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("String     : %s\n", yytext);
    return STRINGT;
}
    /* Global rules */
    /* <<EOF>> return EOFF; // EOF is ignored */
<*>"\n" { // newline
    columnno = 1;
}
{SPACES}+ {} // ignore the spaces
. { // others
    yyerror("Tokens undetected");
    return ERROR;
}
%%

    /*=================== User Code ========================*/
