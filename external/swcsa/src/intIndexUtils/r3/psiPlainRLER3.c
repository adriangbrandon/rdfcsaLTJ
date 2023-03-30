/* psiHuffmanRLE.c
 * Copyright (C) 2011, Antonio Fariña and Eduardo Rodriguez, all rights reserved.
 * Improved representation based on a previous version from Gonzalo Navarro's.
 *
 * psiHuffmanRLE.c: Compressed Representation for Psi in CSA.
 *   It uses Huffman+RLE to encode:
 *     1-runs, short-values(+), negative values(-) and long values(+)
 *
 * More details in:
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

/* About this Version:
 *   Psi divided into 3 regions from RDFCSA.
 *   Version psiR3H: 
 *   psiPlainRLER3c: differs from psiHuffmanRleR3.c in the fact we  no longer use
 * 		Huffman. We use a plain representation for Runs, positive and negative values
 * 		Positive val using K bits :  0     + LL bits (bits(K)) +  K bits (+value).
 * 		Negative val using K bits :  1 + 0 + LL bits (bits(K)) +  K bits (-value).
 * 		Run of len 1ones          :  1 + 1 + Lpsi bits (len)
 * 
 * As in psiHuffmanRleR3.c
 *     the absolute samples must know have values greater than psiSize in
 *     the case or ranges for Subjects and Predicates, since all the ranges
 *     have n/3, n/3, and n/3+1 entries respectively, and the values of the
 *      range of Subjects  has values in [n/3, 2n/3-1]
 *      range of Predicate has values in [2n/3, n-1]
 *      range of Objects   has values in [0, n-1]
 *     due to that, the number of bits required must be computed over the 
 *    input array of values (line 200 aprox): sampleSize = bits(maxval3R);
 */
 
#include "psiPlainRLER3.h"

// IMPLEMENTACION DAS FUNCIONS

void destroyHuffmanCompressedPsiR3(HuffmanCompressedPsiR3 *compressedPsi) {
	free(compressedPsi->samples);
	free(compressedPsi->samplePointers);
	free (compressedPsi->stream);
}

//HuffmanCompressedPsi huffmanCompressPsi(unsigned int *Psi, size_t psiSize, unsigned int T, unsigned int nS) {
HuffmanCompressedPsiR3 huffmanCompressPsiR3(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS) {	
	HuffmanCompressedPsiR3 cPsi;

	printf("\n call to psiPlainRLE3 - compress Psi\n");

#ifdef 	R3H_WITHGAPS	
	cPsi.OFFSET = inioffset;
	//printf("\n **************** offset = %zu ***************************************",inioffset);
#endif	
	size_t maxval3R = 0;               //required to now the max-value that must be used in the samples.
	
	uint absolute_value;
	register size_t index;
	register size_t ptr, samplesPtr, samplePointersPtr;
	unsigned int runLenght, binaryLenght;
	
	ssize_t *diffs;	
	
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
	maxval3R -= cPsi.OFFSET;
#endif	
	sampleSize = bits(maxval3R);


fflush(stdout);fflush(stderr);
    printf("\n **************** maxVal3R = %zu , bits = %u *******",maxval3R,sampleSize);

// ********************************************************************************************
//@@fari!!!!xD!!!  Necesario sólo porque comprimimos el vocabulario del graph-index con esto: ver UNMAP[]
	uint sampleSize2= bits(Psi[psiSize-1]);	
#ifdef 	R3H_WITHGAPS	
	    sampleSize2= bits(Psi[psiSize-1]-cPsi.OFFSET);	 
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
			bitwrite(samples,samplesPtr,sampleSize, Psi[index]-cPsi.OFFSET);
			/**/uint tmp = bitread(samples,samplesPtr,sampleSize)+cPsi.OFFSET;			
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
	totalSize = ((size_t) sizeof(HuffmanCompressedPsiR3)) + 
		sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W) + 
		sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW) +
		sizeof(int)*((size_t)(streamSize+W-1)/W) ;
		
		fprintf(stderr,"\n@@@@@@@@@ psi samaplePeriod= %u, ns=%u ", T,nS);
		fprintf(stderr,"\n@@@@@@@@@ psi size= [samples = %lu] bytes ", sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [pointers = %lu] bytes ",sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW));
		fprintf(stderr,"\n@@@@@@@@@ psi size= [streamsize=%lu] bytes ", sizeof(int)*((size_t)(streamSize+W-1)/W));

	//@@printf("\n\t Compressed Psi size = %zu bytes, with %d different symbols.", totalSize, nS);
	
	
	// Asignamos os valores a cPsi e devolvemolo
	cPsi.T = T;
	cPsi.diffsHT = diffsHT;
	cPsi.nS = nS;
	cPsi.numberOfSamples = numberOfSamples;
	cPsi.samples = samples;
	cPsi.sampleSize = sampleSize;
	cPsi.samplePointers = samplePointers;
	cPsi.pointerSize = pointerSize;
	cPsi.stream = stream;
	cPsi.streamSize = streamSize;
	cPsi.totalMem = totalSize;
	cPsi.L = L;
	cPsi.LL= LL;
	cPsi.Lpsi=Lpsi;
	

/*
{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
	fprintf(stderr,"\n Test compress/uncompress PSI is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
	size_t i;
	uint val1,val2;	
	fflush(stdout);
	
	uint count=0;
	for (i=0; i<psiSize; i++) {
			val1= getHuffmanPsiR3Value(&cPsi, i);
			val2=Psi[i];
			if (val1 != val2) { count++;
					fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
					fprintf(stderr,"\n i=%zu,diffs[i] = %ld ",i, (long)diffs[i]); fflush(stdout);fflush(stderr);
					if (count > 100) {i= i/0; break;}
			}
	}
		fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
} /////////////////////////////////////////////////////////////////////////////////////
*/

	
	//frees resources not needed in advance
	free(diffs);
	
	//returns the data structure that holds the compressed psi.
	return cPsi;	
}






