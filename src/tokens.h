/************************************************************************
*
* File: main.cc
* Testing the PCAT lexer
* Programmer: Leonidas Fegaras, UTA
* Date: 2/9/98
* 
*   Tokens:
* 
*   ----- Literals -----------
* 
*   IDENTIFIER	for identifiers
*   INTEGERT	for integers
*   REALT	for reals
*   STRINGT	for strings
*   
*   ----- Keywords -----------
* 
*   PROGRAM, IS, BEGIN, END, VAR, TYPE, PROCEDURE, ARRAY, RECORD,
*   IN, OUT, READ, WRITE, IF, THEN, ELSE, ELSIF, WHILE, DO, LOOP,
*   FOR, EXIT, RETURN, TO, BY, AND, OR, NOT, OF, DIV, MOD
* 
*   ----- Operators ----------
* 
*   LPAREN	 (   
*   RPAREN	 )  
*   LBRACKET	 [  
*   RBRACKET	 ]  
*   LBRACE	 {  
*   RBRACE	 }  
*   COLON	 :  
*   DOT		 .  
*   SEMICOLON	 ;  
*   COMMA	 ,  
*   ASSIGN	 := 
*   PLUS	 +  
*   MINUS	 -  
*   STAR	 *  
*   SLASH	 /  
*   BACKSLASH	 \  
*   EQ		 =  
*   NEQ		 <> 
*   LT		 <  
*   LE		 <= 
*   GT		 >  
*   GE		 >= 
*   LABRACKET	 [<
*   RABRACKET	 >]
* 
*   ----- Misc -------------- 
* 
*   EOFF	for End Of File
*   ERROR	when lexer error
* 
************************************************************************/


// reserved words
#define	IDENTIFIER	258
#define	INTEGERT	259
#define	REALT	260
#define	STRINGT	261
#define	PROGRAM	262
#define	IS	263
#define	BEGINT	264
#define	END	265
#define	VAR	266
#define	TYPE	267
#define	PROCEDURE	268
#define	ARRAY	269
#define	RECORD	270
#define	IN	271
#define	OUT	272
#define	READ	273
#define	WRITE	274
#define	IF	275
#define	THEN	276
#define	ELSE	277
#define	ELSIF	278
#define	WHILE	279
#define	DO	280
#define	LOOP	281
#define	FOR	282
#define	EXIT	283
#define	RETURN	284
#define	TO	285
#define	BY	286
#define	AND	287
#define	OR	288
#define	NOT	289
#define	OF	290
#define	DIV	291
#define	MOD	292
// operators
#define	LPAREN	293
#define	RPAREN	294
#define	LBRACKET	295
#define	RBRACKET	296
#define	LBRACE	297
#define	RBRACE	298
#define	COLON	299
#define	DOT	300
#define	SEMICOLON	301
#define	COMMA	302
#define	ASSIGN	303
#define	PLUS	304
#define	MINUS	305
#define	STAR	306
#define	SLASH	307
#define	BACKSLASH	308
#define	EQ	309
#define	NEQ	310
#define	LT	311
#define	LE	312
#define	GT	313
#define	GE	314
#define	LABRACKET	315
#define	RABRACKET	316
// others
#define	EOFF	317
#define	ERROR	318


#define NUM_KEYWORD 61

char keywords[][12] = {
"identifier", "integer", "real", "string",
"PROGRAM", "IS", "BEGIN", "END", "VAR",
"TYPE", "PROCEDURE", "ARRAY", "RECORD", "IN",
"OUT", "READ", "WRITE", "IF", "THEN",
"ELSE", "ELSIF", "WHILE", "DO", "LOOP",
"FOR","EXIT","RETURN","TO","BY",
"AND","OR","NOT","OF","DIV",
"MOD","(",")","[","]",
"{","}",":",".",";",
",",":=","+","-","*",
"/","\\","=","<>","<",
"<=",">",">=","[<",">]",
"EOF","ERROR"};
