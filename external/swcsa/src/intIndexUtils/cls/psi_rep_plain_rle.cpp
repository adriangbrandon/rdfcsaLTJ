

#include "psi_rep_plain_rle.h"

// IMPLEMENTACION DAS FUNCIONS

psi_rep_plain_rle::~psi_rep_plain_rle() {

	if (samples)
		free(samples);

	if (samplePointers)
		free(samplePointers);

	if (stream)
		free(stream);

}


psi_rep_plain_rle::psi_rep_plain_rle() {
	totalMem=0;

	T=0;
	//nS=0;
	
	#ifdef 	R3H_WITHGAPS
		OFFSET=0;               
	#endif
	
	numberOfSamples=0;

	sampleSize=0;			
	samples=NULL;			
	
	pointerSize=0;			
	samplePointers=NULL;	

	streamSize=0;			
	stream=NULL;			
	
	L=0;
	LL=0;
	Lpsi=0;	
	
}


psi_rep_plain_rle::psi_rep_plain_rle(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T=DEFAULT_T_PSI_VALUE) {
	build(Psi,psiSize,inioffset,T,0);
}

psi_rep_plain_rle::psi_rep_plain_rle(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE, unsigned int nS=0) { //, uint *psiTypes=NULL){
	build(Psi,psiSize,inioffset,T,0);
}

//	void psi_rep_plain_rle::build(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE, unsigned int nS=0) {
//		build(Psi,psiSize,inioffset,T);
//	}


/** Internal building function, same parameters as the base constructor. */
//	void psi_rep_plain_rle::build(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE) {

