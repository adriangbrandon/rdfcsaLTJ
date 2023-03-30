#### Compilation options (32/64 bit)
CC          = g++


#######################################################################
# CHOOSE PSI COMPRESSION TYPE BY properly SETTING PSI_FLAGS
#######################################################################
# #PSITYPE   =  PSI_R3H_GAPS
# 
# 	PSI_FLAGS = -DPSI_R3H -DR3H_WITHGAPS
# 
# 
# 
# #  [buscar velocidad: S y O en plano (uin32 o log2bits), P con PsiHuffmanRle3R]
#      #  [RDFCSA3RHyb       :     :375]  *usando log_k bits !! para S y Os.
#      #  [RDFCSA3RHy-Gap    : GAPS:375]  *usando log_k bits !! para S y Os.
#      #  [RDFCSA3RHy-u32    :     :375]  *usando uint32s    !! para S y Os.
#      #  * Codifico valores de Psi con 3 rangos.
#      #       -S y O como uint32, o como log_k-bits (en este caso usando o no gaps)
#      #       -P codificado como HuffmanRLE3r.
#             
# #PSITYPE   =  PSI_R3HYBRID        
# 	PSI_FLAGS = -DPSI_R3HYBRID -DPSI_PLAIN_LOG_BASED
# 
# #PSITYPE   =  PSI_R3HYBRID_GAPS	
# 	PSI_FLAGS = -DPSI_R3HYBRID -DPSI_PLAIN_LOG_BASED -DR3H_WITHGAPS 
# 
# #PSITYPE   =  PSI_R3HYBRID_UINT32
# 	PSI_FLAGS = -DPSI_R3HYBRID 



# type either in the command-line   ...> see compile.sh
#     export PSITYPE=PSI_R3H_GAPS
#	  export PSITYPE=PSI_R3HYBRID_GAPS


ifeq ($(PSITYPE),PSI_R3HYBRID)
	PSI_FLAGS = -DPSI_R3HYBRID -DPSI_PLAIN_LOG_BASED 
else ifeq ($(PSITYPE),PSI_R3HYBRID_GAPS) 
	PSI_FLAGS = -DPSI_R3HYBRID -DPSI_PLAIN_LOG_BASED -DR3H_WITHGAPS 
else ifeq ($(PSITYPE),PSI_R3HYBRID_UINT32) 
	PSI_FLAGS = -DPSI_R3HYBRID 
	

else ifeq ($(PSITYPE),PSI_R3H_GAPS)
	PSI_FLAGS = -DPSI_R3H -DR3H_WITHGAPS -DPSI_PLAIN_LOG_BASED
		

else   #default option  PSI_R3CLS_GAPS
	PSI_FLAGS = -DPSI_R3CLS_GAPS -DR3H_WITHGAPS -DPSI_PLAIN_LOG_BASED 
	#PSI_FLAGS = -DPSI_R3H -DR3H_WITHGAPS -DPSI_PLAIN_LOG_BASED	
	export PSITYPE=
endif




#######################################################################
# CHOOSE D BITMAP COMPRESSION STRATEGY
#######################################################################

BITMAPTYPE= -DBITMAP_375
#######################################################################


##options: 
	## -DSKIPMAXMINDEF   --> if min/max macros cause redefinition, this avoids defining min/max    (see src/utils/basics.h)
	## -DSKIP_CHECK_PSI_WORKS_OK  --> after compressing Psi, skips performing decompression check. (see src/intIndexUtils/*.c)
	## -DNO_TEST_SPO_EQ_OPS     --> in BuildAllDual.c, skips comparison SPO-OPS test; i.e. calling to testRecoverAndCompareSPO_OPS_dump().
	## -DSHOW_RESULTS      --> show the results of each query  (2020, not sure what it actually does:D) (see src/benchmark.cpp)
	## Variantes de select:
	## SELECT_BASE (dous select, sen samples), SELECT_SAMPLES (dous select, con samples), SELECT_DUAL (dual_select)
     
#GENFLAGS = -DEXPERIMENTS -DNDEBUG -DSKIPMAXMINDEF 
GENFLAGS = -DEXPERIMENTS -DNDEBUG -DSKIPMAXMINDEF -DSKIP_CHECK_PSI_WORKS_OK -DNO_TEST_SPO_EQ_OPS



