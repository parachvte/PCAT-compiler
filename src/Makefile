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

ROUTINES = routine.o code_style.o type_check.o table.o code_gen.o

all: parser

parser: main.c y.tab.c lex.yy.c tokens.h ast.h ast.o $(ROUTINES)
	$(GCC) $(CXXFLAGS) lex.yy.c y.tab.c main.c ast.o $(ROUTINES) -o parser
	rm -f *.o lex.yy.c y.tab.*

ast.o: ast.c ast.h
	$(GCC) $(CXXFLAGS) -c ast.c

y.tab.c: pcat.y
	$(YACC) $(YFLAGS) pcat.y

lex.yy.c: pcat.lex tokens.h
	$(LEX) pcat.lex

routine.o: routine.c routine.h
	$(GCC) $(CXXFLAGS) -c routine.c

code_style.o: code_style.c routine.h
	$(GCC) $(CXXFLAGS) -c code_style.c

type_check.o: type_check.c routine.h
	$(GCC) $(CXXFLAGS) -c type_check.c

table.o: table.c table.h
	$(GCC) $(CXXFLAGS) -c table.c

code_gen.o: code_gen.c routine.h
	$(GCC) $(CXXFLAGS) -c code_gen.c


test:
	$(PYTHON) run_tests.py

clean: 
	rm -f *.o *~ lex.yy.c y.tab.* parser
	rm -f *.s *.out
	rm -f tests/*.frame tests/*.log tests/*.exe tests/*.code tests/*.ast
