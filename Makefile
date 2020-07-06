IDIR = ../cudd/include

SOURCES = main.cpp DFA.cpp syn.cpp InputFirst.cpp 
OBJS = main.o DFA.o syn.o InputFirst.o

GXX=g++
GXXFLAGS=-I $(IDIR) -std=c++11 -w -g


LDIR=../cudd/lib
LIBS=-lcudd


SSyft: $(SOURCES)
	$(GXX) $(SOURCES) $(GXXFLAGS) -L $(LDIR) $(LIBS)
	$(GXX) -o SSyft $(SOURCES) $(GXXFLAGS) -L $(LDIR) $(LIBS)




.PHONY: SSyft
clean:
	rm  SSyft
