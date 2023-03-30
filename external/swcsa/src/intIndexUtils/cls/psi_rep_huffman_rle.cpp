#include "psi_rep_huffman_rle.h"

// IMPLEMENTACION DAS FUNCIONS

psi_rep_huffman_rle::~psi_rep_huffman_rle() {

	//if (diffsHT.total)	freeHuff(diffsHT);
		
	if (diffsHT.s.symb) free(diffsHT.s.symb);
	if (diffsHT.num) free (diffsHT.num);
	if (diffsHT.fst) free (diffsHT.fst);
	
		
		
	if (samples)
		free(samples);

	if (samplePointers)
		free(samplePointers);

	if (stream)
		free(stream);
}


psi_rep_huffman_rle::psi_rep_huffman_rle() {
	totalMem=0;

	T=0;
	nS=0;
	
	#ifdef 	R3H_WITHGAPS
		OFFSET=0;               
	#endif

	//diffsHT=NULL;			
	//diffsHT.total = 0;
	diffsHT.s.symb = NULL;
	diffsHT.num = NULL;
	diffsHT.fst = NULL;

	numberOfSamples=0;

	sampleSize=0;			
	samples=NULL;			
	
	pointerSize=0;			
	samplePointers=NULL;	

	streamSize=0;			
	stream=NULL;				
}


psi_rep_huffman_rle::psi_rep_huffman_rle(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE,  unsigned int nS=16384) { //, uint *psiTypes=NULL){
	build(Psi,psiSize,inioffset,T,nS);
}



//void psi_rep_huffman_rle::build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS) {
void psi_rep_huffman_rle::build(unsigned int *Psi, size_t psiSize,  size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE,  unsigned int nS=16384) {


	printf("\n call to psi_rep_huffman_rle - compress Psi\n");fflush(stdout);fflush(stderr);

#ifdef 	R3H_WITHGAPS	
	this->OFFSET = inioffset;
	//printf("\n **************** offset = %zu ***************************************",inioffset);
#endif	
	size_t maxval3R = 0;               //required to know the max-value that must be used in the samples.
	
	uint absolute_value;
	register size_t index;
	register size_t ptr, samplesPtr, samplePointersPtr;
	unsigned int runLenght, binaryLenght;
	
	ssize_t *diffs;	
	unsigned int *huffmanDst;
	
	// Estructuras da funcion comprimida (para logo asignar)
	// Tam�n se podian almacenar directamente
	THuff diffsHT;
	size_t numberOfSamples;
	unsigned int *samples;

	unsigned int sampleSize;
	size_t *samplePointers;
	
	unsigned int pointerSize;
	unsigned int *stream;
	size_t streamSize;
	
	// Variables que marcan os intervalos dentro do vector de frecuencias
	unsigned int runLenghtStart = nS - 64 - T; 	// Inicio das Runs
	unsigned int negStart = nS - 64;			// Inicio dos Negativos
	unsigned int bigStart = nS - 32;			// Inicio dos Grandes (>runLenghtStart)
	
	// Para estadistica
	size_t totalSize;
	
	// Reservamos espacio para a distribuci�n de valores de Psi
	huffmanDst = (unsigned int *)malloc(sizeof(int)*nS);
	for(index=0;index<nS;index++) huffmanDst[index]=0;

	
	// Inicializamos diferencias	
	diffs = (ssize_t *)malloc(sizeof(ssize_t)*psiSize);	

	
	diffs[0] = 0;
	for(index=1; index<psiSize; index++) {
		diffs[index] = ((ssize_t)Psi[index]) - ((ssize_t)Psi[index-1]);
	}
		
	// Calculamos a distribucion de frecuencias
	runLenght = 0;
	for(index=0; index<psiSize; index++) {

		if(index%T) {
			
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght++;
			} else {	// Non estamos nun run
				if(runLenght) {
					huffmanDst[runLenght+runLenghtStart]++;
					runLenght = 0;
				}
				if(diffs[index]>((ssize_t)1) && diffs[index]<runLenghtStart) 
					huffmanDst[diffs[index]]++;
				else
					if(diffs[index]< ((ssize_t) 0) ) {	// Valor negativo
						absolute_value = (uint) (-diffs[index]);
						binaryLenght = bits(absolute_value);
						
						if (binaryLenght > 32)
							printf("\n OJO, que absolute_value vale %u",absolute_value);
							
						huffmanDst[binaryLenght+negStart-1]++;
					} else {				// Valor grande >= 128
						absolute_value = (uint)(diffs[index]);
						binaryLenght = bits(absolute_value);

						if (binaryLenght > 32)
							printf("\n OJO, que absolute_value vale %u",absolute_value);

						huffmanDst[binaryLenght+bigStart-1]++;
					}
			}
			
		} else { // Rompemos o run porque atopamos unha mostra
			if(runLenght) {
				huffmanDst[runLenght+runLenghtStart]++;
				runLenght = 0;
			}
		}
		
	}
		
	if(runLenght) huffmanDst[runLenght+runLenghtStart]++;

//  -SHOW THE FREQUENCY DISTRIBUTION OF THE SYMBOLS
//	{int x;
//		fflush(stderr);fflush(stdout);
//		printf ("\n");
//		for (x=0;x<nS;x++) {
//				printf("\n Huff-freq [%3d] = %u",x,huffmanDst[x]);
//		}
//		printf ("\n");
//	}
	
	// Creamos o arbol de Huffman
	diffsHT = createHuff(huffmanDst,nS-1,UNSORTED);
	
	// Calculamos o espacio total ocupado pola secuencia Huffman + RLE
	streamSize = (size_t) diffsHT.total;
	fprintf(stderr,"\n\t\t [3] diffsHT.total = %zu bits ", streamSize);

/******2014.03.12... bug del optimizador de gcc ?? ****/
// ok con gcc -O0
// falla con gcc -O3