void psi_rep_plain_rle::build(unsigned int *Psi, size_t psiSize,  size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE, /*--> not used -->*/ unsigned int nS=0) {
	

	printf("\n call to psi_rep_plain_rle - compress Psi\n");fflush(stdout);fflush(stderr);

#ifdef 	R3H_WITHGAPS	
	this->OFFSET = inioffset;
	//printf("\n **************** offset = %zu ***************************************",inioffset);
#endif	
	size_t maxval3R = 0;               //required to now the max-value that must be used in the samples.
	
	uint absolute_value;
	register size_t index;
	register size_t ptr, samplesPtr, samplePointersPtr;
	unsigned int runLenght, binaryLenght;
	
	ssize_t *diffs;	
	
	// Estructuras da funcion comprimida (para logo asignar)
	// Tamen se podian almacenar directamente
	//THuff diffsHT;
	size_t numberOfSamples;
	unsigned int *samples;

	unsigned int sampleSize;
	size_t *samplePointers;
	
	unsigned int pointerSize;
	unsigned int *stream;
	size_t streamSize;
	

	uint L = bits(psiSize-1);
	uint LL = bits(L);
	uint Lpsi = bits(T-1);

	
	// Para estadistica
	size_t totalSize;
	
	// Inicializamos diferencias	
	diffs = (ssize_t *)malloc(sizeof(ssize_t)*psiSize);	

	
	diffs[0] = 0;
	for(index=1; index<psiSize; index++) {
		diffs[index] = ((ssize_t)Psi[index]) - ((ssize_t)Psi[index-1]);
	}


	/**********/ 	// Calculamos  streamSize 		

	streamSize=0;
	runLenght = 0;
	for(index=0; index<psiSize; index++) {

		if(index%T) {
			
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght++;
			} else {	// Non estamos nun run
				if(runLenght) {
					streamSize += 2+Lpsi;
					runLenght = 0;
				}
				
				if (diffs[index]>((ssize_t)1) ) { //positivo
					absolute_value = (uint)(diffs[index]);
					binaryLenght = bits(absolute_value);
					if (binaryLenght > 32)
							printf("\n OJO, que absolute_value vale %u",absolute_value);
					streamSize += 1+LL+binaryLenght;
				}
				else {	// Valor negativo
					absolute_value = (uint) (-diffs[index]);
					binaryLenght = bits(absolute_value);					
					if (binaryLenght > 32)
							printf("\n OJO, que absolute_value vale %u",absolute_value);				
					streamSize += 2+LL+binaryLenght;
				} 
			}
			
		} else { // Rompemos o run porque atopamos unha mostra
			if(runLenght) {
				streamSize += 2+Lpsi;
				runLenght = 0;
			}
		}		
	}
		
	if(runLenght) streamSize += 2+Lpsi;

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
	{uint ii;
		for (ii=0; ii <= ((streamSize+W-1)/W)-1 ; ii++) stream[ii]=0000;
	}
	
	fprintf(stderr,"\n espacio para stream-psi = %lu", sizeof(int)*((streamSize+W-1)/W) );
	
	// Comprimimos secuencialmente (haber� que levar un punteiro desde o inicio)
	ptr = 0;
	samplesPtr = 0;
	samplePointersPtr = 0;
	runLenght = 0;
	
	for(index=0; index<psiSize; index++) {

		if(index%T) {
			
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght++;
			} else {	// Non estamos nun run
				if(runLenght) {                    /* encodes a run */
					bitset(stream,ptr); ptr++;   //sets "1"
					bitset(stream,ptr); ptr++;   //sets "1"
						absolute_value = (uint) (runLenght);
						binaryLenght = Lpsi;
					bitwrite(stream,ptr,binaryLenght,absolute_value);
					ptr += binaryLenght;
					runLenght = 0;
					// streamSize += 2+Lpsi;
				}
				
				if (diffs[index]>((ssize_t) 1) ) { /* encodes a positive value */
					absolute_value = (uint)(diffs[index]);
					binaryLenght = bits(absolute_value);
					if (binaryLenght > 32)
							printf("\n OJO, que absolute_value vale %u",absolute_value);

					bitclean(stream,ptr); ptr++;   //sets "0"
					bitwrite(stream,ptr,LL,binaryLenght); 
					ptr += LL;
					bitwrite(stream,ptr,binaryLenght,absolute_value);
					ptr += binaryLenght;
							
					//streamSize += 1+LL+binaryLenght;
				}
				else {	                           /* encodes a negative value */
					absolute_value = (uint) (-diffs[index]);
					binaryLenght = bits(absolute_value);
					if (binaryLenght > 32)
							printf("\n OJO, que absolute_value vale %u",absolute_value);

					bitset(stream,ptr);   ptr++;   //sets "1"
					bitclean(stream,ptr); ptr++;   //sets "0"
					bitwrite(stream,ptr,LL,binaryLenght); 
					ptr += LL;
					bitwrite(stream,ptr,binaryLenght,absolute_value);
					ptr += binaryLenght;
				
					//streamSize += 2+LL+binaryLenght;
				} 
			}
			
		} else { // Rompemos o run porque atopamos unha mostra
			if(runLenght) {
					bitset(stream,ptr); ptr++;   //sets "1"
					bitset(stream,ptr); ptr++;   //sets "1"
						absolute_value = (uint) (runLenght);
						binaryLenght = Lpsi;
					bitwrite(stream,ptr,binaryLenght,absolute_value);
					ptr += binaryLenght;
					runLenght = 0;
					// streamSize += 2+Lpsi;
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
			
		}
		
	}
	
	if(runLenght) {	
		bitset(stream,ptr); ptr++;   //sets "1"
		bitset(stream,ptr); ptr++;   //sets "1"
			absolute_value = (uint) (runLenght);
			binaryLenght = Lpsi;
		bitwrite(stream,ptr,binaryLenght,absolute_value);
		ptr += binaryLenght;
		runLenght = 0;
		// streamSize += 2+Lpsi;
	}
	
	// Amosamos o espacio ocupado
	
	totalSize = ((size_t)  sizeof(psi_rep_plain_rle)) + 
		sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W) + 
		sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW) +
		sizeof(int)*((size_t)(streamSize+W-1)/W) ;
		
		//fprintf(stderr,"\n@@@@@@@@@ psi samaplePeriod= %u, ns=%u ", T,nS);
		fprintf(stderr,"\n@@@@@@@@@ psi samaplePeriod= %u ", T);
		fprintf(stderr,"\n@@@@@@@@@ psi size= [samples = %lu] bytes ", sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [pointers = %lu] bytes ",sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [streamsize=%lu] bytes ", sizeof(int)*((size_t)(streamSize+W-1)/W));

	//@@printf("\n\t Compressed Psi size = %zu bytes, with %d different symbols.", totalSize, nS);
	
	
	// Asignamos os valores a cPsi e devolvemolo
	this->T = T;
	//this->diffsHT = diffsHT;
	//this->nS = nS;
	this->numberOfSamples = numberOfSamples;
	this->samples = samples;
	this->sampleSize = sampleSize;
	this->samplePointers = samplePointers;
	this->pointerSize = pointerSize;
	this->stream = stream;
	this->streamSize = streamSize;
	this->totalMem = totalSize;
	//printf("\n\t >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Compressed Psi size = %zu bytes.", this->totalMem);

	this->L = L;
	this->LL= LL;
	this->Lpsi=Lpsi;
	

