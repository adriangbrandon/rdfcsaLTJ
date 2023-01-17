/* icsa.c
 * Copyright (C) 2011, Antonio Fariña and Eduardo Rodriguez, all rights reserved.
 *
 * icsa.c: Implementation of the interface "../intIndex/interfaceIntIndex.h"
 *   that permits to represent a sequence of uint32 integers with an iCSA: 
 *   An integer-oriented Compressed Suffix Array.
 *   Such representation will be handled as a "ticsa" data structure, that
 *   the WCSA self-index will use (as an opaque type) to 
 *   create/save/load/search/recover/getSize of the representation of the 
 *   original sequence.
 *   Suffix sorting is done via q-sort()
 *    
 * See more details in:
 * Antonio Fariña, Nieves Brisaboa, Gonzalo Navarro, Francisco Claude, Ángeles Places, 
 * and Eduardo Rodríguez. Word-based Self-Indexes for Natural Language Text. ACM 
 * Transactions on Information Systems (TOIS), 2012. 
 * http://vios.dc.fi.udc.es/indexing/wsi/publications.html
 * http://www.dcc.uchile.cl/~gnavarro/ps/tois11.pdf	   
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */
  
#include "icsa.h"

//#define SELECT_BASE
//#define SELECT_SAMPLES
//#define SELECT_DUAL


// Global para que funcione a funcion de comparacion do quicksort
uint *intVectorqsort;

void printPsiIndexType(uint t) {
	//http://web.theurbanpenguin.com/adding-color-to-your-output-from-c/
	fflush(stdout);fflush(stderr);
	printf("\033[1;31m\n");
	fflush(stdout);fflush(stderr);
	printf("ICSA-OPTIONS: ");
	printf("\033[0;34m");


	#ifdef PSI_VBYTERLE		
		printf("PSI_VBYTE-RLE");
	#endif

	#ifdef PSI_GONZALO
		printf("PSI_GONZALO");
	#endif
	
	#ifdef PSI_DELTACODES
		printf("PSI_DELTA_CODES");
	#endif

	#ifdef PSI_HUFFMANRLE	
		printf("PSI_HUFFMAN-RLE");
	#endif	

	#ifdef PSI_R3H
		printf("PSI_R3H");
		#ifdef R3H_WITHGAPS
			printf("\033[0;35m -GAPS (on abs-samples)");
		#endif	
	#endif	
	
	#ifdef PSI_R3HYBRID
		printf("PSI_R3HYBRID");
		#ifdef PSI_PLAIN_LOG_BASED
			printf(" [S&O as log_k-bit-arrays, P as psiHuffmanRLER3]");
		#else
			printf(" [S&O as plain uin32-arrays, P as psiHuffmanRLER3]");
		#endif
		#ifdef R3H_WITHGAPS
			printf("\033[0;35m :: -GAPS activated");
		#endif	
	#endif	


	#ifdef PSI_PLAIN
		printf("PSI_PLAIN");
	#endif


	printf("\n\033[1;31m");
	printf("BITMAP-OPTIONS: ");
	printf("\033[0;34m");
	#ifdef BITMAP_375	
		printf("BITMAP_375");
	#endif	
	#ifdef BITMAP_RGK	
		printf("BITMAP_RGK");
	#endif	


	printf("\n\033[1;31m");
	printf("T_PSI: ");
	printf("\033[0;34m");
	printf("%u",t);

	printf("\033[0m\n");
	fflush(stdout);fflush(stderr);	
}	


// Para o quicksort
int suffixCmp(const void *arg1, const void *arg2) {

	register uint a,b;
	a=*((uint *) arg1);
	b=*((uint *) arg2);
	
	assert(arg1 != arg2);

	while(intVectorqsort[a] == intVectorqsort[b]) { a++; b++; }
	
	if (( ((ssize_t)intVectorqsort[a]) - ((ssize_t)intVectorqsort[b]) ) <0)
		return -1;
	return +1;

}

//ticsa *createIntegerCSA(uint **aintVector, uint textSize, char *build_options) {
int buildIntIndex (uint *aintVector, uint n, uint nEntries, char *build_options, void **index ){
	uint textSize=n;	

	intVectorqsort= aintVector;  //global variable for suffix-sort- cmp function
	
	ticsa *myicsa;
	myicsa = (ticsa *) malloc (sizeof (ticsa));
	uint *Psi, *SAI, *C, vocSize;
	register uint i, j;
	uint nsHUFF;

	parametersCSA(myicsa, build_options);
	
	nsHUFF=myicsa->tempNSHUFF;
	
	// Almacenamos o valor dalguns parametros
	myicsa->nEntries = nEntries;
	myicsa->suffixArraySize = textSize;
	myicsa->D = (uint*) malloc (sizeof(uint) * ((textSize+31)/32));	
	myicsa->D[ ((textSize+31)/32) -1]=0;	

	printPsiIndexType(myicsa->T_Psi);

	
//@@	myicsa->samplesASize = (textSize + myicsa->T_A - 1) / myicsa->T_A;// + 1;
//@@	myicsa->samplesA = (uint *)malloc(sizeof(int) * myicsa->samplesASize);
//@@	myicsa->BA = (uint*) malloc (sizeof(uint) * ((textSize+31)/32));
//@@	myicsa->samplesAInvSize = (textSize + myicsa->T_AInv - 1) / myicsa->T_AInv;
//@@	myicsa->samplesAInv = (uint *)malloc(sizeof(int) * myicsa->samplesAInvSize);


	// Reservamos espacio para os vectores
	Psi = (uint *) malloc (sizeof(uint) * textSize);

	// CONSTRUIMOS A FUNCION C
	vocSize = 0;
	for(i=0;i<textSize;i++) if(intVectorqsort[i]>vocSize) vocSize = intVectorqsort[i];
	C = (uint *) malloc(sizeof(uint) * (vocSize + 1));	// Para contar o 0 terminador
	for(i=0;i<vocSize;i++) C[i] = 0;
	for(i=0;i<textSize;i++) C[intVectorqsort[i]]++;
	for (i=0,j=0;i<=vocSize;i++) {
		j = j + C[i];
		C[i] = j;
	}
	for(i=vocSize;i>0;i--) C[i] = C[i-1];
	C[0] = 0;

	// Construimos o array de sufixos (en Psi) - con quicksort
	printf("\n\t *BUILDING THE SUFFIX ARRAY over %u integers... (with qsort)", textSize);fflush(stdout);
	for(i=0; i<textSize; i++) Psi[i]=i;
	
	//skipping call to qsort as the input is sorted.
	//printf("\n\t **** skipping qsort as input is sorted... ");fflush(stdout);
	qsort(Psi, textSize, sizeof(uint), suffixCmp);
	
	printf("\n\t ...... ended.");
	printf("\n SA[0]= %u",Psi[0]);
	printf("\n SA[1]= %u",Psi[1]);
	printf("\n SA[2]= %u",Psi[2]);
	printf("\n   ");
	printf("\n SA[%u]= %u",textSize-4, Psi[textSize-4]);
	printf("\n SA[%u]= %u",textSize-3, Psi[textSize-3]);
	printf("\n SA[%u]= %u",textSize-2, Psi[textSize-2]);
	printf("\n SA[%u]= %u",textSize-1, Psi[textSize-1]);


	// CONSTRUIMOS A INVERSA DO ARRAY DE SUFIXOS
	SAI = (uint *) malloc (sizeof(uint) * (textSize + 1));	// +1 para repetir na ultima posición. Evitamos un if
	for(i=0;i<textSize;i++) SAI[Psi[i]] = i;
	SAI[textSize] = SAI[0];

	{int z;
	for (z=0;z<10;z++) {printf("\n A-inv[%u]= %u",z, Psi[z]);}	printf("\n   ");
    }

		//@@	// ALMACENAMOS AS MOSTRAS DO ARRAY DE SUFIXOS
		//@@	for(i=0;i<((textSize+31)/32);i++) myicsa->BA[i] = 0;
		//@@	for(i=0; i<textSize; i+=myicsa->T_A) bitset(myicsa->BA, SAI[i]);
		//@@	bitset(myicsa->BA, SAI[textSize-1]);			// A ultima posicion sempre muestreada
		//@@	//printf("TextSize = %d\n", textSize);
		//@@	myicsa->bBA = createBitmap(myicsa->BA, textSize);
		//@@	for(i=0,j=0; i<textSize; i++) if(bitget(myicsa->BA, i)) myicsa->samplesA[j++] = Psi[i];
			
			// ALMACENAMOS AS MOSTRAS DA INVERSA DO ARRAY DE SUFIXOS
		//@@	for(i=0,j=0;i<textSize;i+=myicsa->T_AInv) myicsa->samplesAInv[j++] = SAI[i];
	
	// CONSTRUIMOS E COMPRIMIMOS PSI
	printf("\n\t Creating compressed Psi...");
	for(i=0;i<textSize;i++) Psi[i] = SAI[Psi[i]+1];
	
	//FILE *ff = fopen("psi.log","w");
	//for (i=0;i<textSize;i++) fprintf(ff,"%d::%u",i,Psi[i]);
	//fclose(ff);
	
	free(SAI);

	{
	size_t z;
		printf("\n psi antes de cíclica: Rangos = [%u,%u] [%u,%u] [%u,%u] ",0, textSize/3-1, textSize/3, textSize/3*2-1,textSize/3*2 ,textSize-1  );
		for (z=0;z<10;z++) {printf("\n PSI[%zu]= %u",z, Psi[z]);}	printf("\n   ");
		for (z=textSize/3-5; z<textSize/3+4 ;z++) {printf("\n PSI[%zu]= %u",z, Psi[z]);}	printf("\n   ");
		for (z=textSize/3*2-5; z<textSize/3*2+4 ;z++) {printf("\n PSI[%zu]= %u",z, Psi[z]);}	printf("\n   ");
		for (z=textSize-10;z<textSize;z++) {		printf("\n PSI[%zu]= %u",z, Psi[z]); 	}	printf("\n   ");
	}


	// ---------------------------------------------------------------------------//
	// ** aqui modificamos PSI, para hacerla cíclica en las tuplas ** //

	size_t end = textSize -1;
	size_t range_length = end/nEntries;
	size_t z=  range_length * (nEntries -1);


	for (   ; z < end;z++) {
		Psi[z] = ( ((long)Psi[z]) - ((long)1)) % range_length;        //psi[i] = p[i]-1 mod numRecords;
	}

	// ** fin de la modificación de PSI para hacerla cíclica             ** //
	// ---------------------------------------------------------------------------//

	{
	size_t z;
		printf("\n psi cíclica: Rangos = [%u,%u] [%u,%u] [%u,%u] ",0, textSize/3-1, textSize/3, textSize/3*2-1,textSize/3*2 ,textSize-1  );
		for (z=0;z<10;z++) {printf("\n PSI[%zu]= %u",z, Psi[z]);}	printf("\n   ");
		for (z=textSize/3-5; z<textSize/3+4 ;z++) {printf("\n PSI[%zu]= %u",z, Psi[z]);}	printf("\n   ");
		for (z=textSize/3*2-5; z<textSize/3*2+4 ;z++) {printf("\n PSI[%zu]= %u",z, Psi[z]);}	printf("\n   ");
		for (z=textSize-10;z<textSize;z++) {		printf("\n PSI[%zu]= %u",z, Psi[z]); 	}	printf("\n   ");
	}
	
	
	#ifdef PSI_HUFFMANRLE	
	myicsa->hcPsi = huffmanCompressPsi(Psi,textSize,myicsa->T_Psi,nsHUFF);
	#endif				
	#ifdef PSI_GONZALO	
	myicsa->gcPsi = gonzaloCompressPsi(Psi,textSize,myicsa->T_Psi,nsHUFF);
	#endif			
	#ifdef PSI_DELTACODES
	myicsa->dcPsi = deltaCompressPsi(Psi,textSize,myicsa->T_Psi);		
	#endif

	#ifdef PSI_VBYTERLE	
		myicsa->vbPsi = vbyteCompressPsi(Psi,textSize,myicsa->T_Psi);
	#endif
	
	#ifdef PSI_R3H		 
	myicsa->hcPsi = CompressPsiR3H(Psi,textSize,myicsa->T_Psi,nsHUFF);
	#endif				

	#ifdef PSI_R3HYBRID	 
	myicsa->hcPsi = CompressPsiR3Hybrid(Psi,textSize,myicsa->T_Psi,nsHUFF);
	#endif				

	#ifdef PSI_PLAIN
	myicsa->plPsi = createPlainPsi(Psi,textSize);
	#endif




/*
{ ////////////////////////////// CHECKS PSI COMPRESSION WORKED ///////////////////////////////////////////////////////
	char fileNtmp[200]= "tmptmp.huffmanRLeCompressedPSI";
	storeHuffmanCompressedPsi(&(myicsa->hcPsi), fileNtmp);	
	destroyHuffmanCompressedPsi(&(myicsa->hcPsi));
	myicsa->hcPsi = loadHuffmanCompressedPsi(fileNtmp);	
	HuffmanCompressedPsi *cPsi = &myicsa->hcPsi;
	     
    size_t psiSize = textSize;
     
//	FILE *f = fopen("psi.dump","w");
//	fwrite(&psiSize,1,sizeof(size_t),f);
//	fwrite(Psi,sizeof(uint),psiSize,f);
//	fclose(f);

	fprintf(stderr,"\n Test compress/uncompress PSI is starting for all i in Psi[0..%lu]\n ",psiSize-1); fflush(stdout); fflush(stderr);
	size_t i;
	uint val1,val2;	
	fflush(stdout);
	uint count=0;
	uint step = psiSize/1000;
	for (i=0; i<psiSize; i++) {
		if(i%step==0) fprintf(stderr, "Processing %.1f%%\r", (float)i/psiSize*100);
			val1= getHuffmanPsiValue(cPsi, i);
			val2=Psi[i];
			if (val1 != val2) { count++;
					fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
					//fprintf(stderr,"\n i=%zu,diffs[i] = %ld ",i, (long)diffs[i]); fflush(stdout);fflush(stderr);
					if (count > 20) {
						fprintf(stderr,"\n test failed!!");
						exit(0);
					}
						
			}
	}
		fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);

	
} /////////////////////////////////////////////////////////////////////////////////////
*/



  	
  	free(Psi);	
		
	// Contruimos D
	for(i=0;i<((textSize+31)/32);i++) myicsa->D[i] = 0;	
	for(i=0;i<=vocSize;i++) bitset(myicsa->D, C[i]);
		
	//**************************************//		
	//bitmap 37.5%  (Eduardo Rodríguez)
	//-DBITMAP_375
	myicsa->bD = createBitmap(myicsa->D,textSize);	
	
//@NOLONGER-RGK2023	//**************************************//
//@NOLONGER-RGK2023	//bitmap RGK (Roberto Konow).
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023	//-DBITMAP_RGK
//@NOLONGER-RGK2023	BitString *bstr= new BitString(myicsa->D, (size_t) textSize); 	
//@NOLONGER-RGK2023	BitSequenceBuilder *bs;
//@NOLONGER-RGK2023	
//@NOLONGER-RGK2023		
//@NOLONGER-RGK2023    //BASIC LIBCDS BITSEQUENCES
//@NOLONGER-RGK2023    //@@fari2017. Modifications:
//@NOLONGER-RGK2023    //@@fari2017. RG bitmaps now works (there was a bug when adapting from libcdsbasics to our libcdsbasics) 
//@NOLONGER-RGK2023    //@@fari2017. RRR, RGK works as always.
//@NOLONGER-RGK2023    //@@fari2017. SDARRAY. Included
//@NOLONGER-RGK2023    //@@fari2017. DELTA. Uses sampling and deltaCodes do represent the bitmap.
//@NOLONGER-RGK2023    //@@fari2017. ---
//@NOLONGER-RGK2023    //@@fari2017. RUNSOZ: split the original bitString into two bitStrings O and Z (Gonzalos' book, page 86). 
//@NOLONGER-RGK2023    //@@fari2017.    then represents it with what you want: RRR, RG, SDARRAY, DELTA. and with the sampling you want.
//@NOLONGER-RGK2023    //@@fari2017.    except for SDARRAY that takes no sampling parameter. See BitSequenceBuilders below. In this
//@NOLONGER-RGK2023    //@@fari2017.    case you have to indicate "bs_type" and "sample_rate" parameter... and Z and O will be
//@NOLONGER-RGK2023    //@@fari2017.    represented with the same BitSequence type.
//@NOLONGER-RGK2023    //@@fari2017.    According to Gonzalo's advice we should use SDARRAY or possibly *DELTA*.
//@NOLONGER-RGK2023    //@@fari2017. 
//@NOLONGER-RGK2023    //@@fari2017. RUNSOZ_opt: The same as above (split into O and Z). Yet in this case Z and O are represented 
//@NOLONGER-RGK2023    //@@fari2017.    with the BitSequence that leads to the best compression, using default sampling parameters.
//@NOLONGER-RGK2023    //@@fari2017.    See BitSequenceRUNSOZ_opt:getDefaultSampleRate().
//@NOLONGER-RGK2023    //@@fari2017.        RRR-->32, RG --> 20 (overhead 5%), Delta --> 64, SDARRAY --> no-sampling.
//@NOLONGER-RGK2023    //@@fari2017.
//@NOLONGER-RGK2023    
//@NOLONGER-RGK2023		
//@NOLONGER-RGK2023	//bs = new BitSequenceBuilderSDArray();
//@NOLONGER-RGK2023	//bs = new BitSequenceBuilderRRR(32); 
//@NOLONGER-RGK2023	//bs = new BitSequenceBuilderRG(20);  //5% overhead
//@NOLONGER-RGK2023	//bs = new BitSequenceBuilderDelta(64);
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023	//bs = new BitSequenceBuilderRGK(2, 32,0,1);  //DEFAULT_SAMPLING=32   //DEFAULT
//@NOLONGER-RGK2023	//bs = new BitSequenceBuilderRGK(2, 32,1,1);  //DEFAULT_SAMPLING=32
//@NOLONGER-RGK2023	//	bs = new BitSequenceBuilderRGK(factor, sampling,fastSelect0,fastSelect1);  //DEFAULT_SAMPLING=32
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023		// START build options for RUNS-OZ bitSequence:----------------------------------------
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023		// 	bs = new BitSequenceBuilderRUNSOZ(RRR02_HDR,25);  
//@NOLONGER-RGK2023		// 	bs = new BitSequenceBuilderRUNSOZ(RRR02_HDR);  
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023		//  	bs = new BitSequenceBuilderRUNSOZ(DELTA_HDR,128);  
//@NOLONGER-RGK2023		//  	bs = new BitSequenceBuilderRUNSOZ(DELTA_HDR);  
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023		// 	bs = new BitSequenceBuilderRUNSOZ(SDARRAY_HDR);  //no sampling needed
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023		// 	bs = new BitSequenceBuilderRUNSOZ(BRW32_HDR,2);  //50% overhead
//@NOLONGER-RGK2023		//	bs = new BitSequenceBuilderRUNSOZ(BRW32_HDR,4);  //25% overhead
//@NOLONGER-RGK2023		//	bs = new BitSequenceBuilderRUNSOZ(BRW32_HDR,20); //5% overhead   default!
//@NOLONGER-RGK2023		//  bs = new BitSequenceBuilderRUNSOZ(BRW32_HDR);    // 5% overhead  default !
//@NOLONGER-RGK2023		 
//@NOLONGER-RGK2023		//  	bs = new BitSequenceBuilderRUNSOZ();  //default = sdarray 
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023	// END build options for RUNS-OZ bitSequence:----------------------------------------
//@NOLONGER-RGK2023
//@NOLONGER-RGK2023// 	bs = new BitSequenceBuilderRUNSOZ(SDARRAY_HDR);  //no sampling needed
//@NOLONGER-RGK2023// 	bs = new BitSequenceBuilderRUNSOZ_opt();  //50% overhead
//@NOLONGER-RGK2023	bs = new BitSequenceBuilderRRR(20);  
//@NOLONGER-RGK2023//	bs = new BitSequenceBuilderRGK(2, 32,0,1);  //DEFAULT_SAMPLING=32   //DEFAULT
//@NOLONGER-RGK2023	
//@NOLONGER-RGK2023	
//@NOLONGER-RGK2023	myicsa->Dmap = bs->build(*bstr);
//@NOLONGER-RGK2023	delete (BitString *) bstr;                            //@@fari2017. Faltaba por liberar o BitString
//@NOLONGER-RGK2023	delete (BitSequenceBuilder *) bs;                     //@@fari2017. Faltaba por liberar o BitSequenceBuilder 
 		
	//**************************************//	

	
	/********* Tabulamos las posiciones de los 1s en el rango [(SuffixArraySize-1)/3, 2*(SuffixArraySize-1)/3 -1 ******/
	// to solve select_1 over predicates with a single table-lookup
	{   uint nEntries = myicsa->nEntries;
		uint n = (myicsa->suffixArraySize-1)/ nEntries -1; 
		uint ns = rank_1(myicsa->bD,n-1); //-1;
		uint np = rank_1(myicsa->bD,2*n-1)-rank_1(myicsa->bD,n) +1+1;  //the last +1 to know also the pos of the first object (needed during searches)
		myicsa->rankfirstPredicate = ns;
		myicsa->numPredicates = np;
		myicsa->Dselect1Predicates = (uint *)malloc (sizeof(uint) * np);
		uint i;
		
		for (i=0;i<np;i++) 
			myicsa->Dselect1Predicates[i] = bselect(myicsa->bD, ns+i);		
		/*	
		cout << "PREDICATES-TABLE_D_ONES: n =" <<n << ", ns = " << ns << ",np= " << np <<endl;
		cout << "PREDICATES-TABLE_D_ONES: firstPredicate = " << myicsa->rankfirstPredicate  << endl;
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate0) = " <<  myicsa->Dselect1Predicates[0]<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate1) = " <<  myicsa->Dselect1Predicates[1]<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate2) = " <<  myicsa->Dselect1Predicates[2]<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate3) = " <<  myicsa->Dselect1Predicates[3]<< endl;	

		cout << "PREDICATES-TABLE_D_ONES: select1(predicate0) = " << getSelectTablePredicates(myicsa,0+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate1) = " << getSelectTablePredicates(myicsa,1+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate2) = " << getSelectTablePredicates(myicsa,2+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate3) = " << getSelectTablePredicates(myicsa,3+ns)<< endl;	


		cout << "PREDICATES-TABLE_D_ONES: select1(predicate[np-2]) = " << getSelectTablePredicates(myicsa,np-2+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate[np-1]) = " << getSelectTablePredicates(myicsa,np-1+ns)<< endl;			    
		*/
	 } 	
	/********* Tabulamos las posiciones de los 1s en el rango [(SuffixArraySize-1)/3, 2*(SuffixArraySize-1)/3 -1 ******/
		
		
	free(C);	

	// VARIABLE GLOBAL QUE ALMACENA O ESTADO DOS DISPLAYS (IMPORTANTE PARA DISPLAY SECUENCIAL)
	// Almacena a última posición do array de sufixos que mostramos con display ou displayNext
	// Se nos piden un displayNext, aplicamos PSI sobre esta posición e obtemos a seguinte,
	// coa que podemos obter o símbolo pedido, e actualizamos displayState
//@@	myicsa->displayCSAState = 0;
//@@	myicsa->displayCSAPrevPosition = -2;  //needed by DisplayCSA(position)
	
	// Liberamos o espacio non necesario

	*index = myicsa;
	//return (myicsa);
	
	
	printf("\n\n @@ Summary of self-index on Integers:");
	printInfoIntIndex(myicsa, " ");
	fflush(stderr);	fflush(stdout);
	
	return 0;
}