#export CFLAGS  =  -O0 -g   -m64 $(GENFLAGS) $(BITMAPTYPE) $(PSI_FLAGS)
#export CFLAGS  =  -O3   -m64 $(GENFLAGS) $(BITMAPTYPE) $(PSI_FLAGS)
export CFLAGS  =  -msse4.2 -O3  -ffast-math -funroll-loops   -m64 $(GENFLAGS) $(BITMAPTYPE) $(PSI_FLAGS)



#######################################################################
#######################################################################
#### Set the enviroment to build the i-CSA / W-CSA

 ## choose sorting algorithm to build the suffix arrays (uncomment one line)
 ## larson-sadakane alg.
 #export SRCDIRISI   = intIndex_larson  (not used in RDFCSA)
 ## qsort.

 export SRCDIRISI  = intIndex_qsort

 export NAMEINDEX   = wcsa$(PSITYPE)
 export LIBINDEX    = libswcsa$(PSITYPE).a
# export NAMEINDEX   = wcsa
# export LIBINDEX    = libswcsa.a
 export LIBINTINDEX = icsa.a


SRCDIR = ./src
SRCDIRUTILS = utils


all: clean  buildAll buildAllDual buildStats buildStatsDual benchmark benchmarkTestDual \
            buildUncompress buildUncompressIntIndex cleanO 

all-vello: clean  buildSort buildParser buildIntIndex buildUncompressIntIndex buildUncompress \
            buildAll buildAllDual buildStats buildStatsDual buildTestPsi benchmark  benchmarkTest  cleanO 

wsi: intIndexPackage buildFacade.o parameters.o basics.o \
		bitmap.o huff.o fileInfo.o #icsa.o 
	ar rc $(LIBINTINDEX) parameters.o buildFacade.o basics.o \
		bitmap.o huff.o fileInfo.o 
	#ar q $(LIBINDEX) icsa.o psiHuffmanRLE.o psiDeltaCode.o psiGonzalo.o  ##they are included by icsa.a
	mv $(LIBINTINDEX) $(LIBINDEX)

benchmark: wsi
	 $(CC) $(CFLAGS) -o  benchmark$(NAMEINDEX) $(SRCDIR)/benchmark.cpp $(LIBINDEX) 
#	 $(CC) $(CFLAGS) -o  benchmark$(NAMEINDEX) $(SRCDIR)/benchmark.cpp $(LIBINDEX)  -lrt

benchmarkTestDual: wsi
	 $(CC) $(CFLAGS) -o  benchmarkTestDual$(NAMEINDEX) $(SRCDIR)/benchmarkTestDual.cpp $(LIBINDEX) 



buildStats:  wsi
	$(CC)  $(CFLAGS) -o STATS$(NAMEINDEX) $(SRCDIR)/buildStats.c $(LIBINDEX)	

buildStatsDual:  wsi
	$(CC)  $(CFLAGS) -o STATSDUAL$(NAMEINDEX) $(SRCDIR)/buildStatsDual.c $(LIBINDEX)	


buildAll:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDALL$(NAMEINDEX) $(SRCDIR)/buildAll.c $(LIBINDEX)

buildAllDual:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDALLDUAL$(NAMEINDEX) $(SRCDIR)/buildAllDual.c $(LIBINDEX)

buildUncompress:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDUNCOMPRESS$(NAMEINDEX) $(SRCDIR)/buildUncompress.c $(LIBINDEX)


buildFacade.o: parameters.o basics.o bitmap.o \
		 huff.o fileInfo.o  $(LIBINTINDEX)
	 $(CC) $(CFLAGS) -c  $(SRCDIR)/buildFacade.cpp 


buildUncompressIntIndex:  wsi
	$(CC) -DFACADEWITHMAIN $(CFLAGS) -o BUILDUNCOMPRESSINTINDEX$(NAMEINDEX)$(EXE_SUFFIXBITMAP) $(SRCDIR)/buildUncompressIntIndex.c $(LIBINDEX)



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
	rm -f STATSDUAL$(NAMEINDEX) 
	rm -f BUILDUNCOMPRESS$(NAMEINDEX) 
	rm -f BUILDUNCOMPRESSINTINDEX$(NAMEINDEX) 
	rm -f benchmark$(NAMEINDEX) 	
	rm -f benchmarkTestDual$(NAMEINDEX) 	
	rm -f BUILDALLDUAL$(NAMEINDEX) 


############################ PACKAGING ###############################

tar:
	tar czvf RDFcsa.tar.gz   src Makefile* README  scripts.rdf* compile.sh cleanall.sh COPYRIGHT README

#######################################################################