/*
//Si quito if-printf, el optimizador (-O3) hace que este código falle xD!

size_t prev_streamsize = streamSize;
	for(index=negStart;index<bigStart;index++) {
		if (!(index %19)) 
			printf("\n \t\t  %ld , %ld,  %ld , resta= %ld", negStart, index, huffmanDst[index], (index-negStart+1));
		streamSize += ((size_t)huffmanDst[index])*(index-negStart+1);	// Negativos
	}
	printf("\n\t\t [2]streamSize = %ld , index = %ld, incremento = %ld",streamSize, index, streamSize - prev_streamsize);

	for(index=bigStart;index<nS;index++) {
		if (!(index %21)) 
			printf("\n \t\t  %ld , %ld,  %ld , resta= %ld", negStart, index, huffmanDst[index], (index-bigStart+1));
		streamSize += ((size_t)huffmanDst[index])*(index-bigStart+1);		// Grandes	
	}
	printf("\n\t\t [3]streamSize = %ld , index = %ld",streamSize, index);
	fflush(stdout);
	
*/

//versión que funciona OK		
	for(index=negStart;index<nS;index++)  {
		if (index < bigStart)
			streamSize += ((size_t)huffmanDst[index])*(index-negStart+1);	// Negativos
		else 
			streamSize += ((size_t)huffmanDst[index])*(index-bigStart+1);   // Grandes	
	}	
	fprintf(stderr,"\n\t\t [3]streamSize = %ld , index = %ld",streamSize, index);

/**********/
	
	// Calculamos o numero de mostras e o espacio ocupado por cada mostra e por cada punteiro
	numberOfSamples = (psiSize+T-1)/T;	
	
	//change due to implementation of psi with 3 parts --> storing values greater than psiSize
	{
		for(index=0; index<psiSize; index++) {
			if (maxval3R< Psi[index]) maxval3R = Psi[index];
		}
	}
	

#ifdef 	R3H_WITHGAPS	
	maxval3R -= this->OFFSET;
#endif	
	sampleSize = bits(maxval3R);
//	sampleSize = bits(psiSize);

fflush(stdout);fflush(stderr);
    printf("\n **************** maxVal3R = %zu , bits = %u *******",maxval3R,sampleSize);

// ********************************************************************************************
//@@fari!!!!xD!!!  Necesario sólo porque comprimimos el vocabulario del graph-index con esto: ver UNMAP[]
	uint sampleSize2= bits(Psi[psiSize-1]);	
#ifdef 	R3H_WITHGAPS	
	    sampleSize2= bits(Psi[psiSize-1]-this->OFFSET);	 
#endif	
	
	if (sampleSize2>sampleSize) sampleSize=sampleSize2;
// ********************************************************************************************

    printf("\n ====================== maxVal3R = %zu , bits = %u *******",maxval3R,sampleSize);
fflush(stdout);fflush(stderr);
	
	pointerSize = bits(streamSize);	

fprintf(stderr,"\n espacio para Sample-values-psi = %lu",   sizeof(uint)*(((size_t)numberOfSamples*sampleSize+W-1)/W));
fprintf(stderr,"\n espacio para Sample-values-psi** = %lu", sizeof(uint)*(((size_t)numberOfSamples*sampleSize+W-1)/W));
	// Reservamos espacio para a secuencia e para as mostras e punteiros
	samples = (unsigned int *)malloc(sizeof(uint)*(((size_t)numberOfSamples*sampleSize+W-1)/W));	
		samples[(((size_t)numberOfSamples*sampleSize+W-1)/W)-1] =0000; //initialized only to avoid valgrind warnings
		

fprintf(stderr,"\n espacio para Sample-pointers-psi = %lu", sizeof(size_t)* (ulong_len(pointerSize,numberOfSamples)) );
		
	samplePointers = (size_t *)malloc(sizeof(size_t)* (ulong_len(pointerSize,numberOfSamples)) );
		samplePointers[ (ulong_len(pointerSize,numberOfSamples)) -1] = 00000000;  //initialized only to avoid valgrind warnings

//printf("\n\n**** TEST_ULONLEN = %lu, normal = %lu", sizeof(size_t)* (ulong_len(pointerSize,numberOfSamples)) ,
//											(((size_t)numberOfSamples*pointerSize+WW-1)/WW)*sizeof(size_t));
		
	stream = (unsigned int *)malloc(sizeof(int)*((streamSize+W-1)/W));
		stream[((streamSize+W-1)/W)-1]=0000;//initialized only to avoid valgrind warnings
	