//Returns number of elements in the indexed sequence of integers
int sourceLenIntIndex(void *index, uint *numInts){
	ticsa *myicsa = (ticsa *) index;
	*numInts= myicsa->suffixArraySize;
	return 0; //no error;
}

int saveIntIndex(void *index, char *pathname) {
//void storeStructsCSA(ticsa *myicsa, char *basename) {
  
 	ticsa *myicsa = (ticsa *) index; 
 	char *basename=pathname;

	char *filename;
	int file;
	int write_err;
	// Reservamos espacio para o nome do ficheiro
	filename = (char *)malloc(sizeof(char)*MAX_FILENAME_LENGTH);
		
	// Ficheiro co n�mero de elementos indexados (enteiros do texto orixinal)
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, NUMBER_OF_ELEMENTS_FILE_EXT);
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}

	write_err=write(file, &(myicsa->nEntries), sizeof(uint));
	write_err=write(file, &(myicsa->suffixArraySize), sizeof(int));
	close(file);		

	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, PSI_COMPRESSED_FILE_EXT);	

	#ifdef PSI_HUFFMANRLE
		storeHuffmanCompressedPsi(&(myicsa->hcPsi), filename);	
	#endif	
	#ifdef PSI_GONZALO
		storeGonzaloCompressedPsi(&(myicsa->gcPsi), filename);	
	#endif
	#ifdef PSI_DELTACODES
		storeDeltaCompressedPsi(&(myicsa->dcPsi), filename);
	#endif

	#ifdef PSI_VBYTERLE
		storeVbyteCompressedPsi(&(myicsa->vbPsi), filename);	
	#endif
	#ifdef PSI_R3H
		storePsiR3H(&(myicsa->hcPsi), filename);	
	#endif	
	#ifdef PSI_R3HYBRID
		storePsiR3Hybrid(&(myicsa->hcPsi), filename);	
	#endif	
	
	#ifdef PSI_PLAIN
		storePlainPsi(&(myicsa->plPsi), filename);	
	#endif

	

//BITMAP_375
	
	// Ficheiro co vector de bits D
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_FILE_EXT);  
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	write_err=write(file, myicsa->D, sizeof(int)*((myicsa->suffixArraySize+31)/32));
	close(file);

	// Directorio de rank para D
	// Almacenamos o n�mero de superbloques seguido dos superbloques
	// E logo o n�mero de bloques seguido dos bloques
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_RANK_DIRECTORY_FILE_EXT);
	saveBitmap(filename,myicsa->bD);
	
//@NOLONGER-RGK2023 //BITMAP_RGK	
//@NOLONGER-RGK2023 	strcpy(filename, basename);
//@NOLONGER-RGK2023 	strcat(filename, ".");
//@NOLONGER-RGK2023 	strcat(filename, D_BITSEQUENCERGK_FILE_EXT);
//@NOLONGER-RGK2023 	ofstream f(filename);
//@NOLONGER-RGK2023 	myicsa->Dmap->save(f);
//@NOLONGER-RGK2023 	f.close();
	
	
	
//@@	// Ficheiro coas mostras de A
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, SAMPLES_A_FILE_EXT); 
//@@	unlink(filename);
//@@	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//@@		printf("Cannot open file %s\n", filename);
//@@		exit(0);
//@@	}
//@@	write_err=write(file, myicsa->samplesA, sizeof(int) * (myicsa->samplesASize));
//@@	close(file);

//@@	// Ficheiro co vector BA (marca as posicions de A muestreadas)
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, BA_FILE_EXT);  
//@@	unlink(filename);
//@@	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//@@		printf("Cannot open file %s\n", filename);
//@@		exit(0);
//@@	}
//@@	write_err=write(file, myicsa->BA, sizeof(int)*((myicsa->suffixArraySize+31)/32));
//@@	close(file);

//@@	// Directorio de rank para BA
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, BA_RANK_DIRECTORY_FILE_EXT);
//@@	saveBitmap(filename, myicsa->bBA);
	
//@@	// Ficheiro coas mostras de A inversa
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, SAMPLES_A_INV_FILE_EXT);
//@@	unlink(filename);
//@@	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
//@@		printf("Cannot open file %s\n", filename);
//@@		exit(0);
//@@	}
//@@	write_err=write(file, myicsa->samplesAInv, sizeof(int) * (myicsa->samplesAInvSize));
//@@	close(file);	

	// Ficheiro co periodo de muestreo de A e A inversa
	strcpy(filename, basename);
	strcat(filename, ".");
	//strcat(filename, SAMPLING_PERIOD_A_FILE_EXT);
	strcat(filename, SAMPLING_PERIOD_PSI_SEARCHFACTOR_FILE_EXT);
	
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
//@@	write_err=write(file, &(myicsa->T_A), sizeof(int));
//@@	write_err=write(file, &(myicsa->T_AInv), sizeof(int));
	
	write_err=write(file, &(myicsa->psiSearchFactorJump), sizeof(uint));
	write_err=write(file, &(myicsa->T_Psi), sizeof(uint));

	close(file);
	free(filename);	
	return 0; //no error.
}

//Returns the size (in bytes) of the index over the sequence of integers.
//uint CSA_size(ticsa *myicsa) {
int sizeIntIndex(void *index, size_t *numBytes) {
	ticsa *myicsa = (ticsa *) index;
	size_t size = 0;
	size +=(sizeof (ticsa) * 1);
	
//@NOLONGERRGK-2023#ifndef BITMAP_RGK
	size += sizeof(uint)*((myicsa->suffixArraySize+31)/32) ;  //D vector
	size += myicsa->bD->mem_usage;
//@NOLONGERRGK-2023#else
//@NOLONGERRGK-2023	size +=myicsa->Dmap->getSize();
//@NOLONGERRGK-2023#endif
	
	size +=myicsa->numPredicates *sizeof(uint);   //tabla Select en predicados (directo).
	
//@@	size += sizeof(uint) * myicsa->samplesASize ;  // samples A
//@@	size += sizeof(uint) * myicsa->samplesAInvSize ;  // samples A^{-1}
//@@	size += sizeof(uint)*((myicsa->suffixArraySize+31)/32) ;  //BA vector
//@@	size += myicsa->bBA->mem_usage;

	#ifdef PSI_HUFFMANRLE
		size +=myicsa->hcPsi.totalMem;	 	
	#endif	
	#ifdef PSI_GONZALO
		size +=myicsa->gcPsi.totalMem;	 	
	#endif
	#ifdef PSI_DELTACODES
		size +=myicsa->dcPsi.totalMem;
	#endif	
	
	#ifdef PSI_VBYTERLE
		//size +=myicsa->vbPsi.totalMem;	
		size += getTotalMem(&myicsa->vbPsi); 	
	#endif	
	#ifdef PSI_R3H
		size +=myicsa->hcPsi.totalMem;	 	
	#endif	
	#ifdef PSI_R3HYBRID
		size +=myicsa->hcPsi.totalMem;	 	
	#endif	

	#ifdef PSI_PLAIN
		size +=myicsa->plPsi.totalMem;	 	
	#endif
		
		
	*numBytes = size;
	return 0; //no error.
}


//ticsa *loadCSA(char *basename) {
int loadIntIndex(char *pathname, void **index){

	char *basename=pathname;
	char *filename;
	int file;
	uint length;
	char c;
	char *word;
	struct stat f_stat;	
	uint suffixArraySize;


	ticsa *myicsa;
	myicsa = (ticsa *) malloc (sizeof (ticsa) * 1);
	int read_err;
	
	// VARIABLE GLOBAL QUE ALMACENA O ESTADO DOS DISPLAYS (IMPORTANTE PARA DISPLAY SECUENCIAL)
	// Almacena a �ltima posici�n do array de sufixos que mostramos con display ou displayNext
	// Se nos piden un displayNext, aplicamos PSI sobre esta posici�n e obtemos a seguinte,
	// coa que podemos obter o s�mbolo pedido, e actualizamos displayState
//@@	myicsa->displayCSAState = 0;
//@@	myicsa->displayCSAPrevPosition = -2;  //needed by DisplayCSA(position)
	
	// Reservamos espacio para o nome do ficheiro
	filename = (char *)malloc(sizeof(char)*MAX_FILENAME_LENGTH);

	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O NUMERO DE ELEMENTOS INDEXADOS
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, NUMBER_OF_ELEMENTS_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) { 
		printf("Cannot read file %s\n", filename);exit(0);
	}	
	read_err=read(file, &(myicsa->nEntries), sizeof(uint));	
	read_err=read(file, &suffixArraySize, sizeof(uint));		
	myicsa->suffixArraySize = suffixArraySize;
	fprintf(stderr,"Number of indexed elements (suffix array size) = %u\n", suffixArraySize);
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA PSI COMPRIMIDA	
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, PSI_COMPRESSED_FILE_EXT);		
	#ifdef PSI_HUFFMANRLE
		myicsa->hcPsi = loadHuffmanCompressedPsi(filename);	 	
	#endif	
	#ifdef PSI_GONZALO
		myicsa->gcPsi = loadGonzaloCompressedPsi(filename);	 	
	#endif
	#ifdef PSI_DELTACODES
		myicsa->dcPsi = loadDeltaCompressedPsi(filename);
	#endif	 

	#ifdef PSI_VBYTERLE
		myicsa->vbPsi = loadVbyteCompressedPsi(filename, myicsa->bD);	 	
	#endif	
	#ifdef PSI_R3H
		myicsa->hcPsi = loadPsiR3H(filename);	 	
	#endif	
	#ifdef PSI_R3HYBRID
		myicsa->hcPsi = loadPsiR3Hybrid(filename);	 	
	#endif	

	#ifdef PSI_PLAIN
		myicsa->plPsi = loadPlainPsi(filename);	 		
	#endif


	/******* Bitmap 37.5% ***************************************/
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA D
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}	
	myicsa->D = (uint *) malloc (sizeof(uint)*((suffixArraySize+31)/32));
	read_err=read(file, myicsa->D, sizeof(uint)*((suffixArraySize+31)/32));
	fprintf(stderr,"Bit vector D loaded (%u bits)\n", suffixArraySize);

	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O DIRECTORIO DE RANK1 PARA D
	strcpy(filename, basename);
	strcat(filename, ".");
	strcat(filename, D_RANK_DIRECTORY_FILE_EXT);				
	myicsa->bD = loadBitmap(filename,myicsa->D,suffixArraySize);
	{	uint ns, nb;		
		ns = myicsa->bD->sSize;
		nb = myicsa->bD->bSize;
		myicsa->bD->data = myicsa->D;
		fprintf(stderr,"Rank1 Directory for D loaded (%d superblocks, %d blocks)\n", ns, nb);	
	}
	

