CC=gcc
CCFLAGS=-fPIC -pie -Wno-unused-result -O2 -m64 -Wall

O_FILES = kiss.o

all : $(O_FILES)
	$(CC) $(CCFLAGS) -o kiss $(O_FILES)
	strip -g -S -d kiss
	strip -s kiss
	strip -x kiss
	strip -X kiss

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .c

.c.o :  
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f kiss *.o *.d
