## PCAT Compiler

PCAT - Pascal Cloned with an ATtitude

something about PCAT:

1. PCAT uses Standard ASCII character set, and case sensitive.
2. Comments are enclosed in pairs of `(*` and `*)`, not nested.
3. Support nested functions.
3. Support interger, real, record values with implicit pointers, arrays and a few simple structured control constructs.

More details see [The PCAT Programming Language Reference Manual](http://lambda.uta.edu/cse5317/pcat04.pdf).


### Tools

* g++ 4.6.3

    For ubuntu, run `sudo aptitude install build-essential` to install it.

* Lex - GNU Flex: The Fast Lexical analyzer 

    [Flex home page](http://flex.sourceforge.net/)
    
* Yacc - GNU Bison

### HOW TO

Just run `make` to build our program.

### Part.1 Lexer

Main goal:

* Recognize token in the PCAT program, including reserved words, operators, identifiers, comments.

* for every token, list out its internal repressentation, value, line number & column number.

* Besides, if there are some errors, print it out, with line number & column number.

* For errors, try you best to ignore it and go on. 
