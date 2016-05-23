SVCNAME = crunchtime 

EXTRAFLAGS = -Os -ffunction-sections -Wl,-Map,crunchtime.map -pie -fPIC

CC = gcc
CFLAGS = -g -Wall -Wshadow -Wextra -std=gnu99 $(EXTRAFLAGS)
INCLUDEDIR = .

CFILES = $(wildcard *.c)
OBJECTS = $(patsubst %.c,%.o,$(CFILES))

$(SVCNAME): $(OBJECTS)
	$(CC) $(CFLAGS) $(DEFINES) $(OBJECTS) -o $(SVCNAME) $(LIBS)
	strip -s $(SVCNAME)
	strip -g -S -d $(SVCNAME)
	strip -x $(SVCNAME)
	strip -X $(SVCNAME)
	python crunchtime-fixup.py

%.o:  %.c Makefile
	$(CC) $(CFLAGS) $(DEFINES) -I$(INCLUDEDIR) -c $< -o $@

clean:
	rm -rf $(SVCNAME) *.o

.PHONY: clean