fprintf(stderr,"\n espacio para stream-psi = %lu", sizeof(int)*((streamSize+W-1)/W) );
	
	// Comprimimos secuencialmente (haber� que levar un punteiro desde o inicio)
	ptr = 0;
	samplesPtr = 0;
	samplePointersPtr = 0;
	runLenght = 0;
	for(index=0; index<psiSize; index++) {
		
		if(index%T) {
			
			if(diffs[index]==((ssize_t)1)) {
				runLenght++;
			} else {	// Non estamos nun run
				if(runLenght) {
					ptr = encodeHuff(diffsHT,runLenght+runLenghtStart,stream,ptr);
					runLenght = 0;
				}
				if(diffs[index]>((ssize_t)1) && diffs[index]<runLenghtStart) {				
					ptr = encodeHuff(diffsHT,(uint)diffs[index],stream,ptr);	
				}	
				else
					if(diffs[index]< ((ssize_t)0) ) {	// Valor negativo
						absolute_value = (uint) (-diffs[index]);
						binaryLenght = bits(absolute_value);
						ptr = encodeHuff(diffsHT,binaryLenght+negStart-1,stream,ptr);
						bitwrite(stream,ptr,binaryLenght,absolute_value);
						
					/**/	uint tmp = bitread(stream,ptr,binaryLenght);			
					/**/	assert (tmp == absolute_value);	
						
						ptr += binaryLenght;						
					} else {				// Valor grande >= 128
						absolute_value = (uint) diffs[index];
						binaryLenght = bits(absolute_value);					
						ptr = encodeHuff(diffsHT,binaryLenght+bigStart-1,stream,ptr);
						bitwrite(stream,ptr,binaryLenght,absolute_value);
					/**/	uint tmp = bitread(stream,ptr,binaryLenght);			
					/**/	assert (tmp == absolute_value);							
						ptr += binaryLenght;
					}
			}
			
		} else { // Rompemos o run porque atopamos unha mostra
			if(runLenght) {				
				ptr = encodeHuff(diffsHT,runLenght+runLenghtStart,stream,ptr);
				runLenght = 0;
			}

#ifdef 	R3H_WITHGAPS	
			bitwrite(samples,samplesPtr,sampleSize, Psi[index]-this->OFFSET);
			/**/uint tmp = bitread(samples,samplesPtr,sampleSize)+this->OFFSET;			
			/**/assert (tmp == Psi[index]);		
#else
			bitwrite(samples,samplesPtr,sampleSize, Psi[index]);
			/**/uint tmp = bitread(samples,samplesPtr,sampleSize);			
			/**/assert (tmp == Psi[index]);		
#endif				
			samplesPtr += sampleSize;
			
			
			bitwrite64(samplePointers,samplePointersPtr,pointerSize,ptr);
			/**/size_t tmp2 = bitread64(samplePointers,samplePointersPtr,pointerSize);
			/**/assert (tmp2 == ptr);					
			samplePointersPtr += pointerSize;

			


					/*  //Chequear que bitwrite/bitread funcionan OK
						{{
									
									uint tmp = bitread(samples,samplesPtr,sampleSize);
									
									if (tmp != Psi[index]) {
										printf("\n bitwrite32/bitread32 failed: value src= %u value recovered = %u, k-bits=%u, offset = %u/%u",				          
																							   Psi[index],tmp, sampleSize,samplesPtr,(numberOfSamples*sampleSize+W-1));
										//checkBitwriteBitRead(samples, samplesPtr, 70000000);
										
										checkBitwriteBitReadValue(samples, samplesPtr,67120000);
										checkBitwriteBitReadValue(samples, 0,67120000);
																							   
										bitwrite(samples,samplesPtr,sampleSize, 67120000);
										uint tmp = bitread(samples,samplesPtr,sampleSize);
										printf("\n bitwrite32/bitread32 failed: value src= %u value recovered = %u, k-bits=%u, offset = %u/%u",				          
																							   67120000,tmp, sampleSize,samplesPtr,(numberOfSamples*sampleSize+W-1));

										exit(0);
										counterrors ++;                                                       
										if (counterrors > 20) exit(0);
									}
									//assert(tmp == Psi[index]);
						}}
						*/
			
		}
		
	}
	
	if(runLenght) {	
		ptr = encodeHuff(diffsHT,runLenght+runLenghtStart,stream,ptr);
	}
	
	// Amosamos o espacio ocupado
	totalSize = ((size_t) sizeof(psi_rep_huffman_rle)) + 
		sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W) + 
		sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW) +
		sizeof(int)*((size_t)(streamSize+W-1)/W) + 
		sizeHuff(diffsHT);
		
		fprintf(stderr,"\n@@@@@@@@@ psi samaplePeriod= %u, ns=%u ", T,nS);
		fprintf(stderr,"\n@@@@@@@@@ psi size= [samples = %lu] bytes ", sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [pointers = %lu] bytes ",sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [totalsize diffsHt.total  = %lu] bits \n",diffsHT.total);		
		fprintf(stderr,"\n@@@@@@@@@ psi size= [streamsize+largevalues =%lu] bytes ", sizeof(int)*((size_t)(streamSize+W-1)/W));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [sizeHuff tree = %lu] bytes ", (ulong)sizeHuff(diffsHT));		

	//@@printf("\n\t Compressed Psi size = %zu bytes, with %d different symbols.", totalSize, nS);
	
	// Necesario antes de decodificar
	prepareToDecode(&diffsHT);
	
	// Asignamos os valores a cPsi e devolvemolo
	this->T = T;
	this->diffsHT = diffsHT;
	this->nS = nS;
	this->numberOfSamples = numberOfSamples;
	this->samples = samples;
	this->sampleSize = sampleSize;
	this->samplePointers = samplePointers;
	this->pointerSize = pointerSize;
	this->stream = stream;
	this->streamSize = streamSize;
	this->totalMem = totalSize;
	
/*
{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
	fprintf(stderr,"\n Test compress/uncompress PSI - psi_rep_huffman_rle is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
	size_t i;
	uint val1,val2;	
	fflush(stdout);
	
	uint count=0;
	for (i=0; i<psiSize; i++) {
			val1= this->getPsiValue(i);
			val2=Psi[i];
			if (val1 != val2) { count++;
					fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
					fprintf(stderr,"\n i=%zu,diffs[i] = %ld ",i, (long)diffs[i]); fflush(stdout);fflush(stderr);
					//if (count > 100) 
						{ exit(0);;}
			}
	}
		fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
} /////////////////////////////////////////////////////////////////////////////////////
*/

	
	//frees resources not needed in advance
	free(diffs);
	free(huffmanDst);
	
	//returns the data structure that holds the compressed psi.
	//return cPsi;	
}





//for Nieves' statistics
ulong psi_rep_huffman_rle::getPsiSizeBitsUptoPosition(size_t position) { 

	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = this->nS - 64 - this->T;
	unsigned int negStart = this->nS - 64;
	unsigned int bigStart = this->nS - 32;	
	
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
		psiValue +=this->OFFSET;
#endif				
	
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
	
	for(index=0;index<positionsSinceSample;index++) {
		
		ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
		
		if(huffmanCode < runLenghtStart) { 	// Incremento directo
			psiValue += huffmanCode;
		}	
		else 
			if(huffmanCode < negStart) {	// Estamos nun run
				runLenght = huffmanCode - runLenghtStart;
				if(index+runLenght>=positionsSinceSample){
					//return psiValue+positionsSinceSample-index;
					break;
				}
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
			}
			else
				if(huffmanCode < bigStart) {	// Negativo
					binaryLenght = huffmanCode-negStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue -= absolute_value;	
				}
				else {	// Grande
					binaryLenght = huffmanCode-bigStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue += absolute_value;				 
				}				
	}
	
	 
	
	return (unsigned long) (ptr + sampleIndex * (this->sampleSize + this->pointerSize) );

}



