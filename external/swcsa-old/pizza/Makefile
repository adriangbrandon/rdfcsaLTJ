SRCDIR = ./src
CC          = g++
#LIBINDEX	= swcsa.a
#SEARCHEXE	= SEARCHwcsa
#BUILDEXE	= BUILDwcsa

ifndef CFLAGS     ##possibly already defined by the "main Makefile".
	CFLAGS = -O9  -m64 -D_FORTIFY_SOURCE=0  
	##CFLAGS      = -O9 -m32 
	##CFLAGS      = -g -m32 -O0
endif

ifndef LIBINDEX     ##possibly already defined by the "main Makefile".
	LIBINDEX = swcsa.a
endif	

ifndef NAMEINDEX
	NAMEINDEX = wcsa
endif

SEARCHEXE = SEARCH$(NAMEINDEX)
BUILDEXE  = BUILD$(NAMEINDEX)

all: clean $(BUILDEXE) $(SEARCHEXE) cleanO show
	
$(BUILDEXE): 
	$(CC) $(CFLAGS) -o $(BUILDEXE) build_index.c  $(LIBINDEX)

$(SEARCHEXE): 
	 $(CC) $(CFLAGS) -o  $(SEARCHEXE) run_queries.c $(LIBINDEX) 
	 

show:
	###
	###type "BUILDwcsa" to create the index and "SEARCHwcsa" to search on it
	###

cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak  $(SEARCHEXE) $(BUILDEXE)


