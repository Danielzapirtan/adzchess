#! /bin/bash

SOURCE=$1
shift
ARGS="$@"

g++ -o compiler/bas2c compiler/bas2c.cpp -w -O4 -march=native
cat $SOURCE.bas
compiler/bas2c $SOURCE.bas $SOURCE.c
gcc -o $SOURCE $SOURCE.c -w -O4 -march=native
$SOURCE $ARGS | sha256sum | tee cgb30.bas

