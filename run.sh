#!/bin/bash
if [ -n "$1" ]; then
    if [[ $1 == "debug" ]]; then
        gcc main.c util.c child.c F1.c -o main -g -ggdb && ./main
    elif [[ $1 == "all" ]]; then
        gcc main.c util.c child.c F1.c F2.c F3.c -o main && ./main
    else
        gcc main.c util.c child.c F1.c -o main
    fi
else
    gcc main.c util.c child.c F1.c -o main && ./main
fi
