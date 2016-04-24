#!/bin/sh

gcc\
    -ansi -zexecstack -fno-stack-protector -no-pie -g \
    -D 'CANARY="asdf"' -D USER_BUF_LEN=40 -D PAD_BUF_LEN=10 \
    -o main \
    main.c
