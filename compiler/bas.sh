#! /bin/bash

SOURCE=$1
shift
ARGS="$@"

g++ -o compiler/bas2c compiler/bas2c.cpp -w -O4 -march=native
compiler/bas2c $SOURCE.bas $SOURCE.c
gcc -o $SOURCE $SOURCE.c -w -O4 -march=native
$SOURCE $ARGS | sha256sum | tee cgb24.txt

