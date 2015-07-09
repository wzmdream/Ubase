#
# Makefile for the test programs
#

.SUFFIXES: .o .C

#
# Compiler and loader definitions
#
PROGRAM = 	testpage

LD =		ld
LDFLAGS =	

CXX =           g++
CXXFLAGS =	-g -Wall


#
# general definitions
#

MAKEFILE =      Makefile

#
# list of all object and source files
#

OBJS =  page.o error.o testpage.o
SRCS =	page.C error.C testpage.C

all:		$(PROGRAM)

$(PROGRAM):	$(OBJS)
		$(CXX) -o $@ $(OBJS) $(LDFLAGS)

.C.o:
		$(CXX) $(CXXFLAGS) -c $<

clean:
		rm -f core *.bak *~ *.o $(PROGRAM) *.pure .pure testpage

depend:
		makedepend -I /s/gcc/include/g++ -f$(MAKEFILE) \
		$(SRCS)

# DO NOT DELETE THIS LINE -- make depend depends on it.
