# Parser Makefile
# need flex bison

PARSERCFILES =	../ltllexer.c ../ltlparser.c
PARSERHFILES =	../ltllexer.h ../ltlparser.h
ALLCFILES   =	$(PARSERCFILES) ../ltl_formula.c ../trans.c ../utility.cpp safe2fol.cpp
CC	    =   g++ -std=c++11
CDEBUG	    =	-g
INCLUDE	    =   -I../


ltllexer.c:	../ltllexer.l
		flex ../ltllexer.l

ltlparser.c:	../ltlparser.y
		bison ../ltlparser.y

run:	$(ALLCFILES)
	$(CC) $(INCLUDE) $(CDEBUG) $(ALLCFILES) -o safe2fol

run_debug:  $(ALLCFILES)
	    $(CC) $(INCLUDE) $(CDEBUG) $(ALLCFILES) -o safe2fol

run_release:	run

clean:
	rm -f *.o *~ $(PARSERHFILES) $(PARSERCFILES) safe2fol