unsigned int psi_rep_huffman_rle::getPsiValue( size_t position) {
//unsigned int getHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t position) {	
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = this->nS - 64 - this->T;
	unsigned int negStart = this->nS - 64;
	unsigned int bigStart = this->nS - 32;	
	
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
	
	for(index=0;index<positionsSinceSample;index++) {
		
		ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
		
		if(huffmanCode < runLenghtStart) { 	// Incremento directo
			psiValue += huffmanCode;
		}	
		else 
			if(huffmanCode < negStart) {	// Estamos nun run
				runLenght = huffmanCode - runLenghtStart;
				if(index+runLenght>=positionsSinceSample)
					return psiValue+positionsSinceSample-index;
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
			}
			else
				if(huffmanCode < bigStart) {	// Negativo
					binaryLenght = huffmanCode-negStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue -= absolute_value;	
				}
				else {	// Grande
					binaryLenght = huffmanCode-bigStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue += absolute_value;				 
				}				
	}
	
	return psiValue;

}


void psi_rep_huffman_rle::getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end) {
//void getHuffmanPsiR3ValueBuffer_1(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end)
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getPsiValue(i);
	return;
}


void psi_rep_huffman_rle::getPsiValueBuffer(uint *buffer, size_t ini, size_t end) {	
//void getHuffmanPsiR3ValueBuffer(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end) {

	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = this->nS - 64 - this->T;
	unsigned int negStart = this->nS - 64;
	unsigned int bigStart = this->nS - 32;	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {
		
		ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
		
		if(huffmanCode < runLenghtStart) { 	// Incremento directo
			psiValue += huffmanCode;
		}	
		else 
			if(huffmanCode < negStart) {	// Estamos nun run
				runLenght = huffmanCode - runLenghtStart;
				if(index+runLenght>=positionsSinceSample){
					//return psiValue+positionsSinceSample-index;
					psiValue = psiValue+ positionsSinceSample-index;
					inRun=1;//@@
					inRunRemain = runLenght + index-positionsSinceSample;//@@
					break;
				}					
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
			}
			else
				if(huffmanCode < bigStart) {	// Negativo
					binaryLenght = huffmanCode-negStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue -= absolute_value;	
				}
				else {	// Grande
					binaryLenght = huffmanCode-bigStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue += absolute_value;				 
				}				
	}	
	*buffer++= psiValue;  //value psi(ini)
	
	//@@ //--- AVOIDS PROBLES IN RUN... BUG FIXED on 2014.11.20
	if (inRun) {
		int iters = ((end-ini) >inRunRemain)? inRunRemain: (end-ini);
		ini+=iters;
		
		while (iters>0) {		
			psiValue++;	
			*buffer++= psiValue;
			iters --;
		}
		
	}
	//@@
	
	size_t cpsiT = this->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
			
			//
			//size_t ptrnew = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
			//assert (ptr == ptrnew); //ptr is the same position as in the first loop
			//if (ptr != ptrnew) {
			//	printf("\n ptr = %lu and ptrnew = %lu do not match!\n", (ulong) ptr, (ulong) ptrnew);
			//}
		}
		else {
		
			ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
			
			if(huffmanCode < runLenghtStart) { 	// Incremento directo
				psiValue += huffmanCode;
			}	
			else 
				if(huffmanCode < negStart) {	// Estamos nun run
					runLenght = huffmanCode - runLenghtStart;
	/*				{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position;
						for (l=0;l<runLenght;l++) {
							psiValue++;
							*buffer++=psiValue;							
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}					 						
					*/
					
					{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position+1;   //@corregido 2014.05.01 instead of "end-position" xD!
						for (l=0;l<runLenght;l++) {
							psiValue++;
							*buffer++=psiValue;							
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}						 
				}
				else
					if(huffmanCode < bigStart) {	// Negativo
						binaryLenght = huffmanCode-negStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue -= absolute_value;	
					}
					else {	// Grande
						binaryLenght = huffmanCode-bigStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue += absolute_value;				 
					}				
			
		}
		*buffer++ = psiValue;
		position ++;
	} 
			
	return;
}





//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst   value >= fst  and <=sec
// sets in i2 de position in buffer of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found

