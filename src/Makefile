#--------------------------------------------------------------------------------
#
# Makefile for the PCAT scanner
#
#--------------------------------------------------------------------------------


GCC = gcc
CXXFLAGS = -g -std=gnu99 -Wall -Wno-unused-function -Wno-unused-variable
YACC = bison
YFLAGS = -d -t -y
LEX = flex
PYTHON = python

ACTIONS = action.o code_style.o type.o table.o gen.o global.o

all: parser

parser: main.c y.tab.c lex.yy.c tokens.h ast.h ast.o $(ACTIONS)
	$(GCC) $(CXXFLAGS) lex.yy.c y.tab.c main.c ast.o $(ACTIONS) -o parser

ast.o: ast.c ast.h
	$(GCC) $(CXXFLAGS) -c ast.c

y.tab.c: pcat.y
	$(YACC) $(YFLAGS) pcat.y

lex.yy.c: pcat.lex tokens.h
	$(LEX) pcat.lex

action.o: action.c action.h
	$(GCC) $(CXXFLAGS) -c action.c

code_style.o: code_style.c code_style.h
	$(GCC) $(CXXFLAGS) -c code_style.c

type.o: type.c type.h
	$(GCC) $(CXXFLAGS) -c type.c

table.o: table.c table.h
	$(GCC) $(CXXFLAGS) -c table.c

gen.o: gen.c gen.h
	$(GCC) $(CXXFLAGS) -c gen.c

global.o: global.c global.h
	$(GCC) $(CXXFLAGS) -c global.c


test:
	$(PYTHON) run_tests.py

clean: 
	/bin/rm -f *.o *~ lex.yy.c y.tab.* parser tests/*.frame tests/*.log tests/*.exe tests/*.code tests/*.tree