/*
{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
	fprintf(stderr,"\n Test compress/uncompress PSI - psi_rep_plain_rle is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
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
					//if (count > 50) 
						{ exit(0);;}
			}
	}
		fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
} /////////////////////////////////////////////////////////////////////////////////////
*/

	
	//frees resources not needed in advance
	free(diffs);
	//returns the data structure that holds the compressed psi.
	//return cPsi;	
}





//for Nieves' statistics
ulong psi_rep_plain_rle::getPsiSizeBitsUptoPosition(size_t position) { 

	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	//unsigned int runLenghtStart = this->nS - 64 - this->T;
	//unsigned int negStart = this->nS - 64;
	//unsigned int bigStart = this->nS - 32;	
	
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
		psiValue +=this->OFFSET;
#endif				
	
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;
		
	for(index=0;index<positionsSinceSample;index++) {
		uint bit1 = bitget(this->stream,ptr); ptr++;
		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
			absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			uint bit2 = bitget(this->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = this->Lpsi;
				runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample)
					//return psiValue+positionsSinceSample-index;
					break;
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
							
			}
			
		}			
	} 
	
	return (unsigned long) (ptr + sampleIndex * (this->sampleSize + this->pointerSize) );

}



unsigned int psi_rep_plain_rle::getPsiValue( size_t position) {
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;

	uint bit1,bit2;
	
	for(index=0;index<positionsSinceSample;index++) {
		bit1 = bitget(this->stream,ptr); ptr++;
		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
			absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(this->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = this->Lpsi;
				runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample)
					return psiValue+positionsSinceSample-index;
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
							
			}
			
		}

			
	}
	
	return psiValue;
}


void psi_rep_plain_rle::getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end) {
//void getPlainPsiValueBuffer_1(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getPsiValue(i);
	return;
}


