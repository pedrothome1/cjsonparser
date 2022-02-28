#!/bin/bash

gcc -c src/chstack.c src/parser.c src/main.c
gcc chstack.o parser.o main.o
rm -f chstack.o parser.o main.o
./a.out