int psi_rep_huffman_rle::getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindHuffmanPsiR3ValueBuffer(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
		int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = this->nS - 64 - this->T;
	unsigned int negStart = this->nS - 64;
	unsigned int bigStart = this->nS - 32;	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {
		
		ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
		
		if(huffmanCode < runLenghtStart) { 	// Incremento directo
			psiValue += huffmanCode;
		}	
		else 
			if(huffmanCode < negStart) {	// Estamos nun run
				runLenght = huffmanCode - runLenghtStart;
				if(index+runLenght>=positionsSinceSample){
					//return psiValue+positionsSinceSample-index;
					psiValue = psiValue+ positionsSinceSample-index;
					inRun=1;//@@
					inRunRemain = runLenght + index-positionsSinceSample;//@@
					break;
				}					
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
			}
			else
				if(huffmanCode < bigStart) {	// Negativo
					binaryLenght = huffmanCode-negStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue -= absolute_value;	
				}
				else {	// Grande
					binaryLenght = huffmanCode-bigStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue += absolute_value;				 
				}				
	}	
	*buffer++= psiValue;  //value psi(ini)
	
		{
			if ((psiValue >=fstt)&& (psiValue <=sec)) {
					*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
					toreturn=1;
			}
			if ((psiValue >=fst)&& (psiValue <=sec)) {
				*i2= count; 
				toreturn=2;
			}
			if (psiValue >=sec) {
					return toreturn;
			}
			count ++;		
		}
	
	//@@ //--- AVOIDS PROBLES IN RUN... BUG FIXED on 2014.11.20
	if (inRun) {
		int iters = ((end-ini) >inRunRemain)? inRunRemain: (end-ini);
		ini+=iters;
		
		while (iters>0) {		
			psiValue++;	
			*buffer++= psiValue;
						{
							if ((psiValue >=fstt)&& (psiValue <=sec)) {
									*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
									toreturn=1;
							}
							if ((psiValue >=fst)&& (psiValue <=sec)) {
								*i2= count; 
								toreturn=2;
							}
							if (psiValue >=sec) {
									return toreturn;
							}
							count ++;		
						}
			
			iters --;
		}
		
	}
	//@@
	
	size_t cpsiT = this->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
			
			//
			//size_t ptrnew = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
			//assert (ptr == ptrnew); //ptr is the same position as in the first loop
			//if (ptr != ptrnew) {
			//	printf("\n ptr = %lu and ptrnew = %lu do not match!\n", (ulong) ptr, (ulong) ptrnew);
			//}
		}
		else {
		
			ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
			
			if(huffmanCode < runLenghtStart) { 	// Incremento directo
				psiValue += huffmanCode;
			}	
			else 
				if(huffmanCode < negStart) {	// Estamos nun run
					runLenght = huffmanCode - runLenghtStart;
	/*				{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position;
						for (l=0;l<runLenght;l++) {
							psiValue++;
							*buffer++=psiValue;							
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}					 						
					*/
					
					{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position+1;   //@corregido 2014.05.01 instead of "end-position" xD!
						for (l=0;l<runLenght;l++) {
							psiValue++;
							*buffer++=psiValue;		

								{
									if ((psiValue >=fstt)&& (psiValue <=sec)) {
											*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
											toreturn=1;
									}
									if ((psiValue >=fst)&& (psiValue <=sec)) {
										*i2= count; 
										toreturn=2;
									}
									if (psiValue >=sec) {
											return toreturn;
									}
									count ++;		
								}
												
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}						 
				}
				else
					if(huffmanCode < bigStart) {	// Negativo
						binaryLenght = huffmanCode-negStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue -= absolute_value;	
					}
					else {	// Grande
						binaryLenght = huffmanCode-bigStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue += absolute_value;				 
					}				
			
		}
		*buffer++ = psiValue;

					{
						if ((psiValue >=fstt)&& (psiValue <=sec)) {
								*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
								toreturn=1;
						}
						if ((psiValue >=fst)&& (psiValue <=sec)) {
							*i2= count; 
							toreturn=2;
						}
						if (psiValue >=sec) {
								return toreturn;
						}
						count ++;		
					}
		
		position ++;
	} 
	

	return toreturn;
}

//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// sets in i2 de position (from ini on) of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found

int psi_rep_huffman_rle::getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = this->nS - 64 - this->T;
	unsigned int negStart = this->nS - 64;
	unsigned int bigStart = this->nS - 32;	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {
		
		ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
		
		if(huffmanCode < runLenghtStart) { 	// Incremento directo
			psiValue += huffmanCode;
		}	
		else 
			if(huffmanCode < negStart) {	// Estamos nun run
				runLenght = huffmanCode - runLenghtStart;
				if(index+runLenght>=positionsSinceSample){
					//return psiValue+positionsSinceSample-index;
					psiValue = psiValue+ positionsSinceSample-index;
					inRun=1;//@@
					inRunRemain = runLenght + index-positionsSinceSample;//@@
					break;
				}					
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
			}
			else
				if(huffmanCode < bigStart) {	// Negativo
					binaryLenght = huffmanCode-negStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue -= absolute_value;	
				}
				else {	// Grande
					binaryLenght = huffmanCode-bigStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue += absolute_value;				 
				}				
	}	
	//*buffer++= psiValue;  //value psi(ini)
	
		{
			if ((psiValue >=fstt)&& (psiValue <=sec)) {
					*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
					toreturn=1;
			}
			if ((psiValue >=fst)&& (psiValue <=sec)) {
				*i2= count; 
				toreturn=2;
			}
			if (psiValue >=sec) {
					return toreturn;
			}
			count ++;		
		}
	
	//@@ //--- AVOIDS PROBLES IN RUN... BUG FIXED on 2014.11.20
	if (inRun) {
		int iters = ((end-ini) >inRunRemain)? inRunRemain: (end-ini);
		ini+=iters;
		
		while (iters>0) {		
			psiValue++;	
			//*buffer++= psiValue;
						{
							if ((psiValue >=fstt)&& (psiValue <=sec)) {
									*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
									toreturn=1;
							}
							if ((psiValue >=fst)&& (psiValue <=sec)) {
								*i2= count; 
								toreturn=2;
							}
							if (psiValue >=sec) {
									return toreturn;
							}
							count ++;		
						}
			
			iters --;
		}
		
	}
	//@@
	
	size_t cpsiT = this->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
			
			//
			//size_t ptrnew = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
			//assert (ptr == ptrnew); //ptr is the same position as in the first loop
			//if (ptr != ptrnew) {
			//	printf("\n ptr = %lu and ptrnew = %lu do not match!\n", (ulong) ptr, (ulong) ptrnew);
			//}
		}
		else {
		
			ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
			
			if(huffmanCode < runLenghtStart) { 	// Incremento directo
				psiValue += huffmanCode;
			}	
			else 
				if(huffmanCode < negStart) {	// Estamos nun run
					runLenght = huffmanCode - runLenghtStart;
	/*				{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position;
						for (l=0;l<runLenght;l++) {
							psiValue++;
							*buffer++=psiValue;							
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}					 						
					*/
					
					{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position+1;   //@corregido 2014.05.01 instead of "end-position" xD!
						for (l=0;l<runLenght;l++) {
							psiValue++;
							//*buffer++=psiValue;		

								{
									if ((psiValue >=fstt)&& (psiValue <=sec)) {
											*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
											toreturn=1;
									}
									if ((psiValue >=fst)&& (psiValue <=sec)) {
										*i2= count; 
										toreturn=2;
									}
									if (psiValue >=sec) {
											return toreturn;
									}
									count ++;		
								}
												
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}						 
				}
				else
					if(huffmanCode < bigStart) {	// Negativo
						binaryLenght = huffmanCode-negStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue -= absolute_value;	
					}
					else {	// Grande
						binaryLenght = huffmanCode-bigStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue += absolute_value;				 
					}				
			
		}
		//*buffer++ = psiValue;

					{
						if ((psiValue >=fstt)&& (psiValue <=sec)) {
								*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
								toreturn=1;
						}
						if ((psiValue >=fst)&& (psiValue <=sec)) {
							*i2= count; 
							toreturn=2;
						}
						if (psiValue >=sec) {
								return toreturn;
						}
						count ++;		
					}
		
		position ++;
	} 
	

	return toreturn;
}





