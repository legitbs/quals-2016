CXX=./musl-clang
CXXFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector
#CXXFLAGS=-O2 -Wall -w -pedantic -fmessage-length=0 -pie -fPIE
#CXXFLAGS=-std=c++0x -g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector 
#CCFLAGS=-g3 -Wall -w -pedantic -fmessage-length=0 -fno-stack-protector -fPIC

O_FILES=main.o well_rng.o base64.o cpp_helper.o

passwdgen : $(O_FILES)
	$(CXX) $(CXXFLAGS) -o passwdgen  $(O_FILES)

# dependency stuff, see: http://www.cs.berkeley.edu/~smcpeak/autodepend/autodepend.html
# pull in dependency info for *existing* .o files
-include $(O_FILES:.o=.d)

.SUFFIXES : .o .cpp

.cpp.o :  
	$(CXX) $(CXXFLAGS) -c $<
	$(CXX) -MM $(CXXFLAGS) $*.cpp > $*.d

clean:
	rm -f passwdgen *.o *.d
