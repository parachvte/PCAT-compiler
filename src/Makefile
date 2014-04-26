#--------------------------------------------------------------------------------
#
# Makefile for the PCAT scanner
#
#--------------------------------------------------------------------------------


GCC = gcc
CXXFLAGS = -g -std=gnu99 -Wall -Wno-unused-function -Wno-write-strings
YACC = bison
YFLAGS = -d -t -y
LEX = flex
PYTHON = python


all: parser

parser: main.c y.tab.c lex.yy.c tokens.h ast.h ast.o
	$(GCC) $(CXXFLAGS) lex.yy.c y.tab.c main.c ast.o -o parser

ast.o: ast.c ast.h
	$(GCC) $(CXXFLAGS) -c ast.c

y.tab.c: pcat.y
	$(YACC) $(YFLAGS) pcat.y

lex.yy.c: pcat.lex tokens.h
	$(LEX) pcat.lex

test:
	$(PYTHON) run_tests.py

clean: 
	/bin/rm -f *.o *~ lex.yy.c y.tab.* parser
