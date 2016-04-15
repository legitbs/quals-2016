#!/bin/sh

gcc -o main  -zexecstack -fno-stack-protector -g main.c
