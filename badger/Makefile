CC=gcc
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -D_FORTIFY_SOURCE=0 -pie -fPIE
#CCFLAGS=-O3
CCFLAGS=-g3 -O1 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector 
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -fPIC

O_FILES = main.o port_helper.o lcd_hal.o lcd_gfx.o glcdfont.o radio_uart.o cli.o sram_hal.o util.o lzssvw.o message_handler.o gui.o bit_util.o

badger: $(O_FILES)
	$(CC) $(CCFLAGS) -o badger $(O_FILES) -lX11

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .c

.c.o :  
	$(CC) $(CCFLAGS) -c $<
	$(CC) -MM $(CFLAGS) $*.c > $*.d

clean:
	rm -f badger *.o *.d
