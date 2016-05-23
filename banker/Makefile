CXX=g++
CXXFLAGS=-g3 -O1 -Wall -w -fno-stack-protector -static -m32
#CXXFLAGS=-O2 -Wall -w -pedantic -fmessage-length=0 -pie -fPIE
#CXXFLAGS=-std=c++0x -g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector 
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -fPIC

O_FILES=main.o user_manager.o input_reader.o doublelist.o string.o transfer_manager.o cpp_helper.o base64.o

banker : $(O_FILES)
	$(CXX) $(CXXFLAGS) -o banker  $(O_FILES)

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .cpp

.cpp.o :  
	$(CXX) $(CXXFLAGS) -c $<
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

clean:
	rm -f banker *.o *.d
