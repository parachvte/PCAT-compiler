/****************************************************
*
* File  : pcat.lex
* Des   : PCAT lexer
* Author: Ryannnnnnn
*
****************************************************/

    /*================== Definition Section =================*/
%{
    #include <cstdio>

    /* ============= Miscellaneous Macros =============*/
    #define YY_USER_ACTION yyuseraction();

    /* ============== Useful functions ==================
    Maybe move to another file later... by Ryan
    ===================================================*/

    // user action prior to matched rule's action
    void yyuseraction() {
        columnno += yyleng;
        // maybe convert yytext to lowercase
    }
    // print error message
    void yyerror(char *err_msg) {
        printf("\n*** Error: %s (Line: %d, token: '%s')\n", err_msg, yylineno, yytext);
    }

%}

/* Enable global variable yylineno */
%option yylineno
/* Makes the scanner not call yywrap() upon an end-of-file */
%option noyywrap

%x COMMENT

%%
    /*================== Rules Section =======================

    A series of rules of form: pattern action

    How the input is matched:
        http://flex.sourceforge.net/manual/Matching.html
    A match returns character pointer "yytext" and its length "yyleng".
    ========================================================*/

    /* Comments Start Condition */

"(*" {
    columnno += 2;
    BEGIN(COMMENT);
    yymore();
}
<COMMENT>"*)" {
    columnno += 2;
    printf("Ln: %d\tCol: %d\t\t", yylineno, columnno - yyleng);
    printf("Comment: %s\n", yytext);
    BEGIN(INITIAL);
}
<COMMENT>. {
    columnno++;
    yymore();
}
<COMMENT><<EOF>> {
    yyerror("EOF while scanning string literal");
    return EOFF;
}


    /* KEYWORDS */

"PROGRAM"   return PROGRAM;
"IS"        return IS;
"BEGIN"     return BEGINT;
"END"       return END;
"TYPE"      return TYPE;
"PROCEDURE" return PROCEDURE;
"ARRAY"     return ARRAY;
"RECORD"    return RECORD;
"IN"        return IN;
"OUT"       return OUT;
"READ"      return READ;
"WRITE"     return WRITE;
"IF"        return IF;
"THEN"      return THEN;
"ELSE"      return ELSE;
"ELSIF"     return ELSIF;
"WHILE"     return WHILE;
"DO"        return DO;
"LOOP"      return LOOP;
"FOR"       return FOR;
"EXIT"      return EXIT;
"RETURN"    return RETURN;
"TO"        return TO; 
"BY"        return BY; 
"AND"       return AND;
"OR"        return OR; 
"NOT"       return NOT;
"OF"        return OF; 
"DIV"       return DIV;
"MOD"       return MOD;

"("         return LPAREN;
")"         return RPAREN;
"["         return LBRACKET;
"]"         return RBRACKET;
"{"         return LBRACE;
"}"         return RBRACE;
":"         return COLON;
"."         return DOT;
";"         return SEMICOLON;
","         return COMMA;
":="        return ASSIGN;
"+"         return PLUS;
"-"         return MINUS;
"*"         return STAR;
"/"         return SLASH;
"\\"        return BACKSLASH;
"="         return EQ;
"<>"        return NEQ;
"<"         return LT;
"<="        return LE;
">"         return GT;
">="        return GE;
"[<"        return LABRACKET;
">]"        return RABRACKET;

    /* Global rules */

<<EOF>> return EOFF; // EOF
<*>"\n" {
    columnno = 1;
}
. { // others
    yyerror("Tokens undetected");
}
%%

    /*=================== User Code =========================
    ========================================================*/