unsigned long  getHuffmanPsiR3SizeBitsUptoPosition(HuffmanCompressedPsiR3 *cPsi, size_t position) {
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	unsigned int runLenghtStart = cPsi->nS - 64 - cPsi->T;
	unsigned int negStart = cPsi->nS - 64;
	unsigned int bigStart = cPsi->nS - 32;	
	
	sampleIndex = position / cPsi->T;
	psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
		psiValue +=cPsi->OFFSET;
#endif				
	
	
	ptr = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;
		
	for(index=0;index<positionsSinceSample;index++) {
		uint bit1 = bitget(cPsi->stream,ptr); ptr++;
		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
			absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			uint bit2 = bitget(cPsi->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = cPsi->Lpsi;
				runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
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
	
	return (unsigned long) (ptr + sampleIndex * (cPsi->sampleSize + cPsi->pointerSize) );

}




unsigned int getHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t position) {
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	
	
	sampleIndex = position / cPsi->T;
	psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
	
	ptr = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;

	uint bit1,bit2;
	
	for(index=0;index<positionsSinceSample;index++) {
		bit1 = bitget(cPsi->stream,ptr); ptr++;
		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
			absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(cPsi->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = cPsi->Lpsi;
				runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
				if(index+runLenght>=positionsSinceSample)
					return psiValue+positionsSinceSample-index;
				else {
					psiValue += runLenght;
					index += runLenght-1;
				}
							
			}
			
		}


		
	//	if(huffmanCode < runLenghtStart) { 	// Incremento directo
	//		psiValue += huffmanCode;
	//	}	
	//	else 
	//		if(huffmanCode < negStart) {	// Estamos nun run
	//			runLenght = huffmanCode - runLenghtStart;
	//			if(index+runLenght>=positionsSinceSample)
	//				return psiValue+positionsSinceSample-index;
	//			else {
	//				psiValue += runLenght;
	//				index += runLenght-1;
	//			}
	//		}
	//		else
	//			if(huffmanCode < bigStart) {	// Negativo
	//				binaryLenght = huffmanCode-negStart+1;
	//				absolute_value = bitread(cPsi->stream,ptr,binaryLenght);
	//				ptr += binaryLenght;
	//				psiValue -= absolute_value;	
	//			}
	//			else {	// Grande
	//				binaryLenght = huffmanCode-bigStart+1;
	//				absolute_value = bitread(cPsi->stream,ptr,binaryLenght);
	//				ptr += binaryLenght;
	//				psiValue += absolute_value;				 
	//			}				
	}
	
	return psiValue;

}

void getHuffmanPsiR3ValueBuffer_1(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getHuffmanPsiR3Value(cPsi,i);
	return;
}


