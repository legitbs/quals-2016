all: baby-re create-baby-re

create-baby-re: create-baby-re.o
	gcc -o create-baby-re create-baby-re.o

baby-re: baby-re.o create-baby-re.o
	gcc -O0 -o baby-re baby-re.o

baby-re.o: baby-re.c
	gcc -O0 -c baby-re.c

create-baby-re.o: create-baby-re.c
	gcc -c create-baby-re.c
