#!/bin/bash

gcc -c src/json.c src/main.c
gcc json.o main.o
rm -f json.o main.o
./a.out
