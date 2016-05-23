CXX=./musl-clang
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -D_FORTIFY_SOURCE=0 -pie -fPIE
CXXFLAGS=-g3 -Wall -w
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector 
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -fPIC

#O_FILES=main.o exception_handler.o cpp_helper.o well_rng.o string.o common.o malloc-2.7.2.o critical_exception.o digital_receiver.o doublelist.o dsp_engine.o messagehandler.o packet.o radiomac.o rc4engine.o radiomessage.o
O_FILES=main.o exception_handler.o cpp_helper.o well_rng.o string.o common.o critical_exception.o digital_receiver.o doublelist.o dsp_engine.o messagehandler.o packet.o radiomac.o rc4engine.o radiomessage.o

secrf : $(O_FILES)
	$(CXX) $(CXXFLAGS) -o secrf  $(O_FILES)

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .cpp

.cpp.o :  
	$(CXX) $(CXXFLAGS) -c $<
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

.c.o   :
	$(CXX) $(CXXFLAGS) -c $<
	$(CXX) -MM $(CXXFLAGS) $*.c > $*.d

clean:
	rm -f secrf *.o *.d
