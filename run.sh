#!/bin/bash

gcc -c src/chstack.c src/json.c src/main.c
gcc chstack.o json.o main.o
rm -f chstack.o json.o main.o
./a.out
