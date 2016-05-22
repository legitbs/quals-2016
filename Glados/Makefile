ROOT=/home/lightning
CC=$(ROOT)/gcc-custom/bin/gcc
CPP=$(ROOT)/gcc-custom/bin/g++
UCLIBC=$(ROOT)/uClibc
UCLIBCPLUS=$(ROOT)/uClibc++
MODE=64
CCFLAGS=-fPIC -fpic -O2 -m$(MODE) -std=c99 -Wall
CPPFLAGS=-fPIC -fpic -Wno-sign-compare -Wno-unused-result -Wno-write-strings -O2 -m$(MODE) -std=c++11 -Wall -D_FORTIFY_SOURCE=0

O_FILES = main.o classes.o in_out.o init.o

all : $(O_FILES)
	$(CPP) $(CPPFLAGS) -o glados $(O_FILES) $(UCLIBC)/usr/lib/crt1.o -nostdlib -static -lc -L$(UCLIBC)/usr/lib -L$(UCLIBCPLUS)/lib -luClibc++ -lc
	strip -g -S -d glados
	strip -s glados
	strip -x glados
	strip -X glados

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .c .cpp

.cpp.o :
	$(CPP) $(CPPFLAGS) -c $<

.c.o :  
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f glados *.o *.d