//different processing of the runs in getFindxx() below.
//By default the OPT_A is faster than Option_B (rather unexpectedly: no long runs?)
//set to OPT_A by default. Comment next line to set Option_B
#define PSIHUFFMANRLE3R_RUNPROCESS_OPT_A

//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// stops if i1 was set
// returns 0 if all values are < fst.
   // NO LONGER returns 1 if a value >= fst and <=sec was found
// returns x+1, where x is  the first value >= fst and <=sec was found    
// (+1 to ensure zero is not returned as a valid value) --> we can distinghis from the case were non value found (and returns 0)


ulong psi_rep_huffman_rle::getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {
//ulong getfindLeftOnlyHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = this->nS - 64 - this->T;
	unsigned int negStart = this->nS - 64;
	unsigned int bigStart = this->nS - 32;	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {
		
		ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
		
		if(huffmanCode < runLenghtStart) { 	// Incremento directo
			psiValue += huffmanCode;
		}	
		else 
			if(huffmanCode < negStart) {	// Estamos nun run
				runLenght = huffmanCode - runLenghtStart;
				if(index+runLenght>=positionsSinceSample){
					//return psiValue+positionsSinceSample-index;
					psiValue = psiValue+ positionsSinceSample-index;
					inRun=1;//@@
					inRunRemain = runLenght + index-positionsSinceSample;//@@
					break;
				}					
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
			}
			else
				if(huffmanCode < bigStart) {	// Negativo
					binaryLenght = huffmanCode-negStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue -= absolute_value;	
				}
				else {	// Grande
					binaryLenght = huffmanCode-bigStart+1;
					absolute_value = bitread(this->stream,ptr,binaryLenght);
					ptr += binaryLenght;
					psiValue += absolute_value;				 
				}				
	}	
	
	
	//*buffer++= psiValue;  //value psi(ini)
	
		{
			if ((psiValue >=fstt)&& (psiValue <=sec)) {
					*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
					toreturn=1;
					//return toreturn;  //////////////////////////////////////////////////////////77
					return psiValue +1;  //////////////////////////////////////////////////////////77
			}
			//if ((psiValue >=fst)&& (psiValue <=sec)) {
			//	*i2= count; 
			//	toreturn=2;
			//}
			if (psiValue >=sec) {
					return toreturn;
			}
			count ++;		
		}
	
		//@@ //--- AVOIDS PROBLES IN RUN... BUG FIXED on 2014.11.20
		if (inRun) {
			int iters = ((end-ini) >inRunRemain)? inRunRemain: (end-ini);
			ini+=iters;
		
#ifdef PSIHUFFMANRLE3R_RUNPROCESS_OPT_A
/** option A **/	 //absolute_value/typically slightly faster with t_psi=16 (yet rather unexpectedly)
					/*//fari@2023 */ while (iters>0) {		
					/*//fari@2023 */ 	psiValue++;	
					/*//fari@2023 */ 	//*buffer++= psiValue;
					/*//fari@2023 */ 				{
					/*//fari@2023 */ 					if ((psiValue >=fstt)&& (psiValue <=sec)) {
					/*//fari@2023 */ 							*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
					/*//fari@2023 */ 							toreturn=1;
					/*//fari@2023 */							//return toreturn;  //////////////////////////////////////////////////////////
					/*//fari@2023 */							return psiValue +1;  //////////////////////////////////////////////////////////
					/*//fari@2023 */ 							
					/*//fari@2023 */ 					}
					/*//fari@2023 */ 					// if ((psiValue >=fst)&& (psiValue <=sec)) {
					/*//fari@2023 */ 					// 	*i2= count; 
					/*//fari@2023 */ 					// 	toreturn=2;
					/*//fari@2023 */ 					// }
					/*//fari@2023 */ 					if (psiValue >=sec) {
					/*//fari@2023 */ 							return toreturn;
					/*//fari@2023 */ 					}
					/*//fari@2023 */ 					count ++;		
					/*//fari@2023 */ 				}
					/*//fari@2023 */ 	
					/*//fari@2023 */ 	iters --;
					/*//fari@2023 */ }
#else					
/** option B **/		//  NEXT WORKS, BUT DID NOT IMPROVED UPON THE PREVIOUS while !! :(	 //2023.02.05
					/*fari@2023*/	uint gapToFst = fstt - psiValue;
					/*fari@2023*/	if (psiValue < fstt) {
					/*fari@2023*/		if (iters >=gapToFst) {*i1=gapToFst+count-1; return (1+ psiValue+gapToFst);}
					/*fari@2023*/	}
					/*fari@2023*/	psiValue +=iters;
					/*fari@2023*/	count +=iters;
#endif
			
		}
		//@@



/******** ESTA PARTE NO DEBERÍA HACER FALTA PUES NO BUSCAMOS RIGHT ********/
	
	size_t cpsiT = this->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
			
			//
			//size_t ptrnew = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
			//assert (ptr == ptrnew); //ptr is the same position as in the first loop
			//if (ptr != ptrnew) {
			//	printf("\n ptr = %lu and ptrnew = %lu do not match!\n", (ulong) ptr, (ulong) ptrnew);
			//}
		}
		else {
		
			ptr = decodeHuff(&this->diffsHT,&huffmanCode,this->stream,ptr);
			
			if(huffmanCode < runLenghtStart) { 	// Incremento directo
				psiValue += huffmanCode;
			}	
			else 
				if(huffmanCode < negStart) {	// Estamos nun run
					runLenght = huffmanCode - runLenghtStart;
	/*				{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position;
						for (l=0;l<runLenght;l++) {
							psiValue++;
							*buffer++=psiValue;							
						}
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}					 						
					*/
					
					{uint l;
						if (runLenght > (end-position)) 
							runLenght = end-position+1;   //@corregido 2014.05.01 instead of "end-position" xD!

#ifdef PSIHUFFMANRLE3R_RUNPROCESS_OPT_A
/** option A **/	 //typically slightly faster with t_psi=16 (yet rather unexpectedly)							
					 /*fari@2023 */	for (l=0;l<runLenght;l++) {
					 /*fari@2023 */		psiValue++;
					 /*fari@2023 */		//*buffer++=psiValue;		
					 /*fari@2023 */	
					 /*fari@2023 */			{
					 /*fari@2023 */				if ((psiValue >=fstt)&& (psiValue <=sec)) {
					 /*fari@2023 */						*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
					 /*fari@2023 */						toreturn=1;
					 /*fari@2023 */							//return toreturn;  //////////////////////////////////////////////////////////
					 /*fari@2023 */							return psiValue+1;  //////////////////////////////////////////////////////////
					 /*fari@2023 */				}
					 /*fari@2023 */				//if ((psiValue >=fst)&& (psiValue <=sec)) {
					 /*fari@2023 */				//	*i2= count; 
					 /*fari@2023 */				//	toreturn=2;
					 /*fari@2023 */				//}
					 /*fari@2023 */				if (psiValue >=sec) {
					 /*fari@2023 */						return toreturn;
					 /*fari@2023 */				}
					 /*fari@2023 */				count ++;		
					 /*fari@2023 */			}
					 /*fari@2023 */							
					 /*fari@2023 */	}
#else
/** option B**/
					           //NEXT WORKS, BUT DID NOT IMPROVED UPON THE PREVIOUS while !! :(	 //2023.02.05						
						/*fari@2023*/	uint gapToFst = fstt - psiValue;
						/*fari@2023*/	if (psiValue < fstt) {
						/*fari@2023*/		if (runLenght >=gapToFst) {*i1=gapToFst+count-1; return (1+psiValue+gapToFst);}
						/*fari@2023*/	}
						/*fari@2023*/	psiValue +=runLenght;
						/*fari@2023*/	count +=runLenght;
						/*fari@2023*/	if (psiValue >=sec) { return 0;}
#endif						
												
						position +=runLenght;
						continue;   //skips the end of the loop (ten lines below)
					}						 
				}
				else
					if(huffmanCode < bigStart) {	// Negativo
						binaryLenght = huffmanCode-negStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue -= absolute_value;	
					}
					else {	// Grande
						binaryLenght = huffmanCode-bigStart+1;
						absolute_value = bitread(this->stream,ptr,binaryLenght);
						ptr += binaryLenght;
						psiValue += absolute_value;				 
					}				
			
		}
		//*buffer++ = psiValue;

					{
						if ((psiValue >=fstt)&& (psiValue <=sec)) {
								*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
								toreturn=1;
			/*//fari@2023 */	//return toreturn;  //////////////////////////////////////////////////////////
			/*//fari@2023 */	return psiValue+1;  //////////////////////////////////////////////////////////
						}
						//if ((psiValue >=fst)&& (psiValue <=sec)) {
						//	*i2= count; 
						//	toreturn=2;
						//}
						if (psiValue >=sec) {
								return toreturn;
						}
						count ++;		
					}
		
		position ++;
	} 
	

	return toreturn;
}


























