SRCDIR = .
SRCDIRCSA = .
SRCDIRCSAUTILS = ../intIndexUtils
SRCDIRUTILS = ../utils
CC          = g++

ifndef CFLAGS  ##possibly already defined by the "main Makefile".
	##CFLAGS = -O9 -m32
	##CFLAGS      = -O9 -m32 
	CFLAGS      = -g -m32 -O0
endif


LIBINTINDEX	= icsa.a

all: intIndex cleanO


intIndex: icsa.o parameters.o basics.o huff.o bitmap.o psiHuffmanRLE.o psiVbyteRLE.o psiDeltaCode.o psiGonzalo.o psiR3H.o psiHuffmanRLER3.o psiPlain.o psiR3Hybrid.o   psiPlainRLER3.o      psi_rep.o psi_rep_plain.o psi_rep_plain_rle.o psi_rep_huffman_rle.o psi_rep_hyb3r.o
	ar rc $(LIBINTINDEX) icsa.o psiHuffmanRLE.o psiVbyteRLE.o psiDeltaCode.o psiGonzalo.o  psiR3H.o psiPlain.o psiR3Hybrid.o psiHuffmanRLER3.o           psi_rep.o psi_rep_plain.o psi_rep_plain_rle.o psi_rep_huffman_rle.o psi_rep_hyb3r.o
	##    psiPlainRLER3.o  
			#not including "parameters.o basics.o bitmap.o huff.o" as they are included by wcsa
			#they are already included into the library by the presentation layer.

icsa.o: parameters.o basics.o bitmap.o huff.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o 
	 $(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSA)/icsa.c 

psiHuffmanRLE.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiHuffmanRLE.c 


psiVbyteRLE.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiVbyteRLE.c 

psiDeltaCode.o:
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiDeltaCode.c 

psiGonzalo.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiGonzalo.c 


psiR3H.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiR3H.c 

psiR3Hybrid.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiR3Hybrid.c 


psiPlain.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/psiPlain.c 
	
psiHuffmanRLER3.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/r3/psiHuffmanRLER3.c 

psiPlainRLER3.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/r3/psiPlainRLER3.c 




## new implementation of Psi-representations as cpp classes.
psi_rep.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/cls/psi_rep.cpp 

psi_rep_plain.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/cls/psi_rep_plain.cpp 

psi_rep_plain_rle.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/cls/psi_rep_plain_rle.cpp 

psi_rep_huffman_rle.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/cls/psi_rep_huffman_rle.cpp 

psi_rep_hyb3r.o: huff.o
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRCSAUTILS)/cls/psi_rep_hyb3r.cpp 







parameters.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/parameters.c 
	
	
huff.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/huff.c

basics.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c

bitmap.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/bitmap.c


cleanO: 
	rm -f *.o
	
clean:
	rm -rf *~ *% *.o core *.bak icsa.a 

tar:
	tar czvf icsa.tar.gz  Makefile
