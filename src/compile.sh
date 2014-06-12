#!/bin/bash

echo "compiling $1"

if [ -f "./parser" ]; then
	./parser $1 > $1.log
else
	make all
	./parser $1 > $1.log
fi

if [ "$?" -eq "0" ]; then
    rm data.s code.s
    gcc -g -m32 -c sup.c
    gcc -g -m32 -c pcat.s
    gcc -g -m32 sup.o pcat.o -o $1.exe
    cp pcat.s $1.code
    cp frame.info $1.frame
    cp tree.info $1.tree

    rm pcat.s
    rm frame.info
    rm tree.info
    
    echo "compiling success!"
else
    echo "compiling failed!"
fi