//@NOLONGERRGK-2023		
//@NOLONGERRGK-2023	/************* bitmap RGK (R. Konow) ******************/
//@NOLONGERRGK-2023	strcpy(filename, basename);
//@NOLONGERRGK-2023	strcat(filename, ".");
//@NOLONGERRGK-2023	strcat(filename, D_BITSEQUENCERGK_FILE_EXT);
//@NOLONGERRGK-2023	//cout << "read bitmap " << filename << endl;
//@NOLONGERRGK-2023	ifstream g(filename);
//@NOLONGERRGK-2023	myicsa->Dmap = BitSequence::load(g);	  //@@fari2017
//@NOLONGERRGK-2023	g.close();	
//@NOLONGERRGK-2023	/*****************************************************/
	
	
	/********* Tabulamos las posiciones de los 1s en el rango [(SuffixArraySize-1)/3, 2*(SuffixArraySize-1)/3 -1 ******/
	{   uint nEntries = myicsa->nEntries;
		uint n = (myicsa->suffixArraySize-1)/ nEntries -1;
		uint ns = rank_1(myicsa->bD,n-1); //-1;
		uint np = rank_1(myicsa->bD,2*n-1)-rank_1(myicsa->bD,n) +1+1;  //the last +1 to know also the pos of the first object (needed during searches)
		myicsa->rankfirstPredicate = ns;
		myicsa->numPredicates = np;
		myicsa->Dselect1Predicates = (uint *)malloc (sizeof(uint) * np);
		uint i;
		
		for (i=0;i<np;i++) 
			myicsa->Dselect1Predicates[i] = bselect(myicsa->bD, ns+i);		
		/*	
		cout << "PREDICATES-TABLE_D_ONES: n =" <<n << ", ns = " << ns << ",np= " << np <<endl;
		cout << "PREDICATES-TABLE_D_ONES: firstPredicate = " << myicsa->rankfirstPredicate  << endl;
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate0) = " <<  myicsa->Dselect1Predicates[0]<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate1) = " <<  myicsa->Dselect1Predicates[1]<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate2) = " <<  myicsa->Dselect1Predicates[2]<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate3) = " <<  myicsa->Dselect1Predicates[3]<< endl;	

		cout << "PREDICATES-TABLE_D_ONES: select1(predicate0) = " << getSelectTablePredicates(myicsa,0+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate1) = " << getSelectTablePredicates(myicsa,1+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate2) = " << getSelectTablePredicates(myicsa,2+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate3) = " << getSelectTablePredicates(myicsa,3+ns)<< endl;	


		cout << "PREDICATES-TABLE_D_ONES: select1(predicate[np-2]) = " << getSelectTablePredicates(myicsa,np-2+ns)<< endl;	
		cout << "PREDICATES-TABLE_D_ONES: select1(predicate[np-1]) = " << getSelectTablePredicates(myicsa,np-1+ns)<< endl;			    
		*/
	 } 	
	/********* Tabulamos las posiciones de los 1s en el rango [(SuffixArraySize-1)/3, 2*(SuffixArraySize-1)/3 -1 ******/
	
	

	
	
	
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA SAMPLES A
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, SAMPLES_A_FILE_EXT);
//@@	if( (file = open(filename, O_RDONLY)) < 0) {
//@@		printf("Cannot read file %s\n", filename); exit(0);
//@@	}
//@@	if( fstat(file, &f_stat) < 0) {
//@@		printf("Cannot read information from file %s\n", filename);	exit(0);
//@@	}	
//@@	myicsa->samplesASize = (f_stat.st_size)/sizeof(uint);
//@@	myicsa->samplesA = (uint *)malloc(sizeof(uint) * myicsa->samplesASize);
//@@	read_err=read(file, myicsa->samplesA, sizeof(uint) * myicsa->samplesASize);		
//@@	printf("Suffix array samples loaded (%d samples)\n", myicsa->samplesASize);	
	
//@@	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA BA
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, BA_FILE_EXT);
//@@	if( (file = open(filename, O_RDONLY)) < 0) {
		//@@printf("Cannot read file %s\n", filename); exit(0);
//@@	}	
//@@	myicsa->BA = (uint *) malloc (sizeof(uint)*((suffixArraySize+31)/32));
//@@	read_err=read(file, myicsa->BA, sizeof(uint)*((suffixArraySize+31)/32));
//@@	printf("Bit vector BA loaded (%u bits)\n", suffixArraySize);

//@@	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O DIRECTORIO DE RANK1 PARA BA
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, BA_RANK_DIRECTORY_FILE_EXT);				
//@@	myicsa->bBA = loadBitmap(filename,myicsa->BA,suffixArraySize);
//@@	{	uint ns, nb;		
//@@		ns = myicsa->bBA->sSize;
//@@		nb = myicsa->bBA->bSize;
//@@		myicsa->bBA->data = myicsa->BA;
//@@		printf("Rank1 Directory for BA loaded (%d superblocks, %d blocks)\n", ns, nb);	
//@@	}

//@@	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA SAMPLES A INVERSA
//@@	strcpy(filename, basename);
//@@	strcat(filename, ".");
//@@	strcat(filename, SAMPLES_A_INV_FILE_EXT);
//@@	if( (file = open(filename, O_RDONLY)) < 0) {
//@@		printf("Cannot read file %s\n", filename); exit(0);
//@@	}
//@@	if( fstat(file, &f_stat) < 0) {
//@@		printf("Cannot read information from file %s\n", filename);	exit(0);
//@@	}	
//@@	myicsa->samplesAInvSize = (f_stat.st_size)/(sizeof(uint));
//@@	myicsa->samplesAInv = (uint *)malloc(sizeof(uint) * myicsa->samplesAInvSize);
//@@	read_err=read(file, myicsa->samplesAInv, sizeof(uint) * myicsa->samplesAInvSize);		
//@@	printf("Suffix array inverse samples loaded (%d samples)\n", myicsa->samplesAInvSize);
	
	// LEEMOS OS DATOS DO FICHEIRO QUE ALMACENA O PERIODO DE MUESTREO DO ARRAY DE SUFIXOS E DA INVERSA
	strcpy(filename, basename);
	strcat(filename, ".");
//	strcat(filename, SAMPLING_PERIOD_A_FILE_EXT);
	strcat(filename, SAMPLING_PERIOD_PSI_SEARCHFACTOR_FILE_EXT);
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename); exit(0);
	}	
//@@	read_err=read(file, &(myicsa->T_A), sizeof(uint));
//@@	read_err=read(file, &(myicsa->T_AInv), sizeof(uint));	
//@@	printf("Sampling A Period T = %d, Sampling A inv Period TInv = %d\n", myicsa->T_A, myicsa->T_AInv);	
	
	read_err=read(file, &(myicsa->psiSearchFactorJump), sizeof(uint));
	fprintf(stderr,"Psi Bin Search Factor-Jump is = %d\n", myicsa->psiSearchFactorJump);	
	read_err=read(file, &(myicsa->T_Psi), sizeof(uint));
	fprintf(stderr,"Sampling Period PSI is = %d\n", myicsa->T_Psi);	

	printPsiIndexType(myicsa->T_Psi);
	
	close(file);
	free(filename);




	//return myicsa;
	*index = myicsa;
	return 0;
}
	

//uint destroyStructsCSA(ticsa *myicsa) {	
int freeIntIndex(void *index) {	
	ticsa *myicsa = (ticsa *) index;
		// Liberamos o espacio reservado
		
	if (!myicsa) return 0;
	
	size_t total=0, totaltmp=0;
	
	size_t nbytes;sizeIntIndex(index, &nbytes);
		
	total +=(sizeof (ticsa) * 1);;
	
	#ifdef PSI_HUFFMANRLE
		total+= totaltmp = myicsa->hcPsi.totalMem;
		destroyHuffmanCompressedPsi(&(myicsa->hcPsi));
	#endif
	#ifdef PSI_GONZALO
		total+= totaltmp = myicsa->gcPsi.totalMem;
		destroyGonzaloCompressedPsi(&(myicsa->gcPsi));
	#endif
	#ifdef PSI_DELTACODES
		total+= totaltmp = myicsa->dcPsi.totalMem;
		destroyDeltaCodesCompressedPsi(&(myicsa->dcPsi));	
	#endif
	#ifdef PSI_VBYTERLE
		//total+= totaltmp = myicsa->vbPsi.totalMem;
		total += getTotalMem(&myicsa->vbPsi);
		destroyVbyteCompressedPsi(&(myicsa->vbPsi));
	#endif	
	#ifdef PSI_R3H
		total+= totaltmp = myicsa->hcPsi.totalMem;
		destroyPsiR3H(&(myicsa->hcPsi));
	#endif
	#ifdef PSI_R3HYBRID
		total+= totaltmp = myicsa->hcPsi.totalMem;
		destroyPsiR3Hybrid(&(myicsa->hcPsi));
	#endif

	#ifdef PSI_PLAIN
		total+= totaltmp = myicsa->plPsi.totalMem;
		destroyPlainPsi(&(myicsa->plPsi));
	#endif
	
	
	printf("\n\t[destroying  iCSA: compressed PSI structure] ...Freed %zu bytes... RAM", totaltmp);
	

//@NOLONGERRGK-2023#ifndef BITMAP_RGK	
		total+= totaltmp =  (sizeof(uint)*((myicsa->suffixArraySize+31)/32)); 
							printf("\n\t[destroying  iCSA: D vector] ...Freed %zu bytes... RAM",totaltmp);
							total += myicsa->bD->mem_usage;							
//@NOLONGERRGK-2023#else
//@NOLONGERRGK-2023		total+= totaltmp = myicsa->Dmap->getSize();
//@NOLONGERRGK-2023		printf("\n\t[destroying  iCSA: Dvector+bitmapRGK] ...Freed %zu bytes... RAM",totaltmp);	
//@NOLONGERRGK-2023#endif		

					
	//BITMAP_375
	free(myicsa->D);	//	total+= totaltmp =  (sizeof(uint)*((myicsa->suffixArraySize+31)/32)); 
						//	printf("\n\t[destroying  iCSA: D vector] ...Freed %zu bytes... RAM",totaltmp);
						//	total += myicsa->bD->mem_usage;							
	destroyBitmap(myicsa->bD);		
			
//@NOLONGERRGK-2023	//BITMAP_RGK
//@NOLONGERRGK-2023	delete myicsa->Dmap;	
				
	total+= myicsa->numPredicates *sizeof(uint);
	printf("\n\t[destroying Dselect1Predicates table ... Freed %zu bytes... RAM",myicsa->numPredicates *sizeof(uint));
	free(	myicsa->Dselect1Predicates);

				
							
//@@	free(myicsa->samplesA);		total+= totaltmp = (sizeof(uint) * myicsa->samplesASize); 
//@@							printf("\n\t[destroying  Samples A: A   ] ...Freed %zu bytes... RAM",totaltmp);
//@@	free(myicsa->samplesAInv);	total+= totaltmp =  (sizeof(uint) * myicsa->samplesAInvSize); 
//@@							printf("\n\t[destroying  Samples AInv: A   ] ...Freed %zubytes... RAM",totaltmp);							
//@@						printf("\n\t[destroying  rank bit D   ] ...Freed %zu bytes... RAM", (size_t)myicsa->bD->mem_usage);
//@@	free(myicsa->BA);			total+= totaltmp =  (sizeof(uint)*((myicsa->suffixArraySize+31)/32)); 
//@@							printf("\n\t[destroying  SA: BA vector] ...Freed %zu bytes... RAM",totaltmp);
								
//@@								total += myicsa->bBA->mem_usage;
//@@	destroyBitmap(myicsa->bBA);
								
	printf("\n\t**** [the whole iCSA ocuppied ... %zu bytes... RAM",total);
	printf("\n\t**** iCSA size = %zu bytes ", (size_t) nbytes);
	printf("\n");
	
	free(myicsa);
	
	return 0; //no error.
}

	// Shows detailed summary info of the self-index (memory usage of each structure)