void psi_rep_plain_rle::getPsiValueBuffer(uint *buffer, size_t ini, size_t end) {	
//void getPlainPsiValueBuffer(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {

	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(this->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
			absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(this->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = this->Lpsi;
				runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample) {
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
		}
		else {
		
			bit1 = bitget(this->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(this->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
					absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = this->Lpsi;
					runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
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

int psi_rep_plain_rle::getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindHuffmanPsiR3ValueBuffer(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(this->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
			absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(this->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = this->Lpsi;
				runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample) {
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
		}
		else {
		
			bit1 = bitget(this->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(this->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
					absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = this->Lpsi;
					runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
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

int psi_rep_plain_rle::getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
		
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(this->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
			absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(this->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = this->Lpsi;
				runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample) {
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
		}
		else {
		
			bit1 = bitget(this->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(this->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
					absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = this->Lpsi;
					runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
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


ulong psi_rep_plain_rle::getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {
//ulong getfindLeftOnlyHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
		
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / this->T;
	psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
	
	ptr = bitread64(this->samplePointers,sampleIndex*this->pointerSize,this->pointerSize);
	
	positionsSinceSample = position%this->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(this->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
			absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(this->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = this->Lpsi;
				runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample) {
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
		//@@
	
	size_t cpsiT = this->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(this->samples,sampleIndex*this->sampleSize,this->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=this->OFFSET;
#endif				
		}
		else {
		
			bit1 = bitget(this->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(this->stream,ptr,this->LL);     ptr += this->LL;
				absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(this->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(this->stream,ptr,this->LL); 	ptr += this->LL;
					absolute_value= bitread(this->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = this->Lpsi;
					runLenght    = bitread(this->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
					
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


























void psi_rep_plain_rle::psi_save(char *filename) {
//void storeHuffmanCompressedPsiR3(HuffmanCompressedPsiR3 *compressedPsi, char *filename) {

	FILE *file;
	THuff H;

	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	size_t write_err;
	uint psitype = PSIREP_PLAIN_RLE;
	write_err=fwrite(&(psitype), sizeof(uint)  ,1,file);
	
	write_err=fwrite(&(this->T), sizeof(int),1,file);

#ifdef 	R3H_WITHGAPS
    //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
	write_err=fwrite(&(this->OFFSET), sizeof(size_t),1,file);
	//printf("\n **************** saved offset = %zu ***************************************",this->OFFSET);
	fflush(stderr);fflush(stdout);
	
#endif
	
//	// Almacenar o arbol de huffman
//	H = this->diffsHT;
//	write_err=fwrite(  &H.max, sizeof(int),1,file);
//	write_err=fwrite(  &H.lim, sizeof(int),1,file);
//	write_err=fwrite(  &H.depth, sizeof(int),1,file);
//	//	write( file, H.s.spos, (H.lim+1)*sizeof(int));
//	write_err=fwrite(  H.s.symb,sizeof(int), (H.lim+1),file);	
//	write_err=fwrite(  H.num,sizeof(int), (H.depth+1),file);
//	write_err=fwrite(  H.fst,sizeof(int), (H.depth+1),file);
//	// Fin de almacenar o arbol de huffman
	
	//write_err=fwrite(  &(this->nS), sizeof(int),1,file);
	write_err=fwrite(  &(this->numberOfSamples), sizeof(size_t),1,file);
	write_err=fwrite(  &(this->sampleSize), sizeof(int),1,file);
	write_err=fwrite( 	this->samples, sizeof(int), (((size_t)this->numberOfSamples*this->sampleSize+W-1)/W), file);	
	write_err=fwrite(  &(this->pointerSize), sizeof(int),1,file);
	write_err=fwrite( 	this->samplePointers, sizeof(size_t), (((size_t)this->numberOfSamples*this->pointerSize+WW-1)/WW), file);
	write_err=fwrite(  &(this->streamSize), sizeof(size_t),1,file);
	write_err=fwrite( 	this->stream, sizeof(int),((this->streamSize+W-1)/W) , file);
	size_t byteswritentostream = write_err;
	write_err=fwrite(  &(this->totalMem), sizeof(size_t),1,file);


	write_err=fwrite(  &(this->L), sizeof(int),1,file);
	write_err=fwrite(  &(this->LL), sizeof(int),1,file);
	write_err=fwrite(  &(this->Lpsi), sizeof(int),1,file);


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


psi_rep_plain_rle * psi_rep_plain_rle::psi_load (char *filename) {
//PlainPsi loadPlainPsi(char *filename) {	
	//PlainPsi compressedPsi;
	//	//		compressedPsi.samples=NULL;
	//	//		compressedPsi.psi=NULL;

	psi_rep_plain_rle *ret = new psi_rep_plain_rle();


 	THuff H;
     
	FILE *file;

	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}
		fflush(stdout);fflush(stderr);
	
	size_t read_err;


	uint psitype = PSIREP_PLAIN_RLE;
	read_err=fread(&psitype, sizeof(int), 1, file);
	if (psitype !=PSIREP_PLAIN_RLE) {
		perror("\npsi_load failed (wrong type psi_rep_plain_rle):");
		exit(0);
	}
	
	read_err=fread(&(ret->T), sizeof(int), 1, file);

#ifdef 	R3H_WITHGAPS
    //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
	read_err=fread(&(ret->OFFSET), sizeof(size_t),1,file);
	//printf("\n **************** loaded offset = %zu ***************************************",ret->OFFSET);
	//fflush(stderr);fflush(stdout);

	printf("\n-loading PSI_PLAIN_RLE (with-gaps)");	
#endif
#ifndef R3H_WITHGAPS
	printf("\n-loading PSI_PLAIN_RLE");
#endif
	
//	// Cargamos o arbol de Huffman
//	read_err=fread( &H.max, sizeof(int),1, file);
//	read_err=fread( &H.lim, sizeof(int),1, file);
//	read_err=fread( &H.depth, sizeof(int),1, file);
//	//H.s.spos = (unsigned int *) malloc((H.lim+1)*sizeof(int));
//	//H.s.spos =H.s.symb = (unsigned int *) malloc((H.lim+1)*sizeof(int));
//	H.s.symb = (unsigned int *) malloc((H.lim+1)*sizeof(int));
//	H.num = (unsigned int *) malloc((H.depth+1)*sizeof(int));	
//	H.fst = (unsigned int *) malloc((H.depth+1)*sizeof(int));	
//
//	//read(file, H.s.spos, (H.lim+1)*sizeof(int));
//	//fprintf(stderr," \n read %d spos bytes\n", (H.lim+1)*sizeof(int));
//	read_err=fread( H.s.symb, sizeof(int), (H.lim+1),file);	
//
//	read_err=fread( H.num, sizeof(int), (H.depth+1),file);
//	read_err=fread( H.fst, sizeof(int), (H.depth+1),file);	
//	ret->diffsHT = H;
//	// Fin da carga do arbol de Huffman

	//read_err=fread( &(ret->nS), sizeof(int), 1,file);
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

	read_err=fread( &(ret->L), sizeof(int), 1,file);
	read_err=fread( &(ret->LL), sizeof(int), 1,file);
	read_err=fread( &(ret->Lpsi), sizeof(int), 1,file);
	
	
/*

	printf("\n\n to load");
	printf("\n ret->NS = %u",ret->nS);
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


