#! /bin/bash

SOURCE=$1
shift
ARGS="$@"

g++ -o compiler/bas64toc compiler/bas64toc.cpp -w -O4 -march=native
cat $SOURCE.bas | sed -e "s/sect/10/g" >${SOURCE}_pp.bas
cat $SOURCE_pp.bas
compiler/bas64toc $SOURCE_pp.bas $SOURCE.c
gcc -o $SOURCE $SOURCE.c -w -O4 -march=native
$SOURCE $ARGS | sha256sum