void getHuffmanPsiR3ValueBuffer(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end) {
//return 	getHuffmanPsiR3ValueBuffer_1 (cPsi,buffer,ini,end);
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / cPsi->T;
	psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
	
	ptr = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(cPsi->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
			absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(cPsi->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = cPsi->Lpsi;
				runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
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
	
	size_t cpsiT = cPsi->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
		}
		else {
		
			bit1 = bitget(cPsi->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(cPsi->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
					absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = cPsi->Lpsi;
					runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
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
int getfindHuffmanPsiR3ValueBuffer(HuffmanCompressedPsiR3 *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / cPsi->T;
	psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
	
	ptr = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(cPsi->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
			absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(cPsi->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = cPsi->Lpsi;
				runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
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
	
	size_t cpsiT = cPsi->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
		}
		else {
		
			bit1 = bitget(cPsi->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(cPsi->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
					absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = cPsi->Lpsi;
					runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
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
int getfindHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
		
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / cPsi->T;
	psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
	
	ptr = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(cPsi->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
			absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(cPsi->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = cPsi->Lpsi;
				runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
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
	
	size_t cpsiT = cPsi->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
		}
		else {
		
			bit1 = bitget(cPsi->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(cPsi->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
					absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = cPsi->Lpsi;
					runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
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

ulong getfindLeftOnlyHuffmanPsiR3Value(HuffmanCompressedPsiR3 *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
		
	register size_t index;
	size_t sampleIndex, positionsSinceSample, ptr;

	unsigned int psiValue, absolute_value, huffmanCode; 
	unsigned int binaryLenght, runLenght;
	

	size_t position = ini;
	sampleIndex = position / cPsi->T;
	psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
	
	ptr = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;

	uint bit1,bit2;

	
	int inRun=0;//@@
	int inRunRemain=0;//@@
	
	for(index=0;index<positionsSinceSample;index++) {

		bit1 = bitget(cPsi->stream,ptr); ptr++;		
		if (bit1 ==0) {          /*  "0": encoding a positive value */
			binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
			absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
			psiValue += absolute_value;				 	
		}
		else { // "1"
			bit2 = bitget(cPsi->stream,ptr); ptr++;

			if (bit2 == 0) {    /* "10": encoding a negative value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue -= absolute_value;				 					
			}
			else {				/* "11": encoding a run */				
				binaryLenght = cPsi->Lpsi;
				runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
				
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
	
	size_t cpsiT = cPsi->T;	
	for (position = ini+1; position <= end ;  ) {
		if (!(position%cpsiT)){ // a sampled value
			sampleIndex = position / cpsiT;
			psiValue = bitread(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
#ifdef 	R3H_WITHGAPS	
			psiValue +=cPsi->OFFSET;
#endif				
		}
		else {
		
			bit1 = bitget(cPsi->stream,ptr); ptr++;		
			if (bit1 ==0) {          /*  "0": encoding a positive value */
				binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL);     ptr += cPsi->LL;
				absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
				psiValue += absolute_value;				 	
			}
			else { // "1"
				bit2 = bitget(cPsi->stream,ptr); ptr++;

				if (bit2 == 0) {    /* "10": encoding a negative value */
					binaryLenght  = bitread(cPsi->stream,ptr,cPsi->LL); 	ptr += cPsi->LL;
					absolute_value= bitread(cPsi->stream,ptr,binaryLenght); ptr += binaryLenght;
					psiValue -= absolute_value;				 					
				}
				else {				/* "11": encoding a run */				
					binaryLenght = cPsi->Lpsi;
					runLenght    = bitread(cPsi->stream,ptr,binaryLenght); 	ptr += binaryLenght;	
					
					
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











void storeHuffmanCompressedPsiR3(HuffmanCompressedPsiR3 *compressedPsi, char *filename) {

	FILE *file;
	THuff H;

	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	size_t write_err;
	
	write_err=fwrite(&(compressedPsi->T), sizeof(int),1,file);

#ifdef 	R3H_WITHGAPS
    //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
	write_err=fwrite(&(compressedPsi->OFFSET), sizeof(size_t),1,file);
	//printf("\n **************** saved offset = %zu ***************************************",compressedPsi->OFFSET);
	fflush(stderr);fflush(stdout);
	
#endif
	
//	// Almacenar o arbol de huffman
//	H = compressedPsi->diffsHT;
//	write_err=fwrite(  &H.max, sizeof(int),1,file);
//	write_err=fwrite(  &H.lim, sizeof(int),1,file);
//	write_err=fwrite(  &H.depth, sizeof(int),1,file);
//	//	write( file, H.s.spos, (H.lim+1)*sizeof(int));
//	write_err=fwrite(  H.s.symb,sizeof(int), (H.lim+1),file);	
//	write_err=fwrite(  H.num,sizeof(int), (H.depth+1),file);
//	write_err=fwrite(  H.fst,sizeof(int), (H.depth+1),file);
//	// Fin de almacenar o arbol de huffman
	
	write_err=fwrite(  &(compressedPsi->nS), sizeof(int),1,file);
	write_err=fwrite(  &(compressedPsi->numberOfSamples), sizeof(size_t),1,file);
	write_err=fwrite(  &(compressedPsi->sampleSize), sizeof(int),1,file);
	write_err=fwrite( 	compressedPsi->samples, sizeof(int), (((size_t)compressedPsi->numberOfSamples*compressedPsi->sampleSize+W-1)/W), file);	
	write_err=fwrite(  &(compressedPsi->pointerSize), sizeof(int),1,file);
	write_err=fwrite( 	compressedPsi->samplePointers, sizeof(size_t), (((size_t)compressedPsi->numberOfSamples*compressedPsi->pointerSize+WW-1)/WW), file);
	write_err=fwrite(  &(compressedPsi->streamSize), sizeof(size_t),1,file);
	write_err=fwrite( 	compressedPsi->stream, sizeof(int),((compressedPsi->streamSize+W-1)/W) , file);
	size_t byteswritentostream = write_err;
	write_err=fwrite(  &(compressedPsi->totalMem), sizeof(size_t),1,file);


	write_err=fwrite(  &(compressedPsi->L), sizeof(int),1,file);
	write_err=fwrite(  &(compressedPsi->LL), sizeof(int),1,file);
	write_err=fwrite(  &(compressedPsi->Lpsi), sizeof(int),1,file);


/*

	printf("\n\n to save");
	printf("\n compressedPSI->NS = %u",compressedPsi->nS);
	printf("\n numberofsamples = %zu",compressedPsi->numberOfSamples);
	printf("\n samplesize_Bits = %u bits",compressedPsi->sampleSize);
	printf("\n tamaño de array samples (bytes) = %zu", (((size_t)compressedPsi->numberOfSamples*compressedPsi->sampleSize+W-1)/W)*sizeof(int));	
	printf("\n pointersize_Bits = %u bits",compressedPsi->pointerSize);
	printf("\n tamaño de array pointers = %zu (bytes)", (( (size_t)compressedPsi->numberOfSamples*compressedPsi->pointerSize+WW-1)/WW)*sizeof(size_t) );
	printf("\n streamsize (lonxitude en bits) = %zu",compressedPsi->streamSize);
	printf("\n stream (tamaño en bytes) = %zu",	((compressedPsi->streamSize+W-1)/W)*sizeof(int));
	printf("\n      bytes writen for **stream = %zu**",	sizeof(uint)*byteswritentostream);
	
	printf("\n totalMem compressed psi = %zu bytes", compressedPsi->totalMem);
	fflush(stdout);fflush(stderr);
*/
	
	fclose(file);	

}



HuffmanCompressedPsiR3 loadHuffmanCompressedPsiR3(char *filename) {
	
	HuffmanCompressedPsiR3 compressedPsi;

 	THuff H;
     
	FILE *file;

	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}
		fflush(stdout);fflush(stderr);
	
	size_t read_err;
	
	read_err=fread(&(compressedPsi.T), sizeof(int), 1, file);

#ifdef 	R3H_WITHGAPS
    //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
	read_err=fread(&(compressedPsi.OFFSET), sizeof(size_t),1,file);
	//printf("\n **************** loaded offset = %zu ***************************************",compressedPsi.OFFSET);
	//fflush(stderr);fflush(stdout);
	
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
//	compressedPsi.diffsHT = H;
//	// Fin da carga do arbol de Huffman
	read_err=fread( &(compressedPsi.nS), sizeof(int), 1,file);
	read_err=fread( &(compressedPsi.numberOfSamples), sizeof(size_t), 1, file);
	
	read_err=fread( &(compressedPsi.sampleSize), sizeof(int), 1,file);
	compressedPsi.samples = (unsigned int *)malloc((( (size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+W-1)/W)*sizeof(int));
	read_err=fread( compressedPsi.samples, sizeof(int), (( (size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+W-1)/W) , file );
	
	read_err=fread( &(compressedPsi.pointerSize), sizeof(int),1,file);
	compressedPsi.samplePointers = (size_t *)malloc((( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW)*sizeof(size_t));
	read_err=fread( compressedPsi.samplePointers, sizeof(size_t), (( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW), file);
	
	read_err=fread( &(compressedPsi.streamSize), sizeof(size_t),1,file);
	compressedPsi.stream = (unsigned int *)malloc(((compressedPsi.streamSize+W-1)/W)*sizeof(int));
	size_t readbytesstream=fread( compressedPsi.stream, sizeof(uint), ((compressedPsi.streamSize+W-1)/W), file);
	read_err=fread( &(compressedPsi.totalMem), sizeof(size_t),1,file);

	read_err=fread( &(compressedPsi.L), sizeof(int), 1,file);
	read_err=fread( &(compressedPsi.LL), sizeof(int), 1,file);
	read_err=fread( &(compressedPsi.Lpsi), sizeof(int), 1,file);
	
	
/*

	printf("\n\n to load");
	printf("\n compressedPSI->NS = %u",compressedPsi.nS);
	printf("\n numberofsamples = %zu",compressedPsi.numberOfSamples);
	printf("\n samplesize_Bits = %u bits",compressedPsi.sampleSize);
	printf("\n tamaño de array samples (bytes) = %zu", (((size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+W-1)/W)*sizeof(int));	
	printf("\n pointersize_Bits = %u bits",compressedPsi.pointerSize);
	printf("\n tamaño de array pointers = %zu (bytes)", (( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW)*sizeof(size_t) );
	printf("\n streamsize (lonxitude en bits) = %zu",compressedPsi.streamSize);
	printf("\n stream (tamaño en bytes) = %zu",	((compressedPsi.streamSize+W-1)/W)*sizeof(int));
	printf("\n      bytes read for **stream = %zu**",	sizeof(uint)*readbytesstream);
	                                            
	printf("\n totalMem compressed psi = %zu bytes", compressedPsi.totalMem);
	fflush(stdout);fflush(stderr);
*/	

	fclose(file);			
	return compressedPsi;

}



/*

void storeHuffmanCompressedPsi(HuffmanCompressedPsi *compressedPsi, char *filename) {

	int file;
	THuff H;

	if( (file = open(filename, O_WRONLY|O_CREAT, 0700)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	ssize_t write_err;
	
	write_err=write(file, &(compressedPsi->T), sizeof(int));
	// Almacenar o arbol de huffman
	H = compressedPsi->diffsHT;
	write_err=write(file, &H.max, sizeof(int));
	write_err=write(file, &H.lim, sizeof(int));
	write_err=write(file, &H.depth, sizeof(int));
//	write(file, H.s.spos, (H.lim+1)*sizeof(int));
	write_err=write(file, H.s.symb, (H.lim+1)*sizeof(int));	
	write_err=write(file, H.num, (H.depth+1)*sizeof(int));
	write_err=write(file, H.fst, (H.depth+1)*sizeof(int));
	// Fin de almacenar o arbol de huffman
	write_err=write(file, &(compressedPsi->nS), sizeof(int));
	write_err=write(file, &(compressedPsi->numberOfSamples), sizeof(size_t));
	write_err=write(file, &(compressedPsi->sampleSize), sizeof(int));
	write_err=write(file,	compressedPsi->samples, (((size_t)compressedPsi->numberOfSamples*compressedPsi->sampleSize+W-1)/W)*sizeof(int));	
	write_err=write(file, &(compressedPsi->pointerSize), sizeof(int));
	write_err=write(file,	compressedPsi->samplePointers, (((size_t)compressedPsi->numberOfSamples*compressedPsi->pointerSize+WW-1)/WW)*sizeof(size_t));
	write_err=write(file, &(compressedPsi->streamSize), sizeof(size_t));
	write_err=write(file,	compressedPsi->stream, ((compressedPsi->streamSize+W-1)/W)*sizeof(int));
	size_t byteswritentostream = write_err;
	write_err=write(file, &(compressedPsi->totalMem), sizeof(size_t));



	printf("\n\n to save");
	printf("\n compressedPSI->NS = %u",compressedPsi->nS);
	printf("\n numberofsamples = %zu",compressedPsi->numberOfSamples);
	printf("\n samplesize_Bits = %u bits",compressedPsi->sampleSize);
	printf("\n tamaño de array samples (bytes) = %zu", (((size_t)compressedPsi->numberOfSamples*compressedPsi->sampleSize+W-1)/W)*sizeof(int));	
	printf("\n pointersize_Bits = %u bits",compressedPsi->pointerSize);
	printf("\n tamaño de array pointers = %zu (bytes)", (( (size_t)compressedPsi->numberOfSamples*compressedPsi->pointerSize+WW-1)/WW)*sizeof(size_t) );
	printf("\n streamsize (lonxitude en bits) = %zu",compressedPsi->streamSize);
	printf("\n stream (tamaño en bytes) = %zu",	((compressedPsi->streamSize+W-1)/W)*sizeof(int));
	printf("\n      bytes writen for **stream = %zu**",	byteswritentostream);
	
	printf("\n totalMem compressed psi = %zu bytes", compressedPsi->totalMem);
	fflush(stdout);fflush(stderr);

	
	close(file);	

}



HuffmanCompressedPsi loadHuffmanCompressedPsi(char *filename) {
	
	HuffmanCompressedPsi compressedPsi;

 	THuff H;
     
	int file;

	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}
		fflush(stdout);fflush(stderr);
	
	ssize_t read_err;
	
	read_err=read(file, &(compressedPsi.T), sizeof(int));
	// Cargamos o arbol de Huffman
	read_err=read(file, &H.max, sizeof(int));
	read_err=read(file, &H.lim, sizeof(int));
	read_err=read(file, &H.depth, sizeof(int));
	//H.s.spos = (unsigned int *) malloc((H.lim+1)*sizeof(int));
	//H.s.spos =H.s.symb = (unsigned int *) malloc((H.lim+1)*sizeof(int));
	H.s.symb = (unsigned int *) malloc((H.lim+1)*sizeof(int));
	H.num = (unsigned int *) malloc((H.depth+1)*sizeof(int));	
	H.fst = (unsigned int *) malloc((H.depth+1)*sizeof(int));	

	//read(file, H.s.spos, (H.lim+1)*sizeof(int));
	//fprintf(stderr," \n read %d spos bytes\n", (H.lim+1)*sizeof(int));
	read_err=read(file, H.s.symb, (H.lim+1)*sizeof(int));	

	read_err=read(file, H.num, (H.depth+1)*sizeof(int));
	read_err=read(file, H.fst, (H.depth+1)*sizeof(int));	
	compressedPsi.diffsHT = H;
	// Fin da carga do arbol de Huffman
	read_err=read(file, &(compressedPsi.nS), sizeof(int));
	read_err=read(file, &(compressedPsi.numberOfSamples), sizeof(size_t));	
	
	read_err=read(file, &(compressedPsi.sampleSize), sizeof(int));
	compressedPsi.samples = (unsigned int *)malloc((( (size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+W-1)/W)*sizeof(int));
	read_err=read(file, compressedPsi.samples, (( (size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+W-1)/W)*sizeof(int));
	
	read_err=read(file, &(compressedPsi.pointerSize), sizeof(int));
	compressedPsi.samplePointers = (size_t *)malloc((( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW)*sizeof(size_t));
	read_err=read(file, compressedPsi.samplePointers, (( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW)*sizeof(size_t));
	
	read_err=read(file, &(compressedPsi.streamSize), sizeof(size_t));
	compressedPsi.stream = (unsigned int *)malloc(((compressedPsi.streamSize+W-1)/W)*sizeof(int));
	size_t readbytesstream=read(file, compressedPsi.stream, ((compressedPsi.streamSize+W-1)/W)*sizeof(int));
	read_err=read(file, &(compressedPsi.totalMem), sizeof(size_t));
	
	close(file);
	


	printf("\n\n to load");
	printf("\n compressedPSI->NS = %u",compressedPsi.nS);
	printf("\n numberofsamples = %zu",compressedPsi.numberOfSamples);
	printf("\n samplesize_Bits = %u bits",compressedPsi.sampleSize);
	printf("\n tamaño de array samples (bytes) = %zu", (((size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+W-1)/W)*sizeof(int));	
	printf("\n pointersize_Bits = %u bits",compressedPsi.pointerSize);
	printf("\n tamaño de array pointers = %zu (bytes)", (( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW)*sizeof(size_t) );
	printf("\n streamsize (lonxitude en bits) = %zu",compressedPsi.streamSize);
	printf("\n stream (tamaño en bytes) = %zu",	((compressedPsi.streamSize+W-1)/W)*sizeof(int));
	printf("\n      bytes read for **stream = %zu**",	readbytesstream);
	                                            
	printf("\n totalMem compressed psi = %zu bytes", compressedPsi.totalMem);
	fflush(stdout);fflush(stderr);
	
			
	return compressedPsi;

}
*/
