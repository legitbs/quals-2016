all:
	#./neatcc/ncc -o test.o test.c
	./fasm start.S start.o
	#./fasm write.S write.o
	#./neatld/nld -p -s -o test start.o write.o test.o
	gcc crippled.c -o crippled -m32 -fPIC -pie
	cp neatcc/ncc compiler
	cp neatld/nld compiler
	cp start.o compiler

clean:
	rm *.o test
