#!/bin/bash

gcc int3rupt.c -o int3rupt -fno-stack-protector -static -L .
strip -s int3rupt
strip -X int3rupt
strip -x int3rupt
strip -g -S -d int3rupt

