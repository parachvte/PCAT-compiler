#!/bin/sh

echo "Compiling $1"
file=$1

# Check if file exist
if [ ! -f "$file" ]; then
    echo "File no exists"
    exit 0
fi
dir=`dirname $file`

# parse
if [ ! -f "./parser" ]; then
	make
fi
./parser $1 > $1.log

# compile to executable
if [ "$?" -eq "0" ]; then
    rm data.s code.s
    gcc -g -m32 -c centry.c 
    gcc -g -m32 -c pcat.s
    gcc -g -m32 centry.o pcat.o -o $1.exe
    mv pcat.s $1.code
    mv frame.out $1.frame
    mv ast.out $1.ast

    echo "Compiling success!"
else
    echo "Compiling failed!"
fi
