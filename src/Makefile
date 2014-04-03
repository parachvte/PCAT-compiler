#--------------------------------------------------------------------------------
#
# Makefile for the PCAT scanner
#
#--------------------------------------------------------------------------------


GCC = g++
CXXFLAGS = -g -Wall -Wno-unused-function -Wno-write-strings
YACC = bison
LEX = flex
PYTHON = python

all: lexer

test:
	$(PYTHON) run_tests.py

lexer:	main.cc pcat.yy.c tokens.h
	$(GCC) $(CXXFLAGS) main.cc -o lexer

pcat.yy.c: pcat.lex
	$(LEX) pcat.lex
	mv lex.yy.c pcat.yy.c

clean: 
	/bin/rm -f *.o *~ pcat.yy.c lexer
