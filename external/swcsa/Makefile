#### Compilation options (32/64 bit)
CC          = g++


#######################################################################
# CHOOSE PSI COMPRESSION TYPE
#######################################################################
PSI_FLAGS = -DPSI_R3H -DR3H_WITHGAPS
#PSITYPE   =  PSI_R3H_GAPS

#######################################################################
# CHOOSE D BITMAP COMPRESSION STRATEGY
#######################################################################

BITMAPTYPE= -DBITMAP_375
#######################################################################


##options: 
	## -DSKIPMAXMINDEF   --> if min/max macros cause redefinition, this avoids defining min/max    (see src/utils/basics.h)
	## -DSKIP_CHECK_PSI_WORKS_OK  --> after compressing Psi, skips performing decompression check. (see src/intIndexUtils/*.c)
	## -DSHOW_RESULTS      --> show the results of each query  (2020, not sure it actually does:D) (see src/benchmark.cpp)
	## Variantes de select:
	## SELECT_BASE (dous select, sen samples), SELECT_SAMPLES (dous select, con samples), SELECT_DUAL (dual_select)
     
#GENFLAGS = -DEXPERIMENTS -DNDEBUG -DSKIPMAXMINDEF 
GENFLAGS = -DEXPERIMENTS -DNDEBUG -DSKIPMAXMINDEF -DSKIP_CHECK_PSI_WORKS_OK


export CFLAGS  =  -O3 -g   -m64 $(GENFLAGS) $(BITMAPTYPE) $(PSI_FLAGS)



#######################################################################
#######################################################################
#### Set the enviroment to build the i-CSA / W-CSA

 ## choose sorting algorithm to build the suffix arrays (uncomment one line)
 ## larson-sadakane alg.
 #export SRCDIRISI   = intIndex_larson  (not used in RDFCSA)
 ## qsort.

 export SRCDIRISI  = intIndex_qsort

# export NAMEINDEX   = wcsa_$(PSITYPE)
 export NAMEINDEX   = wcsa
 export LIBINDEX    = libswcsa.a
 export LIBINTINDEX = icsa.a


SRCDIR = ./src
SRCDIRUTILS = utils


all: clean  buildAll buildAllDual buildStats benchmark buildUncompress cleanO 

all-vello: clean  buildSort buildParser buildIntIndex buildUncompressIntIndex buildUncompress \
            buildAll buildAllDual buildStats buildTestPsi benchmark  benchmarkTest  cleanO 

wsi: intIndexPackage buildFacade.o parameters.o basics.o \
		bitmap.o huff.o fileInfo.o #icsa.o 
	ar rc $(LIBINTINDEX) parameters.o buildFacade.o basics.o \
		bitmap.o huff.o fileInfo.o 
	#ar q $(LIBINDEX) icsa.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o  ##they are included by icsa.a
	mv $(LIBINTINDEX) $(LIBINDEX)

benchmark: wsi
	 $(CC) $(CFLAGS) -o  benchmark$(NAMEINDEX) $(SRCDIR)/benchmark.cpp $(LIBINDEX) 
#	 $(CC) $(CFLAGS) -o  benchmark$(NAMEINDEX) $(SRCDIR)/benchmark.cpp $(LIBINDEX)  -lrt


buildStats:  wsi
	$(CC)  $(CFLAGS) -o STATS$(NAMEINDEX) $(SRCDIR)/buildStats.c $(LIBINDEX)	


buildAll:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDALL$(NAMEINDEX) $(SRCDIR)/buildAll.c $(LIBINDEX)

buildAllDual:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDALLDUAL$(NAMEINDEX) $(SRCDIR)/buildAllDual.c $(LIBINDEX)

buildUncompress:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDUNCOMPRESS$(NAMEINDEX) $(SRCDIR)/buildUncompress.c $(LIBINDEX)


buildFacade.o: parameters.o basics.o bitmap.o \
		 huff.o fileInfo.o  $(LIBINTINDEX)
	 $(CC) $(CFLAGS) -c  $(SRCDIR)/buildFacade.cpp 


################# SELF INDEX ON INTEGERS ##############################
#Builds the int-self-index in dir $(SRCDIRISI) (ex: i-ssa, i-csa,... ) 

intIndexPackage:
	cd  $(SRCDIR)/$(SRCDIRISI) && $(MAKE) -w
	@echo "[copying the int-index lib into current dir]"
	@cp $(SRCDIR)/$(SRCDIRISI)/$(LIBINTINDEX) .

####################### UTILS MODULES #################################

parameters.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/parameters.c 

fileInfo.o:  
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/fileInfo.c
	

huff.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/huff.c

basics.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/basics.c

bitmap.o: 
	$(CC) $(CFLAGS) -c $(SRCDIR)/$(SRCDIRUTILS)/bitmap.c


############################ CLEANING #################################

cleanO: 
	rm -f *.o
	
clean:
	cd  $(SRCDIR)/$(SRCDIRISI) && $(MAKE) clean -w
	rm -rf *~ *% *.o core *.bak $(LIBINTINDEX) $(LIBINDEX) 
	rm -f BUILDALL$(NAMEINDEX) 
	rm -f STATS$(NAMEINDEX) 
	rm -f BUILDUNCOMPRESS$(NAMEINDEX) 
	rm -f benchmark$(NAMEINDEX) 	
	rm -f BUILDALLDUAL$(NAMEINDEX) 


############################ PACKAGING ###############################

tar:
	tar czvf RDFcsa.tar.gz   src Makefile* README  scripts.rdf* compile.sh cleanall.sh COPYRIGHT README

#######################################################################