void psi_rep_huffman_rle::psi_save(char *filename) {
//void storeHuffmanCompressedPsiR3(HuffmanCompressedPsiR3 *compressedPsi, char *filename) {

	FILE *file;
	THuff H;

	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	size_t write_err;
	uint psitype = PSIREP_HUFFMAN_RLE;
	write_err=fwrite(&(psitype), sizeof(uint)  ,1,file);
	
	write_err=fwrite(&(this->T), sizeof(int),1,file);

#ifdef 	R3H_WITHGAPS
    //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
	write_err=fwrite(&(this->OFFSET), sizeof(size_t),1,file);
	//printf("\n **************** saved offset = %zu ***************************************",this->OFFSET);
	fflush(stderr);fflush(stdout);
	
#endif
	
	// Almacenar o arbol de huffman
	H = this->diffsHT;
	write_err=fwrite(  &H.max, sizeof(int),1,file);
	write_err=fwrite(  &H.lim, sizeof(int),1,file);
	write_err=fwrite(  &H.depth, sizeof(int),1,file);
//	write( file, H.s.spos, (H.lim+1)*sizeof(int));
	write_err=fwrite(  H.s.symb,sizeof(int), (H.lim+1),file);	
	write_err=fwrite(  H.num,sizeof(int), (H.depth+1),file);
	write_err=fwrite(  H.fst,sizeof(int), (H.depth+1),file);
	// Fin de almacenar o arbol de huffman
	write_err=fwrite(  &(this->nS), sizeof(int),1,file);
	write_err=fwrite(  &(this->numberOfSamples), sizeof(size_t),1,file);
	write_err=fwrite(  &(this->sampleSize), sizeof(int),1,file);
	write_err=fwrite( 	this->samples, sizeof(int), (((size_t)this->numberOfSamples*this->sampleSize+W-1)/W), file);	
	write_err=fwrite(  &(this->pointerSize), sizeof(int),1,file);
	write_err=fwrite( 	this->samplePointers, sizeof(size_t), (((size_t)this->numberOfSamples*this->pointerSize+WW-1)/WW), file);
	write_err=fwrite(  &(this->streamSize), sizeof(size_t),1,file);
	write_err=fwrite( 	this->stream, sizeof(int),((this->streamSize+W-1)/W) , file);
	size_t byteswritentostream = write_err;
	write_err=fwrite(  &(this->totalMem), sizeof(size_t),1,file);

/*

	printf("\n\n to save");
	printf("\n this->NS = %u",this->nS);
	printf("\n numberofsamples = %zu",this->numberOfSamples);
	printf("\n samplesize_Bits = %u bits",this->sampleSize);
	printf("\n tamaño de array samples (bytes) = %zu", (((size_t)this->numberOfSamples*this->sampleSize+W-1)/W)*sizeof(int));	
	printf("\n pointersize_Bits = %u bits",this->pointerSize);
	printf("\n tamaño de array pointers = %zu (bytes)", (( (size_t)this->numberOfSamples*this->pointerSize+WW-1)/WW)*sizeof(size_t) );
	printf("\n streamsize (lonxitude en bits) = %zu",this->streamSize);
	printf("\n stream (tamaño en bytes) = %zu",	((this->streamSize+W-1)/W)*sizeof(int));
	printf("\n      bytes writen for **stream = %zu**",	sizeof(uint)*byteswritentostream);
	
	printf("\n totalMem compressed psi = %zu bytes", this->totalMem);
	fflush(stdout);fflush(stderr);
*/
	
	fclose(file);	

}


