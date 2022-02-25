#!/bin/bash

gcc -c json.c main.c
gcc json.o main.o
rm -f json.o main.o
./a.out
