all: step.o enc.o scramble.o
	gcc -o step step.o
	gcc -o enc enc.o
	objdump -DfhprRtTxlS -M intel step > step.objdump
	./enc
	strip --strip-all step
	gcc -o scramble scramble.o

step.o: step.c
	gcc -Wno-format-security -c step.c

enc.o: enc.c
	gcc -c enc.c

scramble.o: scramble.c
	gcc -c scramble.c