int printInfoIntIndex(void *index, const char tab[]) {
	ticsa *myicsa = (ticsa *) index;
	if (!myicsa) return 0;
	
	size_t structure, totalpsi, totalD, totalBD, totalSA,totalSAinv, totalBA,totalBBA;
	
	structure=sizeof(ticsa);
	
	#ifdef PSI_HUFFMANRLE
		totalpsi = myicsa->hcPsi.totalMem;
	#endif
	#ifdef PSI_GONZALO
		totalpsi = myicsa->gcPsi.totalMem;
	#endif
	#ifdef PSI_DELTACODES
		totalpsi = myicsa->dcPsi.totalMem;
	#endif
	#ifdef PSI_VBYTERLE
		totalpsi = getTotalMem(&myicsa->vbPsi);			
	#endif
	#ifdef PSI_R3H
		totalpsi = myicsa->hcPsi.totalMem;
	#endif
	#ifdef PSI_R3HYBRID
		totalpsi = myicsa->hcPsi.totalMem;
	#endif

	#ifdef PSI_PLAIN
		totalpsi = myicsa->plPsi.totalMem;
	#endif



	#ifdef PSI_HUFFMANRLE

		unsigned long  getHuffmanPsiSizeBitsUptoPosition(HuffmanCompressedPsi *cPsi, size_t position);
		unsigned long sizesPSIQuartil[3];
		unsigned long sizesPSIAcummQuartil[3];
		size_t n = myicsa->suffixArraySize;

		sizesPSIAcummQuartil[0] = getHuffmanPsiSizeBitsUptoPosition(&myicsa->hcPsi, n/3);
		sizesPSIAcummQuartil[1] = getHuffmanPsiSizeBitsUptoPosition(&myicsa->hcPsi, 2*n/3);
		sizesPSIAcummQuartil[2] = getHuffmanPsiSizeBitsUptoPosition(&myicsa->hcPsi, n-1);
	
		sizesPSIQuartil[0] = getHuffmanPsiSizeBitsUptoPosition(&myicsa->hcPsi, n/3);
		sizesPSIQuartil[1] = getHuffmanPsiSizeBitsUptoPosition(&myicsa->hcPsi, 2*n/3) - sizesPSIAcummQuartil[0];
		sizesPSIQuartil[2] = getHuffmanPsiSizeBitsUptoPosition(&myicsa->hcPsi, n-1) - sizesPSIAcummQuartil[1];
	
		sizesPSIQuartil[0] /=8;  //(in bytes);
		sizesPSIQuartil[1] /=8;  //(in bytes);
		sizesPSIQuartil[2] /=8;  //(in bytes);

		sizesPSIAcummQuartil[0] /=8;  //(in bytes);
		sizesPSIAcummQuartil[1] /=8;  //(in bytes);
		sizesPSIAcummQuartil[2] /=8;  //(in bytes);
	#endif

	#ifdef PSI_VBYTERLE

		unsigned long  getVbytePsiSizeBitsUptoPosition(VbyteCompressedPsi *cPsi, size_t position);
		unsigned long sizesPSIQuartil[3];
		unsigned long sizesPSIAcummQuartil[3];
		size_t n = myicsa->suffixArraySize;

		sizesPSIAcummQuartil[0] = getVbytePsiSizeBitsUptoPosition(&myicsa->vbPsi, n/3); 
		sizesPSIAcummQuartil[1] = getVbytePsiSizeBitsUptoPosition(&myicsa->vbPsi, 2*n/3);
		sizesPSIAcummQuartil[2] = getVbytePsiSizeBitsUptoPosition(&myicsa->vbPsi, n-1);
	
		sizesPSIQuartil[0] = getVbytePsiSizeBitsUptoPosition(&myicsa->vbPsi, n/3);
		sizesPSIQuartil[1] = getVbytePsiSizeBitsUptoPosition(&myicsa->vbPsi, 2*n/3) - sizesPSIAcummQuartil[0];
		sizesPSIQuartil[2] = getVbytePsiSizeBitsUptoPosition(&myicsa->vbPsi, n-1) - sizesPSIAcummQuartil[1];
	
		sizesPSIQuartil[0] /=8;  //(in bytes);
		sizesPSIQuartil[1] /=8;  //(in bytes);
		sizesPSIQuartil[2] /=8;  //(in bytes);

		sizesPSIAcummQuartil[0] /=8;  //(in bytes);
		sizesPSIAcummQuartil[1] /=8;  //(in bytes);
		sizesPSIAcummQuartil[2] /=8;  //(in bytes);
	#endif

	#ifdef PSI_R3H

		unsigned long  getPsiR3HSizeBitsUptoPosition(CompressedPsiR3H *cPsi, size_t position);
		unsigned long sizesPSIQuartil[3];
		unsigned long sizesPSIAcummQuartil[3];
		size_t n = myicsa->suffixArraySize;

		sizesPSIAcummQuartil[0] = getPsiR3HSizeBitsUptoPosition(&myicsa->hcPsi, n/3);
		sizesPSIAcummQuartil[1] = getPsiR3HSizeBitsUptoPosition(&myicsa->hcPsi, 2*n/3);
		sizesPSIAcummQuartil[2] = getPsiR3HSizeBitsUptoPosition(&myicsa->hcPsi, n-1);
	
		sizesPSIQuartil[0] = getPsiR3HSizeBitsUptoPosition(&myicsa->hcPsi, n/3);
		sizesPSIQuartil[1] = getPsiR3HSizeBitsUptoPosition(&myicsa->hcPsi, 2*n/3) - sizesPSIAcummQuartil[0];
		sizesPSIQuartil[2] = getPsiR3HSizeBitsUptoPosition(&myicsa->hcPsi, n-1) - sizesPSIAcummQuartil[1];
	
		sizesPSIQuartil[0] /=8;  //(in bytes);
		sizesPSIQuartil[1] /=8;  //(in bytes);
		sizesPSIQuartil[2] /=8;  //(in bytes);

		sizesPSIAcummQuartil[0] /=8;  //(in bytes);
		sizesPSIAcummQuartil[1] /=8;  //(in bytes);
		sizesPSIAcummQuartil[2] /=8;  //(in bytes);
	#endif

	#ifdef PSI_R3HYBRID

		unsigned long  getPsiR3HybridSizeBitsUptoPosition(CompressedPsiR3Hybrid *cPsi, size_t position);
		unsigned long sizesPSIQuartil[3];
		unsigned long sizesPSIAcummQuartil[3];
		size_t n = myicsa->suffixArraySize;

		sizesPSIAcummQuartil[0] = getPsiR3HybridSizeBitsUptoPosition(&myicsa->hcPsi, n/3);
		sizesPSIAcummQuartil[1] = getPsiR3HybridSizeBitsUptoPosition(&myicsa->hcPsi, 2*n/3);
		sizesPSIAcummQuartil[2] = getPsiR3HybridSizeBitsUptoPosition(&myicsa->hcPsi, n-1);
	
		sizesPSIQuartil[0] = getPsiR3HybridSizeBitsUptoPosition(&myicsa->hcPsi, n/3);
		sizesPSIQuartil[1] = getPsiR3HybridSizeBitsUptoPosition(&myicsa->hcPsi, 2*n/3) - sizesPSIAcummQuartil[0];
		sizesPSIQuartil[2] = getPsiR3HybridSizeBitsUptoPosition(&myicsa->hcPsi, n-1) - sizesPSIAcummQuartil[1];
	
		sizesPSIQuartil[0] /=8;  //(in bytes);
		sizesPSIQuartil[1] /=8;  //(in bytes);
		sizesPSIQuartil[2] /=8;  //(in bytes);

		sizesPSIAcummQuartil[0] /=8;  //(in bytes);
		sizesPSIAcummQuartil[1] /=8;  //(in bytes);
		sizesPSIAcummQuartil[2] /=8;  //(in bytes);
	#endif


	#ifdef PSI_PLAIN
		unsigned long getPlainPsiSizeBitsUptoPosition (PlainPsi *plPsi, size_t position);
		unsigned long sizesPSIQuartil[3];
		unsigned long sizesPSIAcummQuartil[3];
		size_t n = myicsa->suffixArraySize;

		sizesPSIAcummQuartil[0] = getPlainPsiSizeBitsUptoPosition(&myicsa->plPsi, n/3);
		sizesPSIAcummQuartil[1] = getPlainPsiSizeBitsUptoPosition(&myicsa->plPsi, 2*n/3);
		sizesPSIAcummQuartil[2] = getPlainPsiSizeBitsUptoPosition(&myicsa->plPsi, n-1);
	
		sizesPSIQuartil[0] = getPlainPsiSizeBitsUptoPosition(&myicsa->plPsi, n/3);
		sizesPSIQuartil[1] = getPlainPsiSizeBitsUptoPosition(&myicsa->plPsi, 2*n/3) - sizesPSIAcummQuartil[0];
		sizesPSIQuartil[2] = getPlainPsiSizeBitsUptoPosition(&myicsa->plPsi, n-1) - sizesPSIAcummQuartil[1];
	
		sizesPSIQuartil[0] /=8;  //(in bytes);
		sizesPSIQuartil[1] /=8;  //(in bytes);
		sizesPSIQuartil[2] /=8;  //(in bytes);

		sizesPSIAcummQuartil[0] /=8;  //(in bytes);
		sizesPSIAcummQuartil[1] /=8;  //(in bytes);
		sizesPSIAcummQuartil[2] /=8;  //(in bytes);
	#endif









	totalD   = (sizeof(uint)*((myicsa->suffixArraySize+31)/32));
	totalBD  = myicsa->bD->mem_usage;
	
//@@	totalSA  = (sizeof(uint) * myicsa->samplesASize); 
//@@	totalSAinv = (sizeof(uint) * myicsa->samplesAInvSize); 
//@@	totalBA  = (sizeof(uint)*((myicsa->suffixArraySize+31)/32));
//@@	totalBBA = myicsa->bBA->mem_usage;
	
	size_t nbytes; sizeIntIndex(index, &nbytes); //whole self-index
	
	printf("\n ===================================================:");		
	printf("\n%sSummary Self-index on integers (icsa) layer:",tab);		
	printf("\n%s   icsa structure = %zu bytes",tab, structure);
	printf("\n%s    psi          = %10zu bytes",tab, totalpsi);
	printf("\n%s    -D (bitmap)  = %10zu bytes",tab, totalD);
	printf("\n%s    -rank for D  = %10zu bytes",tab, totalBD);
//@NOLONGERRGK-2023	printf("\n%s    +D+bitmapRGK = %10zu bytes",tab,myicsa->Dmap->getSize());
	printf("\n%s    +Dselect1Predicates table = %10zu bytes",tab,myicsa->numPredicates *sizeof(uint));
	

	
//@@	printf("\n%s   SA(sampled) = %10u bytes",tab, totalSA);
//@@	printf("\n%s   SAinv(samp) = %10u bytes",tab, totalSAinv);
	//@@printf("\n%s   BA (bitmap) = %10u bytes",tab, totalBA);
//@@	printf("\n%s   rank for BA = %10u bytes",tab, totalBBA);
	printf("\n%sTotal = ** %zu bytes (in RAM) ** ",tab, nbytes);
	printf("\n");
	
	printf("\n ===================================================:");		
	printf("\n ===================================================:");		
	printf("\n%sSummary Self-index on integers (PSI) :",tab);		
	printf("\n%s   icsa structure = %zu bytes",tab, structure);
	printf("\n%s   psi         = %10zu bytes",tab, totalpsi);
	#ifdef PSI_HUFFMANRLE
		printf("\n%s  psi ... Size at pos psi[T1  (00-33%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[0], sizesPSIAcummQuartil[0]);
		printf("\n%s  psi ... Size at pos psi[T2  (33-66%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[1], sizesPSIAcummQuartil[1]);
		printf("\n%s  psi ... Size at pos psi[T3  (66-100%%)]  = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[2], sizesPSIAcummQuartil[2]);
	#endif
	#ifdef PSI_VBYTERLE
		printf("\n%s  psi ... Size at pos psi[T1  (00-33%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[0], sizesPSIAcummQuartil[0]);
		printf("\n%s  psi ... Size at pos psi[T2  (33-66%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[1], sizesPSIAcummQuartil[1]);
		printf("\n%s  psi ... Size at pos psi[T3  (66-100%%)]  = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[2], sizesPSIAcummQuartil[2]);
	#endif	
	#ifdef PSI_R3H
		printf("\n%s  psi ... Size at pos psi[T1  (00-33%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[0], sizesPSIAcummQuartil[0]);
		printf("\n%s  psi ... Size at pos psi[T2  (33-66%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[1], sizesPSIAcummQuartil[1]);
		printf("\n%s  psi ... Size at pos psi[T3  (66-100%%)]  = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[2], sizesPSIAcummQuartil[2]);
	#endif
	#ifdef PSI_R3HYBRID
		printf("\n%s  psi ... Size at pos psi[T1  (00-33%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[0], sizesPSIAcummQuartil[0]);
		printf("\n%s  psi ... Size at pos psi[T2  (33-66%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[1], sizesPSIAcummQuartil[1]);
		printf("\n%s  psi ... Size at pos psi[T3  (66-100%%)]  = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[2], sizesPSIAcummQuartil[2]);
	#endif

	#ifdef PSI_PLAIN
		printf("\n%s  psi ... Size at pos psi[T1  (00-33%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[0], sizesPSIAcummQuartil[0]);
		printf("\n%s  psi ... Size at pos psi[T2  (33-66%%)]   = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[1], sizesPSIAcummQuartil[1]);
		printf("\n%s  psi ... Size at pos psi[T3  (66-100%%)]  = %10zu bytes (accum = %10zu bytes)",tab, (size_t) sizesPSIQuartil[2], sizesPSIAcummQuartil[2]);
	#endif

	
	
	printf("\n ===================================================:");		
	printf("\n ===================================================:");		
	
	return 0; //no error.
}











/*FARI: CORRECCIÓN FEITA EN XUÑO DO 2019 */
/* Chamar a getPsiBuffericsa(wcsa, B,l,r) ten sentido cando l e r están incluídos entre 2 samples consecutivas
 * pois a búsqueda binaria que se fai nese intervalo faise usando que o primeiro uint dos sufixos que están nese
 * intervalo xa son coñecidos... e así susamos SadCSACompareON() en lugar de SadCSACompare().
 * Sen embargo, non o ten cando l e r poden estar precedidos por "samples moi distantes"... pois nese caso é preferible
 * facer a búsqueda binaria normal usando SadCSACompare().
 * Por iso, modifiquei countIntIndex_Buffer para que:
 *   + só cando p ==previous P (i.e. l e r entre 2 samples consecutivas) se chama a getPsiBuffericsa.
 *      - neste caso, o buffer B ten tamaño = tPsi+1, e resérvase con malloc dinámicamente.
 *   + no outro caso, non se usa buffer B.
 * 
 * O CÓDIGO ANTERIOR (ver función countIntIndex_Bufferprev2019() abaixo), que tiña un buffer B reservado estáticamente, 
 * falla na operación ?PO na dbpedia2015 porque o número "r-l+1" era moi grande e B desbordaba. En concreto cando se buscaba
 * S;  P ;  O     ;x
 * 0;1396;91336013;0
 * 0;1396;69515009;0
 * */ 

/* AS THE NEXT ONE, BUT WHEN WE ARE BETWEEN 2 SAMPLES, WE DECODE PSI(L,R) INTO A 
 * TEMPORAL BUFFER B... SO THAT CSACOMPARE USES B[i] FOR THE COMPARISON OF THE FIRST
 * INTEGER OF THE SUFFIX COMPARED.
 */
//int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	
int countIntIndex_Buffer(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	

//	static uint B[4096*4]; unsigned long iniB;    //FARI 2019!! ERROR !! ... the buffer used for the last "binary search within the end of the interval obtained with the exponential serach could Be > 4096x4 integers !!!)
	
	//static uint B[4096*8192]; 
	  unsigned long iniB;

	uint patternSize = length;
	ticsa *myicsa = (ticsa *) index;
	
	register unsigned long l, r, i; 
	register long comp, p, previousP;
	register uint bin_search_psi_skip_interval = myicsa->T_Psi;
		
	l = 0; 
	r = (myicsa->suffixArraySize+bin_search_psi_skip_interval-2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	p = ((l+r)/2)/bin_search_psi_skip_interval * bin_search_psi_skip_interval;
	previousP = 0;
	
	// BUSCA BINARIA SOBRE MOSTRAS
	while( (p != previousP) && (comp = SadCSACompare(myicsa, pattern, patternSize, p)) ) {
		if(comp > 0) l = p;
		else r = p;
		previousP = p;
		p = ((l+r)/2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	}

	if(p==previousP) {
		uint * B= (uint *)malloc(sizeof(uint) * (bin_search_psi_skip_interval+1));
	
		// BUSCA BINARIA ENTRE O PIVOTE E A SEGUINTE MOSTRA
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		
		
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		getPsiBuffericsa(myicsa,B,l,r);iniB=l;
		//printf("\n to bin search  [%u, %u]", l,r);fflush(stdout);
		while(l < r) {
			p = (l+r)/2; 			 			 
			
			if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) <= 0) r = p;
			//if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;			
			else l = p+1;
		}

		if(SadCSACompareSecondOn(myicsa, pattern, patternSize, r, B[r-iniB])) {
		//if(SadCSACompare(myicsa, pattern, patternSize, r)) {
			*left = l;
			*right = r;
			//return 0;
			*numocc = 0;
			free(B);
			return 0; //no error.
		}
		*left = r;
	
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		//getPsiBuffericsa(myicsa,B,l,r);	iniB=l;
		while(l < r) {
			p = (l+r+1)/2;
			//if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) >= 0) l=p;
			else r = p-1;	
		}
		*right = l;
		free(B);
		
	} else {
		
		previousP = p;	// En previousP poñemos o p atopado na busca sobre as mostras
		
		
		// BUSCA EXPONENCIAL HACIA ATRÁS
		i = 1;
		p -= bin_search_psi_skip_interval;
		while(p>0 && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP-(i*bin_search_psi_skip_interval);
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(previousP > i*bin_search_psi_skip_interval) l = previousP-(i*bin_search_psi_skip_interval);
		else l=0;
		i>>=1;		
		r = previousP-(i*bin_search_psi_skip_interval);
		//getPsiBuffericsa(myicsa,B,l,r);iniB=l;
		while(l < r) {
			p = (l+r)/2; 
			if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
			//if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) <= 0) r = p;
			else l = p+1;
		}
		*left = r;
		
		// BUSCA EXPONENCIAL HACIA ADIANTE
		i = 1;
		p = previousP+bin_search_psi_skip_interval;
		while(p<myicsa->suffixArraySize && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP+(i*bin_search_psi_skip_interval);		
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(p < myicsa->suffixArraySize) r = previousP+(i*bin_search_psi_skip_interval);
		else r = myicsa->suffixArraySize-1;
		i>>=1;		
		l = previousP+(i*bin_search_psi_skip_interval);
		//getPsiBuffericsa(myicsa,B,l,r);iniB=l;
		while(l < r) {
			p = (l+r+1)/2;
			if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			//if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) >= 0) l = p;
			else r = p-1;	
		}
		*right = l;	
	}

	//return *right-*left+1;
	*numocc = (uint) *right-*left+1;
	return 0; //no error		
}





/* FARI 2019, A SEGUINTE FUNCIÓN ERA Á QUE TIÑA O BUG, E XA NON SE USA  */
/* AS THE NEXT ONE, BUT WHEN WE ARE BETWEEN 2 SAMPLES, WE DECODE PSI(L,R) INTO A 
 * TEMPORAL BUFFER B... SO THAT CSACOMPARE USES B[i] FOR THE COMPARISON OF THE FIRST
 * INTEGER OF THE SUFFIX COMPARED.
 */
//int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	
int countIntIndex_Bufferprev2019(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	

	static uint B[4096*4]; unsigned long iniB;

	uint patternSize = length;
	ticsa *myicsa = (ticsa *) index;
	
	register unsigned long l, r, i; 
	register long comp, p, previousP;
	register uint bin_search_psi_skip_interval = myicsa->T_Psi;
		
	l = 0; 
	r = (myicsa->suffixArraySize+bin_search_psi_skip_interval-2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	p = ((l+r)/2)/bin_search_psi_skip_interval * bin_search_psi_skip_interval;
	previousP = 0;
	
	// BUSCA BINARIA SOBRE MOSTRAS
	while( (p != previousP) && (comp = SadCSACompare(myicsa, pattern, patternSize, p)) ) {
		if(comp > 0) l = p;
		else r = p;
		previousP = p;
		p = ((l+r)/2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	}

	if(p==previousP) {
	
		// BUSCA BINARIA ENTRE O PIVOTE E A SEGUINTE MOSTRA
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		
		
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		getPsiBuffericsa(myicsa,B,l,r);iniB=l;
		//printf("\n to bin search  [%u, %u]", l,r);fflush(stdout);
		while(l < r) {
			p = (l+r)/2; 			 			 
			
			if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) <= 0) r = p;
			//if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;			
			else l = p+1;
		}

		if(SadCSACompareSecondOn(myicsa, pattern, patternSize, r, B[r-iniB])) {
		//if(SadCSACompare(myicsa, pattern, patternSize, r)) {
			*left = l;
			*right = r;
			//return 0;
			*numocc = 0;
			return 0; //no error.
		}
		*left = r;
	
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		getPsiBuffericsa(myicsa,B,l,r);	iniB=l;
		while(l < r) {
			p = (l+r+1)/2;
			//if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) >= 0) l=p;
			else r = p-1;	
		}
		*right = l;

		
	} else {
		
		previousP = p;	// En previousP poñemos o p atopado na busca sobre as mostras
		
		
		// BUSCA EXPONENCIAL HACIA ATRÁS
		i = 1;
		p -= bin_search_psi_skip_interval;
		while(p>0 && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP-(i*bin_search_psi_skip_interval);
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(previousP > i*bin_search_psi_skip_interval) l = previousP-(i*bin_search_psi_skip_interval);
		else l=0;
		i>>=1;		
		r = previousP-(i*bin_search_psi_skip_interval);
		getPsiBuffericsa(myicsa,B,l,r);iniB=l;
		while(l < r) {
			p = (l+r)/2; 
			//if(SadCSACompare(myicsa, pattern, patte// OPERACIONS DO CSA

// BUSCA BINARIA SOBRE MOSTRAS + 2 BUSCAS EXPONENCIAIS + 2 BUSCAS BINARIAS
// 1º Busca binaria sobre o array de sufixos, elexindo como pivote un múltiplo de bin_search_psi_skip_interval (que orixinalmente foi pensado para igualar co valor de Psi).
// 2º Esta busca pode deterse por:
//	a) O pivote repítese entre dúas iteracións -> As ocorrencias están entre o pivote e a seguinte mostra (pivote + bin_search_psi_skip_interval) -> facemos dúas buscas binarias
//	b) O pivote é unha ocorrencia do patrón -> Faise unha busca exponencial sobre mostras hacia a esquerda e outra hacia a dereita, ata atopar a unha mostra á esquerda e outra
//	á dereita do intervalo de ocorrencias. Entre cada unha destas e a inmediatamente anterior da busca exponencial, faise unha busca binaria para atopar os extremos do intervalo.rnSize, p) <= 0) r = p;
			if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) <= 0) r = p;
			else l = p+1;
		}
		*left = r;
		
		// BUSCA EXPONENCIAL HACIA ADIANTE
		i = 1;
		p = previousP+bin_search_psi_skip_interval;
		while(p<myicsa->suffixArraySize && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP+(i*bin_search_psi_skip_interval);		
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(p < myicsa->suffixArraySize) r = previousP+(i*bin_search_psi_skip_interval);
		else r = myicsa->suffixArraySize-1;
		i>>=1;		
		l = previousP+(i*bin_search_psi_skip_interval);
		getPsiBuffericsa(myicsa,B,l,r);iniB=l;
		while(l < r) {
			p = (l+r+1)/2;
			//if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			if(SadCSACompareSecondOn(myicsa, pattern, patternSize, p, B[p-iniB]) >= 0) l = p;
			else r = p-1;	
		}
		*right = l;	
	}

	//return *right-*left+1;
	*numocc = (uint) *right-*left+1;
	return 0; //no error		
}







































// probado con Vpo, pero no mejoraba, y en Vpo0 non funcionaba (no encontraba nada)
// /************************* DANIIL STARTS **********************************/
// /************************* DANIIL STARTS **********************************/
// /************************* DANIIL STARTS **********************************/
// /************************* Backward search ********************************/
// 
// 
// uint inline locateSymbol(const ticsa *myicsa, const uint i) {
// 	//return i+1 > myicsa->bD->pop ? myicsa->bD->n-1 : bselect(myicsa->bD, i+1);
// 	#ifdef BITMAP_RGK	
// 		return ((i+1)>myicsa->Dmap->countOnes()) ? myicsa->Dmap->getLength()-1: myicsa->Dmap->select1(i+1);
// 	#else
// 		return i+1 > myicsa->bD->pop ? myicsa->bD->n-1 : bselect(myicsa->bD, i+1);
// 	#endif	
// }
// 
// uint inline DaniilgetPsiValue(ticsa *myicsa, const uint i) {
// 
// 		#ifdef PSI_HUFFMANRLE
// 			return getHuffmanPsiValue(&(myicsa->hcPsi), i );
// 		#endif			 
// 		#ifdef PSI_GONZALO
// 			return getGonzaloPsiValue(&(myicsa->gcPsi), i);
// 		#endif
// 		#ifdef PSI_DELTACODES
// 			return getDeltaPsiValue(&(myicsa->dcPsi), i);
// 		#endif	
// 		#ifdef PSI_VBYTERLE
// 			return getVbytePsiValue(&(myicsa->vbPsi), i );
// 		#endif			
// 		#ifdef PSI_R3H
// 			return getPsiR3HValue(&(myicsa->hcPsi), i );
// 		#endif		
//		#ifdef PSI_R3HYBRID
//			return getPsiR3HybridValue(&(myicsa->hcPsi), i );
//		#endif		
//		#ifdef PSI_PLAIN
//			return getPlainPsiValue(&(myicsa->plPsi), i );
//		#endif
//
// }
// 
// void inline DaniilgetPsiBuffericsa(ticsa *myicsa , uint *buffer, size_t ini, size_t end) {
// 		#ifdef PSI_HUFFMANRLE
// 			getHuffmanPsiValueBuffer(&(myicsa->hcPsi), buffer,ini,end);
// 		#endif			 
// 		#ifdef PSI_GONZALO
// 			getGonzaloPsiValueBuffer(&(myicsa->gcPsi), buffer,ini,end);
// 		#endif
// 		#ifdef PSI_DELTACODES
// 			getDeltaPsiValueBuffer(&(myicsa->dcPsi), buffer,ini,end);
// 		#endif	
// 		#ifdef PSI_VBYTERLE
// 			getVbytePsiValueBuffer(&(myicsa->vbPsi), buffer,ini,end);
// 		#endif			
//		#ifdef PSI_R3H
//			getPsiR3HValueBuffer(&(myicsa->hcPsi), buffer,ini,end);
//		#endif		
//		#ifdef PSI_R3HYBRID
//			getPsiR3HybridValueBuffer(&(myicsa->hcPsi), buffer,ini,end);
//		#endif		
// 		#ifdef PSI_PLAIN
// 			getPlainPsiValueBuffer(&(myicsa->plPsi), buffer,ini,end);
// 		#endif			 
// 	return;
// }
// 
// 
// 
// /* somehow back-ward search */
// int countIntIndexDANIIL(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right) {
// 	*numocc = 0;
// 
// 	if (length == 0)
// 		return 0;
// 
// 	ticsa *myicsa = (ticsa *) index;
// 	uint skip = myicsa->T_Psi;
// 	uint buffer[skip+1];
// 	register unsigned long l, r, i, lv, rv, k;
// 	//unsigned long sl, sr;
// 
// 	if (*left == 0) {
// 		*left = locateSymbol(myicsa, pattern[length-1]);
// 	}
// 
// 	if (*right == 0) {
// 		*right = locateSymbol(myicsa, pattern[length-1]+1) - 1;
// 	}
// 
// 	for (i = length-1; i > 0; i--) {
// 		l = locateSymbol(myicsa, pattern[i-1]);
// 		r = locateSymbol(myicsa, pattern[i-1]+1) - 1;
// 		lv = r;
// 		rv = l;
// 
// 		// printf("%lu %lu %lu\n", pattern[i], *left, *right);
// 
// 		int j=0;
// 		if (l > 10) {
// 			j = -10;
// 		}
// 
// 		// LEFT BINARY SEARCH
// 		while (l < lv && l <= (k = ((l+lv)/2)/skip*skip)) {
// 			// printf("\t\t%lu %lu %lu\n", l, k, lv);
// 
// 			if (DaniilgetPsiValue(myicsa, k) < *left) {
// 				l = k+skip;
// 			} else {
// 				lv = k;
// 			}
// 		}
// 
// 		if (l >= skip && l >= lv)
// 			l -= skip;
// 
// 		if (l < rv)
// 			l += skip;
// 
// 		// printf("\tLEFT %lu %lu %lu %lu\n", l, lv, getPsiValue(myicsa, l), getPsiValue(myicsa, lv));
// 
// 		if (l <= r) {
// 			// LEFT BINARY REFINE
// 			lv = lv-l > skip ? skip : lv-l+1; // right limit
// 			DaniilgetPsiBuffericsa(myicsa, buffer, l, l+lv);
// 			k = 0; // left limit
// 
// 			/*
// 			for (int j=250; j<lv; j++) {
// 				printf("%u ", buffer[j]);
// 			}
// 			printf("\n");
// 			*/
// 
// 			while (k < lv) {
// 				if (buffer[(k+lv)/2] < *left) {
// 					k = (k+lv)/2 + 1;
// 				} else {
// 					lv = (k+lv)/2;
// 				}
// 			}
// 
// 			l = l + k;
// 			lv = buffer[k];
// 
// 			if (lv < *left) {
// 				lv = *right+1; // NOT FOUND
// 			}
// 
// 			// printf("\tLEFT %lu:%lu\n", l, lv);
// 
// 			if (lv <= *right) {
// 				// RIGHT BINARY SEARCH
// 				lv = rv;
// 				rv = r;
// 				r = l;
// 				k = ((rv+r)/2)/skip*skip;
// 
// 				if (r > k && k+skip < rv) {
// 					k += skip;
// 				}
// 
// 				while (r < rv && r <= k) {
// 					// printf("\t\t%lu %lu %lu\n", r, k, rv);
// 
// 					if (DaniilgetPsiValue(myicsa, k) > *right) {
// 						rv = k;
// 					} else {
// 						r = k + skip;
// 					}
// 
// 					k = ((rv+r)/2)/skip*skip;
// 				}
// 
// 				if (r >= skip && r >= rv && r-skip >= l)
// 					r -= skip;
// 
// 				// printf("\tRIGHT %lu %lu\n", r, rv);
// 
// 				if (l <= rv) {
// 					// RIGHT BINARY REFINE
// 					rv = rv-r >= skip ? skip-1 : rv-r; // right limit
// 					DaniilgetPsiBuffericsa(myicsa, buffer, r, r+rv);
// 					k = 0;
// 					// printf("\t\t%u %u %u\n", r, k, rv);
// 
// 					while (k < rv) {
// 						if (buffer[(k+rv+1)/2] > *right) {
// 							rv = (k+rv+1)/2 - 1;
// 						} else {
// 							k = (k+rv+1)/2;
// 						}
// 					}
// 
// 					r = r + k;
// 					rv = buffer[k];
// 					// printf("\tRIGHT %lu:%lu\n", r, rv);
// 				}
// 			} else {
// 				*left = l;
// 				*right = l;
// 				return 0;
// 			}
// 		}
// 
// 		*left = l;
// 		*right = r;
// 
// 		if (l > r) {
// 			return 0;
// 		}
// 	}
// 
// 	// printf("%lu %lu %lu\n", pattern[0], *left, *right);
// 	*numocc = *right-*left+1;
// 	return 0;
// }


/************************* DANIIL BACKWARD SEARCH ***********************/
/************************* DANIIL ENDS **********************************/
/************************* DANIIL ENDS **********************************/



	
//TESTFari_28abril   //tardaba +- lo mismo que el de abajo en "PLAIN"!
int countIntIndexXX(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	
	ticsa *myicsa = (ticsa *) index;
	uint l,r,occs;
	*numocc = countCSABin(myicsa,pattern,length,&l,&r);  //l,r must be uint *
	*left=l; *right=r;
	return 0;
	
}

// OPERACIONS DO CSA

// BUSCA BINARIA SOBRE MOSTRAS + 2 BUSCAS EXPONENCIAIS + 2 BUSCAS BINARIAS
// 1º Busca binaria sobre o array de sufixos, elexindo como pivote un múltiplo de bin_search_psi_skip_interval (que orixinalmente foi pensado para igualar co valor de Psi).
// 2º Esta busca pode deterse por:
//	a) O pivote repítese entre dúas iteracións -> As ocorrencias están entre o pivote e a seguinte mostra (pivote + bin_search_psi_skip_interval) -> facemos dúas buscas binarias
//	b) O pivote é unha ocorrencia do patrón -> Faise unha busca exponencial sobre mostras hacia a esquerda e outra hacia a dereita, ata atopar a unha mostra á esquerda e outra
//	á dereita do intervalo de ocorrencias. Entre cada unha destas e a inmediatamente anterior da busca exponencial, faise unha busca binaria para atopar os extremos do intervalo.

int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right){	
	//unsigned int countCSA(ticsa *myicsa, uint *pattern, uint patternSize, uint *left, uint *right) {

	uint patternSize = length;
	ticsa *myicsa = (ticsa *) index;
	
	register unsigned long l, r, i; 
	register long comp, p, previousP;
	//register unsigned int l, r, i;
	//register int comp, p, previousP;
	register uint bin_search_psi_skip_interval = myicsa->psiSearchFactorJump;
	
	//fprintf(stderr,"\n psiSearchFactor = %d",myicsa->psiSearchFactorJump);
	
	l = 0; 
	r = (myicsa->suffixArraySize+bin_search_psi_skip_interval-2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	p = ((l+r)/2)/bin_search_psi_skip_interval * bin_search_psi_skip_interval;
	previousP = 0;
	
	// BUSCA BINARIA SOBRE MOSTRAS
	while( (p != previousP) && (comp = SadCSACompare(myicsa, pattern, patternSize, p)) ) {
		if(comp > 0) l = p;
		else r = p;
		previousP = p;
		p = ((l+r)/2)/bin_search_psi_skip_interval*bin_search_psi_skip_interval;
	}

	if(p==previousP) {
	
		// BUSCA BINARIA ENTRE O PIVOTE E A SEGUINTE MOSTRA
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		while(l < r) {
			p = (l+r)/2; 
			if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
			else l = p+1;
		}

		if(SadCSACompare(myicsa, pattern, patternSize, r)) {
			*left = l;
			*right = r;
			//return 0;
			*numocc = 0;
			return 0; //no error.
		}
		*left = r;
	
		l = previousP; 
		r = previousP+bin_search_psi_skip_interval;
		if(r > myicsa->suffixArraySize) r = myicsa->suffixArraySize - 1;
		while(l < r) {
			p = (l+r+1)/2;
			if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			else r = p-1;	
		}
		*right = l;
		
	} else {
		
		previousP = p;	// En previousP poñemos o p atopado na busca sobre as mostras
		
		// BUSCA EXPONENCIAL HACIA ATRÁS
		i = 1;
		p -= bin_search_psi_skip_interval;
		while(p>0 && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP-(i*bin_search_psi_skip_interval);
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(previousP > i*bin_search_psi_skip_interval) l = previousP-(i*bin_search_psi_skip_interval);
		else l=0;
		i>>=1;		
		r = previousP-(i*bin_search_psi_skip_interval);
		while(l < r) {
			p = (l+r)/2; 
			if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
			else l = p+1;
		}
		*left = r;
		
		// BUSCA EXPONENCIAL HACIA ADIANTE
		i = 1;
		p = previousP+bin_search_psi_skip_interval;
		while(p<myicsa->suffixArraySize && !SadCSACompare(myicsa, pattern, patternSize, p)) {
			i<<=1;
			p = previousP+(i*bin_search_psi_skip_interval);		
		}
		// Busca binaria entre as duas ultimas mostras da exponencial
		if(p < myicsa->suffixArraySize) r = previousP+(i*bin_search_psi_skip_interval);
		else r = myicsa->suffixArraySize-1;
		i>>=1;		
		l = previousP+(i*bin_search_psi_skip_interval);
		while(l < r) {
			p = (l+r+1)/2;
			if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
			else r = p-1;	
		}
		*right = l;	
	}

	//return *right-*left+1;
	*numocc = (uint) *right-*left+1;
	return 0; //no error		
}

// Version inicial de busca binaria
unsigned int countCSABin(ticsa *myicsa, uint *pattern, uint patternSize, uint *left, uint *right) {
	register ulong l, r, p;

	l = 0; 
	r = myicsa->suffixArraySize-1;

	while(l < r) {
		p = (l+r)/2; 
		if(SadCSACompare(myicsa, pattern, patternSize, p) <= 0) r = p;
		else l = p+1;
	}

	// SE SON DISTINTOS O PATRON NON APARECE NO TEXTO E DEVOLVEMOS 0
	if(SadCSACompare(myicsa, pattern, patternSize, r)) {
		*left = l;
		*right = r;
		return 0;
	}
	
	// Almacenamos o limite esquerdo
	*left = r;

	// SE SON IGUALES (O PATRON APARECE NO TEXTO), BUSCAMOS AGORA O LIMITE DEREITO, QUE ALMACENAREMOS EN right
	// NOTA: INICIAMOS A BUSQUEDA A PARTIR DO ESQUERDO...
	l = r; 
	r = myicsa->suffixArraySize-1;
	
	while(l < r) {
		p = (l+r+1)/2;
		if(SadCSACompare(myicsa, pattern, patternSize, p) >= 0) l = p;
		else r = p-1;	
	}
	
	// Gardamos o limite dereito
	*right = l;
	
	return (uint) *right-*left+1;	
}








/*******************************************************************************/

//finds the range [left-right] in a zone of increasing values of psi[left-right], such 
//that for any $i \in [left-right] $, psi(i) maps into [tl,tr] 
//no improvements... binary searches access psi at any (sampled/no-sampled) positions.
int binSearchPsiTarget(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr){	

	ticsa *myicsa = (ticsa *) index;
	register unsigned long l, r, p, psi_p;
	l=*left; r=*right;
	
	// looks for left limit	
	while(l < r) {
		p = (l+r)/2; 
		psi_p= getPsiicsa(myicsa, p);
		if (psi_p >= tl) r = p;
		else l = p+1;
	}
	// If there is no $i \in [*left, *right] such that psi(i)\in [tl,tr]$  we are done with no matches.
	psi_p= getPsiicsa(myicsa, r);
	if( (tl>psi_p) || (tr<psi_p)) {
		*left = l; *right = r; *numocc = 0;
		return 0;
	}
	// Left limit is saved.
	*left = r;

	// now finds right limit   (we know at least 1 match occurs if we arrived here).
	l = r;     //start from previous position
	r = *right;
	
	while(l < r) {
		p = (l+r+1)/2;
		psi_p= getPsiicsa(myicsa, p);
		if (psi_p <= tr) l=p;
		else r = p-1;	
	}
	
	// Gardamos o limite dereito
	*right = l;		
	*numocc = r-l+1;
	return 0;
}


//cheks if p falls int [tl,tr].
//returns -1 if p<tl, +1 if p>tr, 0 if p \in [tl,tr]
int cmptarget(ulong p, ulong tl, ulong tr) {
	if (p<tl) return -1;
	if (p>tr) return +1;
	return 0;
}


//finds the range [left-right] in a zone of increasing values of psi[left-right], such 
//that for any $i \in [left-right] $, psi(i) maps into [tl,tr] 
//optimizes the search by taking advantage of accessing sampled psi positions first 
//(at positions i = k *myicsa->T_Psi )
int binSearchPsiTarget_samplesFirst_BUFFER(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr){	

	ticsa *myicsa = (ticsa *) index;
	register uint tpsi = myicsa->T_Psi;
	//uint *B = myicsa->Buffer;
	static uint B[4096];
		

	register unsigned long l, r, sl, sr, p;
	l=*left; r=*right;
		
	sl=l/tpsi * tpsi; 
	sr=r/tpsi * tpsi;
	
	
	//no sample in [l,r] --------------------
	if ( (sl<l) && (sr<l)) {
		getPsiBuffericsa(myicsa,B,l,r);
		uint i; uint n=r-l+1;
		for (i=0; i<n;i++) if (B[i]>=tl) break;
		if (i>=n) {
			*left =l+i; *right=l+i; *numocc =0; return 0;
		}
		else {
			if (B[i]>tr) { //no occs
				*left =l+i; *right=l+i; *numocc =0; return 0;			
			}
			else { // B[i] <=tr
				*left =l+i;
				i++;
				for ( ; i<n; i++) if (B[i]>tr) break;
				i--;
				*right=l+i; *numocc = (*right - *left +1); return 0;
			}
		}
	}
	
	//only 1 sample in [l,r] --------------------
	else if ( ((sl<l) && (sr == (sl+tpsi)))  || ((sl ==l) && (sr==sl)) ) {

		if (sl==sr){   //l == sl == sr.
			p= getPsiicsa(myicsa, sl);    //sampled pos.
			int cmp = cmptarget(p,tl,tr);
			if ( cmp ==0 ) {
				getPsiBuffericsa(myicsa,B,l,r);
				uint i=1; uint n=r-l+1;				
				for ( ; i<n;i++) if (B[i]>tr) break;
				i--;
				*left=l; *right=l+i; *numocc = (*right - *left +1); return 0;				
			}
			else if ( cmp>0 ) {
				*left =l; *right=l; *numocc =0; return 0;			
			}
			else { // (cmp <0)
			
				getPsiBuffericsa(myicsa,B,l,r);
				uint i=1; uint n=r-l+1;
				for ( ; i<n;i++) if (cmptarget(B[i],tl,tr) >=0) break;
				if (i>=n) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				else if (cmptarget(B[i],tl,tr) >0) {
					*left =l+i; *right=l+i; *numocc =0; return 0;
				}
				else { //if (cmptarget(B[i],tl,tr) ==0) 
					*left =l+i;
					i++;
					for ( ; i<n; i++) if (B[i]>tr) break;
					i--;
					*right=l+i; *numocc = (*right - *left +1); return 0;
				}			
					
			}
		}
		
		else {  //sl <l   && sr=sl+tpsi
			p= getPsiicsa(myicsa, sr);    //sampled pos.
			if (p > tr) r=sr-1;
			
			getPsiBuffericsa(myicsa,B,l,r);
			uint i=0; uint n=r-l+1;
			for ( ; i<n;i++) if (cmptarget(B[i],tl,tr) >=0) break;

			if (i>=n) {
				*left =l; *right=l; *numocc =0; return 0;
			}
			else if (cmptarget(B[i],tl,tr) >0) {
				*left =l+i; *right=l+i; *numocc =0; return 0;
			}
			else { //if (cmptarget(B[i],tl,tr) ==0) 
				*left =l+i;
				i++;
				for ( ; i<n; i++) if (B[i]>tr) break;
				i--;
				*right=l+i; *numocc = (*right - *left +1); return 0;
			}				
 
		}
	}
		
	// 2+ samples in [l,r] --------------------
	//else 2+ samples en [l,r]
	else {
		int checkedleft=0;
		if (sl < l) {
			sl+=tpsi;
			p= getPsiicsa(myicsa, sl);
			if (p >tl) {
				checkedleft=1;
				uint i=0; uint n=sl -l +1;
				
				getPsiBuffericsa(myicsa,B,l,sl);
				for ( ; i<n;i++) if (cmptarget(B[i],tl,tr) >=0) break;
				
				if (i>=n) {
					*left =l; *right=l; *numocc =0; return 0;   // I think this could not happen as  p=psi(sl) >tl !!
				}
				else if (cmptarget(B[i],tl,tr) >0) {
					*left =l+i; *right=l+i; *numocc =0; return 0;
				}
				else { //if (cmptarget(B[i],tl,tr) ==0) 
					*left =l+i;
					//p= getPsiicsa(myicsa, sl);
					if (p > tr) {   // right should be within [l..sl] --> avoid binary searches ;)
						i++;
						for ( ; i<n; i++) if (B[i]>tr) break;
						i--;
						*right=l+i; *numocc = (*right - *left +1); return 0;  // [*left, *right] already determined :D:D
					}
					//else /* left already established and checkedleft==1 from here on.
											
				}													
			}				
		}
		
		ulong ll,rr;
		l=sl;  // now check for *right (and maybe *left) from sl on...
		       // l starts in a sampled position "sl"
		
		//--- looks for right in [l=sl,r --------------------------------------
		
		//case 1.. right should be after the last sample.
		if (cmptarget(getPsiicsa(myicsa, sr),tl,tr) <=0) {
			getPsiBuffericsa(myicsa,B,sr,r);
			int i; int n=r -sr +1;
			for (i=n-1; i>=0; i--) if (cmptarget(B[i], tl, tr)<=0) break;		
			if (i>=0) {
				if (cmptarget(B[i], tl, tr)<0) {
					*left =sr; *right=sr; *numocc =0; return 0;
				}
				else 
				    *right = sr+i;
			}
			else {
				assert(0);  //at least one value in [sr,r] should be <=0 as cmptarget(psi[sr]...) <=0;
			}
		}		
		else {//case 2... binary search
			r=sr;  //r=sr-1 actually ... hence looking for *right in [sl..sr]
			
			//binary search on the samples.			
			ll=l;rr=sr;
			assert(ll%tpsi == 0);
			assert(rr%tpsi == 0);
					

			ulong prevp=0;		
			//printf("\n ll=%lu, p= %lu, rr=%lu, prevp=%lu: cmptarget = %d",ll,((ll+rr)/2 /tpsi * tpsi),rr,prevp,cmptarget(getPsiicsa(myicsa, ((ll+rr)/2 /tpsi * tpsi)),tl,tr));
			while (ll<rr) {  // ll and rr should be multiples of tpsi								
				p = ((ll+rr)/2 /tpsi * tpsi);
				//printf("\n ll=%lu, p= %lu, rr=%lu, prevp=%lu: cmptarget = %d",ll,p,rr,prevp,cmptarget(getPsiicsa(myicsa, p),tl,tr));
				if (p==prevp) break;
				assert(p%tpsi == 0);
				if (cmptarget(getPsiicsa(myicsa, p),tl,tr) <=0) 
					ll=p;
				else
					rr=p-tpsi;
				
				prevp=p;	

			}
			
			if ((rr > ll) && (cmptarget(p=getPsiicsa(myicsa, rr),tl,tr) <=0)) {
				// rr could be <ll if p == ll and rr=p+tpsi and cmptarget (ll) > 0 in the previous iteration
				// therefore we set rr = p-tpsi	... and the loop ends because ll>rr			
				sr=rr;
			}
			else {
				sr=ll; p=getPsiicsa(myicsa, sr);
			}
			//printf("\n final sr= %lu, and psi(sr) = %lu",sr,p);fflush(stdout);
			
			if (cmptarget(p,tl,tr) >0) {   //todos los samples >tr --> no match.
				*left =sr; *right=sr; *numocc =0; return 0;
			}
			//else if ((sr==r) && (cmptarget(p,tl,tr) ==0)) {
			//	*right = sr;
			//}
			//else {  //SIN CHEQUEAR, CREO QUE HAI ALGO MAL AQUÍ.
			//	getPsiBuffericsa(myicsa,B,sr,r);
			//	uint i=0; uint n=r -sr +1;
			//	for ( ; i<n;i++) if (B[i] > tr) break;
			//	i--;
			//	*right = sr+i;			
			//}
			else {   // cmptarget(p,tl,tr) <=0)... 
			         // si hay un ==0, ese es right, otherwise no existe right (ni left)
			    
			    ulong end = ( (sr+tpsi < r) ? sr+tpsi : r );
				getPsiBuffericsa(myicsa,B,sr,end);
				int i; int n=end -sr +1;
				for (i=n-1; i>=0; i--) if (cmptarget(B[i], tl, tr)<=0) break;		
				if (i>=0) {
					if (cmptarget(B[i], tl, tr)<0) {
						*left =sr; *right=sr; *numocc =0; return 0;
					}
					else { //cmptarget(B[i], tl, tr)==0
						*right = sr+i;
						
							//if *left must BE within the uncompressed psi values... 
							//--> search sequentially in B[0..i], so we avoid the 
							//                         binary search for it below
							//note that cmptarget(B[i],tl,tr) ==0 at this point.
							if ((checkedleft==0) && (cmptarget(B[0], tl, tr)<0)) {							
								for (; i>0; i--) if (cmptarget(B[i-1], tl, tr)<0) break;
								*left = sr+i;
								*numocc = *right -*left +1; 
								return 0;
							}
							//
							//
					}
				}
				else {
					assert(0);  //at least one value in [sr,r] should be <=0 
					            //as cmptarget(psi[sr]...) <=0;
				}
			}
		}	
		
		
		// IF YOU REACHED HERE, *right is already known.
		// LEFT is unknown (but within [sl,*right]) if checkleft==0.
		// LEFT is known if (checkleft==1).
		
		//-----------------
		if (checkedleft == 0) {  // buscar *left en [sl,*right];
			r= *right;
			ll= sl; rr = r/tpsi * tpsi;
			while (ll < rr) {
				p = ((ll+rr)/2 /tpsi * tpsi);
				if (cmptarget(getPsiicsa(myicsa, p),tl,tr) >=0) 
					rr=p;
				else
					ll=p+tpsi;				
			}
			
			p=rr;
			assert(p%tpsi == 0);			
			if ((p>sl) &&  (cmptarget(getPsiicsa(myicsa, p),tl,tr) >=0) ) {
				p-=tpsi;   //p--;
			}
			
			if (cmptarget(getPsiicsa(myicsa,p),tl,tr) ==0) {
				*left = p;
			}
			else if (cmptarget(getPsiicsa(myicsa, p),tl,tr) <0) {
				sl =p;
				ulong end = ( (sl+tpsi < r) ? sl+tpsi : r );
				getPsiBuffericsa(myicsa,B,sl,end);
				uint i=0; uint n=end -sl +1;
				for ( ; i<n;i++) if (cmptarget(B[i],tl,tr) ==0) break;
				
				if (i>=n){  // todos <tl
					*left =sl; *right=sl; *numocc =0; return 0;
				}
				else 
					*left = sl+i;   //found
			}
			else { //cmptarget(getPsiicsa(myicsa, p),tl,tr) >0)
				*left =sl; *right=sl; *numocc =0; return 0;
			}
		}
		
		//else //left was set before;
		*numocc = *right -*left +1;		
	}//end 2+samples
		
	return 0;
}























//finds the range [left-right] in a zone of increasing values of psi[left-right], such 
//that for any $i \in [left-right] $, psi(i) maps into [tl,tr] 
//optimizes the search by taking advantage of accessing sampled psi positions first 
//(at positions i = k *myicsa->T_Psi )
//it uses getFindPsiicsa to locate left/right in a zone psi[x,y]... those psi values are
//sequentially decompressed but not stored anywhere
int binSearchPsiTarget_samplesFirst(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr){	

	ticsa *myicsa = (ticsa *) index;
	register uint tpsi = myicsa->T_Psi;
	

	register unsigned long l, r, sl, sr, p;
	l=*left; r=*right;
		
	sl=l/tpsi * tpsi; 
	sr=r/tpsi * tpsi;
	
	
	//no sample in [l,r] --------------------
	if ( (sl<l) && (sr<l)) {

		
		ulong i1,i2;		
		int ipos= getFindPsiicsa(myicsa,l,r,tl,tr,&i1,&i2);
		if (ipos==0) {
			*left =l; *right=l; *numocc =0; return 0;
		}
		if (ipos>=1) {
			*left= l+i1; *right=l+i2; *numocc = (*right - *left +1); return 0;			
		}
		
	}
	
	//only 1 sample in [l,r] --------------------
	else if ( ((sl<l) && (sr == (sl+tpsi)))  || ((sl ==l) && (sr==sl)) ) {

		if (sl==sr){   //l == sl == sr.
			p= getPsiicsa(myicsa, sl);    //sampled pos.
			int cmp = cmptarget(p,tl,tr);
			if ( cmp ==0 ) {

				ulong i1,i2;		
				int ipos= getFindPsiicsa(myicsa,l,r,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= l; *right=l+i2; *numocc = (*right - *left +1); return 0;			
				}

			}
			else if ( cmp>0 ) {
				*left =l; *right=l; *numocc =0; return 0;			
			}
			else { // (cmp <0)

				ulong i1,i2;		
				int ipos= getFindPsiicsa(myicsa,l,r,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= l+i1; *right=l+i2; *numocc = (*right - *left +1); return 0;			
				}		
					
			}
		}
		
		else {  //sl <l   && sr=sl+tpsi
			p= getPsiicsa(myicsa, sr);    //sampled pos.
			if (p > tr) r=sr-1;

			ulong i1,i2;		
			int ipos= getFindPsiicsa(myicsa,l,r,tl,tr,&i1,&i2);
			if (ipos==0) {
				*left =l; *right=l; *numocc =0; return 0;
			}
			if (ipos>=1) {
				*left= l+i1; *right=l+i2; *numocc = (*right - *left +1); return 0;			
			}
					
		}
	}
		
	// 2+ samples in [l,r] --------------------
	//else 2+ samples en [l,r]
	else {
		int checkedleft=0;
		if (sl < l) {
			sl+=tpsi;
			p= getPsiicsa(myicsa, sl);
			if (p >tl) {
				checkedleft=1;
								
				ulong i1,i2;		
				int ipos= getFindPsiicsa(myicsa,l,sl,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= l+i1; 
					if (p>tr) {
						*right=l+i2; *numocc = (*right - *left +1); return 0;			
					}
					//else /* left already established and checkedleft==1 from here on.
				}
														
			}				
		}
		
		ulong ll,rr;
		l=sl;  // now check for *right (and maybe *left) from sl on...
		       // l starts in a sampled position "sl"
		
		//--- looks for right in [l=sl,r --------------------------------------
		
		//case 1.. right should be after the last sample.
		if (cmptarget(getPsiicsa(myicsa, sr),tl,tr) <=0) {
			
			ulong i1,i2;		
			int ipos= getFindPsiicsa(myicsa,sr,r,tl,tr,&i1,&i2);
			if (ipos==0) {
				*left =sr; *right=sr; *numocc =0; return 0;
			}
			if (ipos>=1) {
				*right=sr+i2;			
			}
			
	
		}		
		else {//case 2... binary search
			r=sr;  //r=sr-1 actually ... hence looking for *right in [sl..sr]
			
			//binary search on the samples.			
			ll=l;rr=sr;
			assert(ll%tpsi == 0);
			assert(rr%tpsi == 0);
					

			ulong prevp=0;		
			//printf("\n ll=%lu, p= %lu, rr=%lu, prevp=%lu: cmptarget = %d",ll,((ll+rr)/2 /tpsi * tpsi),rr,prevp,cmptarget(getPsiicsa(myicsa, ((ll+rr)/2 /tpsi * tpsi)),tl,tr));
			while (ll<rr) {  // ll and rr should be multiples of tpsi								
				p = ((ll+rr)/2 /tpsi * tpsi);
				//printf("\n ll=%lu, p= %lu, rr=%lu, prevp=%lu: cmptarget = %d",ll,p,rr,prevp,cmptarget(getPsiicsa(myicsa, p),tl,tr));
				if (p==prevp) break;
				assert(p%tpsi == 0);
				if (cmptarget(getPsiicsa(myicsa, p),tl,tr) <=0) 
					ll=p;
				else
					rr=p-tpsi;
				
				prevp=p;	

			}
			
			if ((rr > ll) && (cmptarget(p=getPsiicsa(myicsa, rr),tl,tr) <=0)) {
				// rr could be <ll if p == ll and rr=p+tpsi and cmptarget (ll) > 0 in the previous iteration
				// therefore we set rr = p-tpsi	... and the loop ends because ll>rr			
				sr=rr;
			}
			else {
				sr=ll; p=getPsiicsa(myicsa, sr);
			}
			//printf("\n final sr= %lu, and psi(sr) = %lu",sr,p);fflush(stdout);
			
			if (cmptarget(p,tl,tr) >0) {   //todos los samples >tr --> no match.
				*left =sr; *right=sr; *numocc =0; return 0;
			}
			else {   // cmptarget(p,tl,tr) <=0)... 
			         // si hay un ==0, ese es right, otherwise no existe right (ni left)
			    
			    ulong end = ( (sr+tpsi < r) ? sr+tpsi : r );
			    

				ulong i1,i2;		
				int ipos= getFindPsiicsa(myicsa,sr,end,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =sr; *right=sr; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*right=sr+i2;
					
						//if *left must BE within the uncompressed psi values... 
						//--> search sequentially in B[0..i], so we avoid the 
						//                         binary search for it below
						//note that cmptarget(B[i],tl,tr) ==0 at this point.				
						if ((checkedleft==0) && (cmptarget(p, tl, tr)<0)) {
							*left = sr+i1;
							*numocc = (*right - *left +1); return 0;			
						}
					 
				}

			}
		}	
		
		
		// IF YOU REACHED HERE, *right is already known.
		// LEFT is unknown (but within [sl,*right]) if checkleft==0.
		// LEFT is known if (checkleft==1).
		
		//-----------------
		if (checkedleft == 0) {  // buscar *left en [sl,*right];
			r= *right;
			ll= sl; rr = r/tpsi * tpsi;
			while (ll < rr) {
				p = ((ll+rr)/2 /tpsi * tpsi);
				if (cmptarget(getPsiicsa(myicsa, p),tl,tr) >=0) 
					rr=p;
				else
					ll=p+tpsi;				
			}
			
			p=rr;
			assert(p%tpsi == 0);			
			if ((p>sl) &&  (cmptarget(getPsiicsa(myicsa, p),tl,tr) >=0) ) {
				p-=tpsi;   //p--;
			}
			
			if (cmptarget(getPsiicsa(myicsa,p),tl,tr) ==0) {
				*left = p;
			}
			else if (cmptarget(getPsiicsa(myicsa, p),tl,tr) <0) {
				sl =p;
				ulong end = ( (sl+tpsi < r) ? sl+tpsi : r );
				

				ulong i1,i2;		
				int ipos= getFindPsiicsa(myicsa,sl,end,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =sl; *right=sl; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= sl+i1;			
				}
								
			}
			else { //cmptarget(getPsiicsa(myicsa, p),tl,tr) >0)
				*left =sl; *right=sl; *numocc =0; return 0;
			}
		}
		
		//else //left was set before;
		*numocc = *right -*left +1;		
	}//end 2+samples
		
	return 0;
}






//finds the range [left-right] in a zone of increasing values of psi[left-right], such 
//that for any $i \in [left-right] $, psi(i) maps into [tl,tr] 
//optimizes the search by taking advantage of accessing sampled psi positions first 
//(at positions i = k *myicsa->T_Psi )
//it uses getFindPsiBuffericsa to locate left/right in a zone psi[x,y]... those psi values are
//sequentially decompressed and stored (but never used) into a temporal buffer "B"
int binSearchPsiTarget_samplesFirst_buffer_not_used(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr){	

	ticsa *myicsa = (ticsa *) index;
	register uint tpsi = myicsa->T_Psi;
	//uint *B = myicsa->Buffer;
	static uint B[4096];
		

	register unsigned long l, r, sl, sr, p;
	l=*left; r=*right;
		
	sl=l/tpsi * tpsi; 
	sr=r/tpsi * tpsi;
	
	
	//no sample in [l,r] --------------------
	if ( (sl<l) && (sr<l)) {
		
		ulong i1,i2;		
		int ipos= getFindPsiBuffericsa(myicsa,B,l,r,tl,tr,&i1,&i2);
		if (ipos==0) {
			*left =l; *right=l; *numocc =0; return 0;
		}
		if (ipos>=1) {
			*left= l+i1; *right=l+i2; *numocc = (*right - *left +1); return 0;			
		}
		

	}
	
	//only 1 sample in [l,r] --------------------
	else if ( ((sl<l) && (sr == (sl+tpsi)))  || ((sl ==l) && (sr==sl)) ) {

		if (sl==sr){   //l == sl == sr.
			p= getPsiicsa(myicsa, sl);    //sampled pos.
			int cmp = cmptarget(p,tl,tr);
			if ( cmp ==0 ) {

				ulong i1,i2;		
				int ipos= getFindPsiBuffericsa(myicsa,B,l,r,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= l; *right=l+i2; *numocc = (*right - *left +1); return 0;			
				}


			}
			else if ( cmp>0 ) {
				*left =l; *right=l; *numocc =0; return 0;			
			}
			else { // (cmp <0)

				ulong i1,i2;		
				int ipos= getFindPsiBuffericsa(myicsa,B,l,r,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= l+i1; *right=l+i2; *numocc = (*right - *left +1); return 0;			
				}
			
			
					
			}
		}
		
		else {  //sl <l   && sr=sl+tpsi
			p= getPsiicsa(myicsa, sr);    //sampled pos.
			if (p > tr) r=sr-1;

			ulong i1,i2;		
			int ipos= getFindPsiBuffericsa(myicsa,B,l,r,tl,tr,&i1,&i2);
			if (ipos==0) {
				*left =l; *right=l; *numocc =0; return 0;
			}
			if (ipos>=1) {
				*left= l+i1; *right=l+i2; *numocc = (*right - *left +1); return 0;			
			}
					

		}
	}
		
	// 2+ samples in [l,r] --------------------
	//else 2+ samples en [l,r]
	else {
		int checkedleft=0;
		if (sl < l) {
			sl+=tpsi;
			p= getPsiicsa(myicsa, sl);
			if (p >tl) {
				checkedleft=1;
								
				ulong i1,i2;		
				int ipos= getFindPsiBuffericsa(myicsa,B,l,sl,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =l; *right=l; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= l+i1; 
					if (p>tr) {
						*right=l+i2; *numocc = (*right - *left +1); return 0;			
					}
					//else /* left already established and checkedleft==1 from here on.
				}
				
											
			}				
		}
		
		ulong ll,rr;
		l=sl;  // now check for *right (and maybe *left) from sl on...
		       // l starts in a sampled position "sl"
		
		//--- looks for right in [l=sl,r --------------------------------------
		
		//case 1.. right should be after the last sample.
		if (cmptarget(getPsiicsa(myicsa, sr),tl,tr) <=0) {
			
			ulong i1,i2;		
			int ipos= getFindPsiBuffericsa(myicsa,B,sr,r,tl,tr,&i1,&i2);
			if (ipos==0) {
				*left =sr; *right=sr; *numocc =0; return 0;
			}
			if (ipos>=1) {
				*right=sr+i2;			
			}
			

		}		
		else {//case 2... binary search
			r=sr;  //r=sr-1 actually ... hence looking for *right in [sl..sr]
			
			//binary search on the samples.			
			ll=l;rr=sr;
			assert(ll%tpsi == 0);
			assert(rr%tpsi == 0);
					

			ulong prevp=0;		
			//printf("\n ll=%lu, p= %lu, rr=%lu, prevp=%lu: cmptarget = %d",ll,((ll+rr)/2 /tpsi * tpsi),rr,prevp,cmptarget(getPsiicsa(myicsa, ((ll+rr)/2 /tpsi * tpsi)),tl,tr));
			while (ll<rr) {  // ll and rr should be multiples of tpsi								
				p = ((ll+rr)/2 /tpsi * tpsi);
				//printf("\n ll=%lu, p= %lu, rr=%lu, prevp=%lu: cmptarget = %d",ll,p,rr,prevp,cmptarget(getPsiicsa(myicsa, p),tl,tr));
				if (p==prevp) break;
				assert(p%tpsi == 0);
				if (cmptarget(getPsiicsa(myicsa, p),tl,tr) <=0) 
					ll=p;
				else
					rr=p-tpsi;
				
				prevp=p;	

			}
			
			if ((rr > ll) && (cmptarget(p=getPsiicsa(myicsa, rr),tl,tr) <=0)) {
				// rr could be <ll if p == ll and rr=p+tpsi and cmptarget (ll) > 0 in the previous iteration
				// therefore we set rr = p-tpsi	... and the loop ends because ll>rr			
				sr=rr;
			}
			else {
				sr=ll; p=getPsiicsa(myicsa, sr);
			}
			//printf("\n final sr= %lu, and psi(sr) = %lu",sr,p);fflush(stdout);
			
			if (cmptarget(p,tl,tr) >0) {   //todos los samples >tr --> no match.
				*left =sr; *right=sr; *numocc =0; return 0;
			}
			else {   // cmptarget(p,tl,tr) <=0)... 
			         // si hay un ==0, ese es right, otherwise no existe right (ni left)
			    
			    ulong end = ( (sr+tpsi < r) ? sr+tpsi : r );
			    

				ulong i1,i2;		
				int ipos= getFindPsiBuffericsa(myicsa,B,sr,end,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =sr; *right=sr; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*right=sr+i2;
					
						//if *left must BE within the uncompressed psi values... 
						//--> search sequentially in B[0..i], so we avoid the 
						//                         binary search for it below
						//note that cmptarget(B[i],tl,tr) ==0 at this point.				
						if ((checkedleft==0) && (cmptarget(p, tl, tr)<0)) {
							*left = sr+i1;
							*numocc = (*right - *left +1); return 0;			
						}
					 
				}
			    

			}
		}	
		
		
		// IF YOU REACHED HERE, *right is already known.
		// LEFT is unknown (but within [sl,*right]) if checkleft==0.
		// LEFT is known if (checkleft==1).
		
		//-----------------
		if (checkedleft == 0) {  // buscar *left en [sl,*right];
			r= *right;
			ll= sl; rr = r/tpsi * tpsi;
			while (ll < rr) {
				p = ((ll+rr)/2 /tpsi * tpsi);
				if (cmptarget(getPsiicsa(myicsa, p),tl,tr) >=0) 
					rr=p;
				else
					ll=p+tpsi;				
			}
			
			p=rr;
			assert(p%tpsi == 0);			
			if ((p>sl) &&  (cmptarget(getPsiicsa(myicsa, p),tl,tr) >=0) ) {
				p-=tpsi;   //p--;
			}
			
			if (cmptarget(getPsiicsa(myicsa,p),tl,tr) ==0) {
				*left = p;
			}
			else if (cmptarget(getPsiicsa(myicsa, p),tl,tr) <0) {
				sl =p;
				ulong end = ( (sl+tpsi < r) ? sl+tpsi : r );
				

				ulong i1,i2;		
				int ipos= getFindPsiBuffericsa(myicsa,B,sl,end,tl,tr,&i1,&i2);
				if (ipos==0) {
					*left =sl; *right=sl; *numocc =0; return 0;
				}
				if (ipos>=1) {
					*left= sl+i1;			
				}
								

			}
			else { //cmptarget(getPsiicsa(myicsa, p),tl,tr) >0)
				*left =sl; *right=sl; *numocc =0; return 0;
			}
		}
		
		//else //left was set before;
		*numocc = *right -*left +1;		
	}//end 2+samples
		
	return 0;
}
















	// returns the rank_1 (D,i)	
uint getRankicsa(void *index, uint i) {
	ticsa *myicsa = (ticsa *) index;
//@NOLONGERRGK-2023	#ifndef BITMAP_RGK	
		return rank_1(myicsa->bD, i);// - 1;		
//@NOLONGERRGK-2023	#else
//@NOLONGERRGK-2023		return myicsa->Dmap->rank1(i);
//@NOLONGERRGK-2023	#endif

}


	// returns the select_1 (D,i)	
uint getSelecticsa375(void *index, uint i) {
	ticsa *myicsa = (ticsa *) index;
	//#ifndef BITMAP_RGK	
		return bselect(myicsa->bD, i);
	//#else
	//	return myicsa->Dmap->select1(i);
	//#endif
}


	// returns the select_1 (D,i)	
uint getSelecticsa(void *index, uint i) {
	ticsa *myicsa = (ticsa *) index;
	
//@NOLONGERRGK-2023	#ifndef BITMAP_RGK	
		return bselect(myicsa->bD, i);
//@NOLONGERRGK-2023	#else
//@NOLONGERRGK-2023		return myicsa->Dmap->select1(i);
//@NOLONGERRGK-2023	#endif
}

int test_select1ymas(void *index) {
	ticsa *myicsa = (ticsa *) index;

	size_t i,l,r;
	for (i=1; i<myicsa->bD->pop-1; i++) {
		
		l= getSelecticsa(myicsa, i);
		r= getSelecticsa(myicsa, i+1)-1;
		
		size_t roo,loo;
		geticsa_select_j_y_j_mas_1 (myicsa, i, &loo, &roo);	roo--;

		if (l != loo) {
			printf("\n l =  %zu, occs=%zu ",l, r-l+1);
			printf("\n geticsa_select_j_y_j_mas_1 FALLO en loo, %zu != %zu",l, loo);
			exit(0);
		}

		if (r != roo) {
			printf("\n r =  %zu, occs=%zu ",r, r-l+1);
			printf("\n geticsa_select_j_y_j_mas_1 FALLO en roo, %zu != %zu",r, roo);
			exit(0);
		}	
	}
	return 0;
}

	// returns the select_1 (D,i)	
uint getSelecticsaNext(void *index, uint i, uint curval) {
//	test_select1ymas(index);

	ticsa *myicsa = (ticsa *) index;
	
//@NOLONGERRGK-2023	#ifndef BITMAP_RGK	
		return bselect(myicsa->bD, i+1);
//@NOLONGERRGK-2023	#else
//@NOLONGERRGK-2023		return myicsa->Dmap->selectNext1(curval+1);	
//@NOLONGERRGK-2023	#endif
}

void geticsa_select_j_y_j_mas_1 (void *index, uint i, size_t *sj, size_t *sjmas1){	
	ticsa *myicsa = (ticsa *) index;
//@NOLONGERRGK-2023	#ifndef BITMAP_RGK
			//		//v1: sin samples
			//
			//#ifdef SELECT_BASE
			//		*sj = bselect_orig(myicsa->bD, i);
			//		*sjmas1 = bselect_orig(myicsa->bD, i+1);
			//#endif
			//#ifdef SELECT_SAMPLES
			//		//v2: con samples
			//		*sj = bselect(myicsa->bD, i);
			//		*sjmas1 = bselect(myicsa->bD, i+1);
			//#endif
			//#ifdef SELECT_DUAL
			//
			//		//v3: dual_select
			//
	
		//return bselect(myicsa->bD, i+1);
		//return getNext1 (myicsa->bD->data, curval,myicsa->bD->n-1);
		bselect_j_y_j_mas_1 (myicsa->bD, i, sj, sjmas1);
//@NOLONGERRGK-2023	#else
//@NOLONGERRGK-2023		*sj=myicsa->Dmap->select1(i);
//@NOLONGERRGK-2023		*sjmas1 = myicsa->Dmap->selectNext1(*sj+1);	
//@NOLONGERRGK-2023	#endif
	//return 0;	
}


//i is the rank among the predicates [n.. 2n-1]
uint getSelectTablePredicates(void *index, uint i) {
	ticsa *myicsa = (ticsa *) index;
	uint ns=myicsa->rankfirstPredicate;
	return myicsa->Dselect1Predicates[i-ns];
}



	// returns psi(i)	
uint getPsiicsa(void *index, uint i) {
	ticsa *myicsa = (ticsa *) index;
	register uint positionAux;
		#ifdef PSI_HUFFMANRLE
			positionAux=getHuffmanPsiValue(&(myicsa->hcPsi), i );
		#endif			 
		#ifdef PSI_GONZALO
			positionAux=getGonzaloPsiValue(&(myicsa->gcPsi), i);
		#endif
		#ifdef PSI_DELTACODES
			positionAux=getDeltaPsiValue(&(myicsa->dcPsi), i);
		#endif	
		#ifdef PSI_VBYTERLE
			positionAux=getVbytePsiValue(&(myicsa->vbPsi), i );
		#endif			
		#ifdef PSI_R3H
			positionAux=getPsiR3HValue(&(myicsa->hcPsi), i );
		#endif		
		#ifdef PSI_R3HYBRID
			positionAux=getPsiR3HybridValue(&(myicsa->hcPsi), i );
		#endif		
		#ifdef PSI_PLAIN
			positionAux=getPlainPsiValue(&(myicsa->plPsi), i );
		#endif
			 

		return	positionAux;
}

	// returns buffer[] <-- psi[ini..end]	
void getPsiBuffericsa(void *index, uint *buffer, size_t ini, size_t end) {
	ticsa *myicsa = (ticsa *) index;
		#ifdef PSI_HUFFMANRLE
			getHuffmanPsiValueBuffer(&(myicsa->hcPsi), buffer,ini,end);
		#endif			 
		#ifdef PSI_GONZALO
			getGonzaloPsiValueBuffer(&(myicsa->gcPsi), buffer,ini,end);
		#endif
		#ifdef PSI_DELTACODES
			getDeltaPsiValueBuffer(&(myicsa->dcPsi), buffer,ini,end);
		#endif	
		#ifdef PSI_VBYTERLE
			getVbytePsiValueBuffer(&(myicsa->vbPsi), buffer,ini,end);
		#endif			
		#ifdef PSI_R3H
			getPsiR3HValueBuffer(&(myicsa->hcPsi), buffer,ini,end);
		#endif		
		#ifdef PSI_R3HYBRID
			getPsiR3HybridValueBuffer(&(myicsa->hcPsi), buffer,ini,end);
		#endif		
		#ifdef PSI_PLAIN
			getPlainPsiValueBuffer(&(myicsa->plPsi), buffer,ini,end);
		#endif
			 
	return;
}

//**//
int  getFindPsiBuffericsa(void *index, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
	ticsa *myicsa = (ticsa *) index;
		#ifdef PSI_HUFFMANRLE
			return	getfindHuffmanPsiValueBuffer(&(myicsa->hcPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif			 
		#ifdef PSI_GONZALO
			return	getfindGonzaloPsiValueBuffer(&(myicsa->gcPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif
		#ifdef PSI_DELTACODES
			return	getfindDeltaPsiValueBuffer(&(myicsa->dcPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif	
		#ifdef PSI_VBYTERLE
			return getfindVbytePsiValueBuffer(&(myicsa->vbPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif			
		#ifdef PSI_R3H
			return	getfindPsiR3HValueBuffer(&(myicsa->hcPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif		
		#ifdef PSI_R3HYBRID
			return getfindPsiR3HybridValueBuffer(&(myicsa->hcPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif		
		#ifdef PSI_PLAIN
			return	getfindPlainPsiValueBuffer(&(myicsa->plPsi), buffer,ini,end,  fst,sec, i1,i2);
		#endif
	
}



int getFindPsiicsa(void *index, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
	ticsa *myicsa = (ticsa *) index;
		#ifdef PSI_HUFFMANRLE
			return	getfindHuffmanPsiValue(&(myicsa->hcPsi),ini,end,  fst,sec, i1,i2);
		#endif			 
		#ifdef PSI_GONZALO
			return	getfindGonzaloPsiValue(&(myicsa->gcPsi),ini,end,  fst,sec, i1,i2);
		#endif
		#ifdef PSI_DELTACODES
			return	getfindDeltaPsiValue(&(myicsa->dcPsi),ini,end,  fst,sec, i1,i2);
		#endif	
		#ifdef PSI_VBYTERLE
			return getfindVbytePsiValue(&(myicsa->vbPsi),ini,end,  fst,sec, i1,i2);
		#endif			
		#ifdef PSI_R3H
			return	getfindPsiR3HValue(&(myicsa->hcPsi),ini,end,  fst,sec, i1,i2);
		#endif
		#ifdef PSI_R3HYBRID
			return	getfindPsiR3HybridValue(&(myicsa->hcPsi),ini,end,  fst,sec, i1,i2);
		#endif
		#ifdef PSI_PLAIN
			return	getfindPlainPsiValue(&(myicsa->plPsi),ini,end,  fst,sec, i1,i2);
		#endif
}



	//recovers the sequence of integers kept by the ICSA. (including the terminator)
void dumpICSASequence(void *index, uint **data, size_t *len) {
	ticsa *icsa = (ticsa *) index;
	uint nEntries = icsa->nEntries;
	size_t n = (icsa->suffixArraySize-1)/icsa->nEntries;
	
	uint * buffer = (uint *) malloc (sizeof(uint) * (icsa->suffixArraySize ) );
	uint * psibuffer = (uint *) malloc (sizeof(uint) * (icsa->suffixArraySize ) );

	//psibuffer[0]=getPsiicsa(icsa,0);		
	//getHuffmanPsiValueBuffer(&(icsa->hcPsi),&psibuffer[1],1,icsa->suffixArraySize-1);
	//getHuffmanPsiValueBuffer(&(icsa->hcPsi),psibuffer,0,icsa->suffixArraySize-1);
	 getPsiBuffericsa(index, psibuffer,0,icsa->suffixArraySize-1);
	
	size_t i,j;
	size_t z=0;
	uint p;
	for (i=0;i<n;i++) {
		p=i;
		for (j=0;j<nEntries;j++) {
			uint value = getRankicsa(icsa,p);
			buffer[z++] = value -1;  // !! entrada en el vocab 
			//p=getPsiicsa(icsa,p);
			p=psibuffer[p];
		}
	}
	
	uint value = getRankicsa(icsa,icsa->suffixArraySize-1);
	buffer[z++] = value-1;   // !! entrada en el vocab 

	free(psibuffer);
	*data = buffer;
	*len = z;	
}

	//recovers the sequence of integers kept by the ICSA. (including the terminator)
void dumpICSASequence_slow_lessmem(void *index, uint **data, size_t *len) {
	ticsa *icsa = (ticsa *) index;
	uint nEntries = icsa->nEntries;
	size_t n = (icsa->suffixArraySize-1)/icsa->nEntries;
	
	uint * buffer = (uint *) malloc (sizeof(uint) * (icsa->suffixArraySize ) );
		
	size_t i,j;
	size_t z=0;
	uint p;
	for (i=0;i<n;i++) {
		p=i;
		for (j=0;j<nEntries;j++) {
			uint value = getRankicsa(icsa,p);
			buffer[z++] = value -1;  // !! entrada en el vocab 
			p=getPsiicsa(icsa,p);
		}
	}
	
	uint value = getRankicsa(icsa,icsa->suffixArraySize-1);
	buffer[z++] = value-1;   // !! entrada en el vocab 
	
	*data = buffer;
	*len = z;	
}











// Mostra as estructuras creadas
void showStructsCSA(ticsa *myicsa) {

	unsigned int index;

	// ESTRUCTURAS PARA CSA
	printf("Basic CSA structures:\n\n");
	
	// VALORES DA FUNCI�N PSI (decodificando)
	printf("\tPSI: (Sampling period = %d)\n", myicsa->T_Psi);
	for(index=0; index < myicsa->suffixArraySize; index++)	 
		#ifdef PSI_HUFFMANRLE
			printf("\tPsi[%d] = %d\n", index, getHuffmanPsiValue(&(myicsa->hcPsi),index));
		#endif
		#ifdef PSI_GONZALO
			printf("\tPsi[%d] = %d\n", index, getGonzaloPsiValue(&(myicsa->gcPsi),index));
		#endif
		#ifdef PSI_DELTACODES
			printf("\tPsi[%d] = %d\n", index, getDeltaPsiValue(&(myicsa->dcPsi),index));		
		#endif			
		#ifdef PSI_VBYTERLE
			printf("\tPsi[%d] = %d\n", index, getVbytePsiValue(&(myicsa->vbPsi),index));
		#endif
		#ifdef PSI_R3H
			printf("\tPsi[%d] = %d\n", index, getPsiR3HValue(&(myicsa->hcPsi),index));
		#endif
		#ifdef PSI_R3HYBRID
			printf("\tPsi[%d] = %d\n", index, getPsiR3HybridValue(&(myicsa->hcPsi),index));
		#endif
		#ifdef PSI_PLAIN
			printf("\tPsi[%d] = %d\n", index, getPlainPsiValue(&(myicsa->plPsi),index));
		#endif		
			
	printf("\n");
	
	// VECTOR D DE SADAKANE CO DIRECTORIO DE RANK ASOCIADO
	printf("\tD = ");
	showBitVector(myicsa->D, myicsa->suffixArraySize);
	printf("\n\nSuperbloques de D:\n");
	{	uint ns;
		uint nb;
		ns = myicsa->bD->sSize;
		nb= myicsa->bD->bSize;
		for(index=0; index<ns; index++) {
			//printf("\tDs[%d] = %d\n", index, Dir.Ds[index]);
			printf("\tDs[%d] = %d\n", index, myicsa->bD->sdata[index]);
		}
		printf("\nBloques de D:\n");
		
		for(index=0; index<nb; index++) {
			//printf("\tDb[%d] = %d\n", index, Dir.Db[index]);
			printf("\tDb[%d] = %d\n", index, myicsa->bD->bdata[index]);		
		}	
		printf("\n\n");
	}
	
//@NOLONGERRGK-2023	printf("\n Dmap-bitmapRGK not shown!!");
	
	// ESTRUCTURAS PARA ACCEDER O ARRAY DE SUFIXOS E A SUA INVERSA
//@@	printf("Suffix Array Sampling Structures: (Sampling period = %d)\n", myicsa->T_A);
//@@	printf("\tSuffix Array Samples:\n");
//@@	for(index=0; index < myicsa->samplesASize; index++) 
//@@		printf("\tSamplesA[%d] = %d\n", index, myicsa->samplesA[index]);
//@@	printf("\n");	
//@@	printf("\tInverse Suffix Array Samples:\n");
//@@	for(index=0; index < myicsa->samplesASize; index++) 
//@@		printf("\tSamplesAInv[%d] = %d\n", index, myicsa->samplesAInv[index]);
//@@	printf("\n");
			
}


// Comparacion de Sadakane entre un patron (pattern) y el sufijo en la posicion p del array de sufijos
// IMPORTANTE EVITAR ULTIMA CHAMADA A PSI
int SadCSACompare(ticsa *myicsa, uint *pattern, uint patternSize, uint p) {

	register unsigned int j, i, currentInteger, diff;
	
	i = p;
	j = 0;
	
	while(1) {
//@NOLONGERRGK-2023		#ifndef BITMAP_RGK
		currentInteger = rank_1(myicsa->bD, i) - 1;
//@NOLONGERRGK-2023		#else
//@NOLONGERRGK-2023		currentInteger = myicsa->Dmap->rank1(i) -1;
//@NOLONGERRGK-2023		#endif
		
		diff = pattern[j++] - currentInteger;
		if(diff) return diff;
		if(j == patternSize) return 0;
		else 
			#ifdef PSI_HUFFMANRLE
				i=getHuffmanPsiValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_GONZALO
				i=getGonzaloPsiValue(&(myicsa->gcPsi),i);
			#endif		
			#ifdef PSI_DELTACODES
				i=getDeltaPsiValue(&(myicsa->dcPsi),i);
			#endif
			#ifdef PSI_VBYTERLE
				i=getVbytePsiValue(&(myicsa->vbPsi),i);
			#endif			
			#ifdef PSI_R3H
				i=getPsiR3HValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_R3HYBRID
				i=getPsiR3HybridValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_PLAIN
				i=getPlainPsiValue(&(myicsa->plPsi),i);
			#endif					
	}
}

// as the previous one, but the first call to getPsi is the value in parameter "psiini"
int SadCSACompareSecondOn(ticsa *myicsa, uint *pattern, uint patternSize, uint p, uint psiini) {

	register unsigned int j, i, currentInteger, diff;
	
	i = p;
	j = 0;
	
	while(1) {
//@NOLONGERRGK-2023		#ifndef BITMAP_RGK
		currentInteger = rank_1(myicsa->bD, i) - 1;
//@NOLONGERRGK-2023		#else
//@NOLONGERRGK-2023		currentInteger = myicsa->Dmap->rank1(i) -1;
//@NOLONGERRGK-2023		#endif
				
		diff = pattern[j++] - currentInteger;
		if(diff) return diff;
		if(j == patternSize) return 0;
		else  {
			if (j==1) i=psiini;
			else
			#ifdef PSI_HUFFMANRLE
				i=getHuffmanPsiValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_GONZALO
				i=getGonzaloPsiValue(&(myicsa->gcPsi),i);
			#endif		
			#ifdef PSI_DELTACODES
				i=getDeltaPsiValue(&(myicsa->dcPsi),i);
			#endif
			#ifdef PSI_VBYTERLE
				i=getVbytePsiValue(&(myicsa->vbPsi),i);
			#endif			
			#ifdef PSI_R3H
				i=getPsiR3HValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_R3HYBRID
				i=getPsiR3HybridValue(&(myicsa->hcPsi),i);
			#endif
			#ifdef PSI_PLAIN
				i=getPlainPsiValue(&(myicsa->plPsi),i);
			#endif					
		}
	}
	
}



// Initializes the parameters of the index.
uint parametersCSA(ticsa *myicsa, char *build_options){ 
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;
	int ssA,ssAinv,ssPsi,nsHuff,psiSearchFactor;
	
	ssPsi  = DEFAULT_PSI_SAMPLING_PERIOD;
	nsHuff = DEFAULT_nsHUFF;
	psiSearchFactor = DEFAULT_PSI_BINARY_SEARCH_FACTOR;
	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  
		if ((strcmp(parameters[j], "sPsi") == 0 ) && (j < num_parameters-1) ) {
			ssPsi=atoi(parameters[j+1]);			
		} 	
		if ((strcmp(parameters[j], "nsHuff") == 0 ) && (j < num_parameters-1) ) {
			nsHuff=atoi(parameters[j+1]);
			nsHuff *=1024;			
		} 
		if ((strcmp(parameters[j], "psiSF") == 0 ) && (j < num_parameters-1) ) {
			psiSearchFactor=atoi(parameters[j+1]);
		} 			
		j++;
	}
	free_parameters(num_parameters, &parameters);
	}		

	myicsa->T_Psi = ssPsi;
	myicsa->tempNSHUFF = nsHuff;
	myicsa->psiSearchFactorJump = psiSearchFactor * ssPsi;	

	printf("\n\t parameters for iCSA: samplePsi=%d", ssPsi);
	printf("\n\t              : nsHuff=%d, psiSearchFactor = %d --> jump = %d", nsHuff,psiSearchFactor, myicsa->psiSearchFactorJump);
	return 0;
}
