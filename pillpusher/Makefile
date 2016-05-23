all:
	gcc -S -fomit-frame-pointer -fno-builtin -nostdlib stubstart.S pillpusher.c write.c mmap.c exit.c printf.c malloc.c stdlib.c read.c open.c close.c
	python insert.py pillpusher.s
	python insert.py write.s
	python insert.py mmap.s
	python insert.py exit.s
	python insert.py printf.s
	python insert.py malloc.s
	python insert.py stdlib.s
	python insert.py read.s
	python insert.py open.s
	python insert.py close.s
	gcc -s -fno-builtin -nostdlib stubstart.S pillpusher.s write.s mmap.s exit.s printf.s malloc.s stdlib.s read.s open.s close.s -o pillpusher 

clean:
	rm *.s
	rm pillpusher