psi_rep_huffman_rle * psi_rep_huffman_rle::psi_load (char *filename) {
//PlainPsi loadPlainPsi(char *filename) {	
	//PlainPsi compressedPsi;
	//	//		compressedPsi.samples=NULL;
	//	//		compressedPsi.psi=NULL;

	psi_rep_huffman_rle *ret = new psi_rep_huffman_rle();


 	THuff H;
     
	FILE *file;

	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}
		fflush(stdout);fflush(stderr);
	
	size_t read_err;


	uint psitype = PSIREP_HUFFMAN_RLE;
	read_err=fread(&psitype, sizeof(int), 1, file);
	if (psitype !=PSIREP_HUFFMAN_RLE) {
		perror("\npsi_load failed (wrong type psi_rep_huffman_rle):");
		exit(0);
	}
	
	read_err=fread(&(ret->T), sizeof(int), 1, file);

#ifdef 	R3H_WITHGAPS
    //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
	read_err=fread(&(ret->OFFSET), sizeof(size_t),1,file);
	//printf("\n **************** loaded offset = %zu ***************************************",ret->OFFSET);
	//fflush(stderr);fflush(stdout);
	
	printf("\n-loading PSI_HUFFMAN_RLE (with-gaps)");	
#endif
#ifndef R3H_WITHGAPS
	printf("\n-loading PSI_HUFFMAN_RLE");
#endif


	
	// Cargamos o arbol de Huffman
	read_err=fread( &H.max, sizeof(int),1, file);
	read_err=fread( &H.lim, sizeof(int),1, file);
	read_err=fread( &H.depth, sizeof(int),1, file);
	//H.s.spos = (unsigned int *) malloc((H.lim+1)*sizeof(int));
	//H.s.spos =H.s.symb = (unsigned int *) malloc((H.lim+1)*sizeof(int));
	H.s.symb = (unsigned int *) malloc((H.lim+1)*sizeof(int));
	H.num = (unsigned int *) malloc((H.depth+1)*sizeof(int));	
	H.fst = (unsigned int *) malloc((H.depth+1)*sizeof(int));	

	//read(file, H.s.spos, (H.lim+1)*sizeof(int));
	//fprintf(stderr," \n read %d spos bytes\n", (H.lim+1)*sizeof(int));
	read_err=fread( H.s.symb, sizeof(int), (H.lim+1),file);	

	read_err=fread( H.num, sizeof(int), (H.depth+1),file);
	read_err=fread( H.fst, sizeof(int), (H.depth+1),file);	
	ret->diffsHT = H;
	// Fin da carga do arbol de Huffman
	read_err=fread( &(ret->nS), sizeof(int), 1,file);
	read_err=fread( &(ret->numberOfSamples), sizeof(size_t), 1, file);
	
	read_err=fread( &(ret->sampleSize), sizeof(int), 1,file);
	ret->samples = (unsigned int *)malloc((( (size_t)ret->numberOfSamples*ret->sampleSize+W-1)/W)*sizeof(int));
	read_err=fread( ret->samples, sizeof(int), (( (size_t)ret->numberOfSamples*ret->sampleSize+W-1)/W) , file );
	
	read_err=fread( &(ret->pointerSize), sizeof(int),1,file);
	ret->samplePointers = (size_t *)malloc((( (size_t)ret->numberOfSamples*ret->pointerSize+WW-1)/WW)*sizeof(size_t));
	read_err=fread( ret->samplePointers, sizeof(size_t), (( (size_t)ret->numberOfSamples*ret->pointerSize+WW-1)/WW), file);
	
	read_err=fread( &(ret->streamSize), sizeof(size_t),1,file);
	ret->stream = (unsigned int *)malloc(((ret->streamSize+W-1)/W)*sizeof(int));
	size_t readbytesstream=fread( ret->stream, sizeof(uint), ((ret->streamSize+W-1)/W), file);
	read_err=fread( &(ret->totalMem), sizeof(size_t),1,file);
	printf("::  total bytes = %zu",ret->totalMem);
	
	
/*

	printf("\n\n to load");
	printf("\n compressedPSI->NS = %u",ret->nS);
	printf("\n numberofsamples = %zu",ret->numberOfSamples);
	printf("\n samplesize_Bits = %u bits",ret->sampleSize);
	printf("\n tamaño de array samples (bytes) = %zu", (((size_t)ret->numberOfSamples*ret->sampleSize+W-1)/W)*sizeof(int));	
	printf("\n pointersize_Bits = %u bits",ret->pointerSize);
	printf("\n tamaño de array pointers = %zu (bytes)", (( (size_t)ret->numberOfSamples*ret->pointerSize+WW-1)/WW)*sizeof(size_t) );
	printf("\n streamsize (lonxitude en bits) = %zu",ret->streamSize);
	printf("\n stream (tamaño en bytes) = %zu",	((ret->streamSize+W-1)/W)*sizeof(int));
	printf("\n      bytes read for **stream = %zu**",	sizeof(uint)*readbytesstream);
	                                            
	printf("\n totalMem compressed psi = %zu bytes", ret->totalMem);
	fflush(stdout);fflush(stderr);
*/	

	fclose(file);
	return ret;

}


