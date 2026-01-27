#! /bin/bash

SOURCE=$1
shift
ARGS="$@"

g++ -o compiler/bas64toc compiler/bas64toc.cpp -w -O4 -march=native
cat $SOURCE.bas
compiler/bas64toc $SOURCE.bas $SOURCE.c
gcc -o $SOURCE $SOURCE.c -w -O4 -march=native
$SOURCE $ARGS | sha256sum | tee 1792_2048.txt
