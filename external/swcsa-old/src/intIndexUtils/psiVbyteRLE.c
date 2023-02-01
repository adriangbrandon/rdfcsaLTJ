

/* psiVbyteRLE.c
 * Copyright (C) 2011, Antonio Fariña and Eduardo Rodriguez, all rights reserved.
 * Improved representation based on a previous version from Gonzalo Navarro's.
 *
 * psiVbyteRLE.c: Compressed Representation for Psi in CSA.
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

#include "psiVbyteRLE.h"

// IMPLEMENTACION DAS FUNCIONS

void destroyVbyteCompressedPsi(VbyteCompressedPsi *compressedPsi) {
	free(compressedPsi->samples);
	free(compressedPsi->samplePointers);
	free(compressedPsi->bc);
}





VbyteCompressedPsi vbyteCompressPsi(unsigned int *Psi, size_t psiSize, unsigned int T) { //, unsigned int nS);
//VbyteCompressedPsi vbyteCompressPsi(unsigned int *Psi, size_t psiSize, uint vocSize,  bitmap bD, unsigned int T){
	
	VbyteCompressedPsi cPsi;
	
	printf("\n psiSize= %zu, t_PSI %u\n",psiSize,T);
	uint maxrun = T;
	size_t negoffset = maxrun+psiSize+1;

	// Estructuras da funcion comprimida (para logo asignar)
	// TamEn se podian almacenar directamente
	size_t numberOfSamples;
	size_t *samples;

	uint sampleSize;
	size_t *samplePointers;
	
	unsigned int pointerSize;
	//unsigned int *stream;
	size_t streamSize;
	byte * bc;
	size_t bcsize;
	// Para estadistica 
	size_t totalSize;
	
	
	// Inicializamos diferencias	
	ssize_t *diffs = NULL;
	diffs =  (ssize_t *) malloc (sizeof(ssize_t) * psiSize);
	if (!diffs) {
		printf("\n unable to allocate %zu bytes\n",sizeof(ssize_t)*psiSize);
		exit(0);
	}
	diffs[0] = 0;
	size_t index;
	for(index=1; index<psiSize; index++) 
		diffs[index] = ((ssize_t)Psi[index]) - ((ssize_t)Psi[index-1]);
		
	////////////////
	size_t runLenght = 0;
	size_t absolute_value;
	//size_t numbccodes=0;  //for current symbol
	size_t tmpsize; 
	//size_t end;

	bcsize=0;
	// Calculamos canto ocupará a secuencia comprimida con Bytecodes.
	runLenght = 0;
	for(index=0; index<psiSize; index++) {
		if(index%T) {
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght++;
			} else {	// Non estamos nun run
				if(runLenght) {
					//huffmanDst[runLenght+runLenghtStart]++;
					//runLenght = 0;
					absolute_value = runLenght;
					SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
					bcsize += tmpsize;
					runLenght = 0;					
				}
				//if(diffs[index]>((ssize_t)1) && diffs[index]<runLenghtStart) 
				//	huffmanDst[diffs[index]]++;
				//else
					if(diffs[index]< ((ssize_t) 0) ) {	// Valor negativo
						absolute_value = (size_t) (-diffs[index]);
						absolute_value += negoffset;
						SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
						bcsize += tmpsize;						
					} else {				// Valor grande >= 128
						absolute_value = (uint)(diffs[index]);
						absolute_value += maxrun;
						SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
						bcsize += tmpsize;						
					}
			}
			
		} else { // Rompemos o run porque atopamos unha mostra
			if(runLenght) {
				absolute_value = runLenght;
				SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
				bcsize += tmpsize;
				runLenght = 0;
			}
		}
		
	}
		
	if(runLenght) {
		absolute_value = runLenght;
		SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
		bcsize += tmpsize;
		runLenght = 0;
	}
	

	streamSize=bcsize;

	// Calculamos o numero de mostras e o espacio ocupado por cada mostra e por cada punteiro
	numberOfSamples = (psiSize+T-1)/T;	
	sampleSize = bits(psiSize);	
		// ********************************************************************************************
		//@@fari!!!!xD!!!  Necesario sólo porque comprimimos el vocabulario del graph-index con esto: ver UNMAP[]
			uint sampleSize2 = bits(Psi[psiSize-1]);	
			if (sampleSize2>sampleSize) sampleSize=sampleSize2;
		// ********************************************************************************************

	pointerSize = bits(streamSize);	
	fprintf(stderr,"\n psi: pointersize = %u bits, sampleSize=%u bits", pointerSize,sampleSize);
	fprintf(stderr,"\n psi: bytecodesSize = %zu bytes", bcsize);
	
	bc = (unsigned char *) malloc (sizeof(unsigned char) * bcsize);
	

	fprintf(stderr,"\n espacio para Sample-values-psi = %lu bytes"      , sizeof(size_t)* (ulong_len(sampleSize ,numberOfSamples)) );
	fprintf(stderr,"\n espacio para Sample-ptrs-2-bcs-psi** = %lu bytes", sizeof(size_t)* (ulong_len(pointerSize,numberOfSamples)) );

	// Reservamos espacio para a secuencia e para as mostras e punteiros
	samples = (size_t *)malloc(sizeof(size_t)* (ulong_len(sampleSize ,numberOfSamples)));	
	samples[( (ulong_len(sampleSize ,numberOfSamples)) )-1] =0000; //initialized only to avoid valgrind warnings
		
	fprintf(stderr,"\n espacio para Sample-pointers-psi = %lu bytes", sizeof(size_t)* (ulong_len(pointerSize,numberOfSamples)) );
		
	samplePointers = (size_t *)malloc(sizeof(size_t)* (ulong_len(pointerSize,numberOfSamples)) );
	samplePointers[ (ulong_len(pointerSize,numberOfSamples)) -1] = 0L;  //initialized only to avoid valgrind warnings

	fflush(stderr);fflush(stdout);





	// Comprimimos secuencialmente (habera que levar un punteiro desde o inicio)
	size_t samplesPtr = 0;
	size_t samplePointersPtr = 0;
	size_t bcpos=0;
	runLenght = 0;
	
	for(index=0; index<psiSize; index++) {
		if(index%T) {
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght++;
			} else {	// Non estamos nun run
				if(runLenght) {
					DEC_TO_BC(bc, bcpos, runLenght)	//encode runLenght
					runLenght = 0;					
				}
				//if(diffs[index]>((ssize_t)1) && diffs[index]<runLenghtStart) 
				//	huffmanDst[diffs[index]]++;
				//else
					if(diffs[index]< ((ssize_t) 0) ) {	// Valor negativo
						absolute_value = (size_t) (-diffs[index]);
						absolute_value += negoffset;
						DEC_TO_BC(bc, bcpos, absolute_value);
						
					} else {				           // Valor positiovo
						absolute_value = (uint)(diffs[index]);
						absolute_value += maxrun;
						DEC_TO_BC(bc, bcpos, absolute_value);					
					}
			}
			
		} else { // Rompemos o run porque atopamos unha mostra
			if(runLenght) {
				absolute_value = runLenght;
				DEC_TO_BC(bc, bcpos, runLenght)	//encode runLenght
				runLenght = 0;	
			}


			bitwrite64(samples,samplesPtr,sampleSize, Psi[index]);
			/**/size_t tmp = bitread64(samples,samplesPtr,sampleSize);			
			/**/assert (tmp == Psi[index]);		
						
			samplesPtr += sampleSize;
			
			bitwrite64(samplePointers,samplePointersPtr,pointerSize,bcpos);
			/**/size_t tmp2 = bitread64(samplePointers,samplePointersPtr,pointerSize);
			/**/assert (tmp2 == bcpos);					
			samplePointersPtr += pointerSize;
			
		}		
	}
		
	if(runLenght) {
		absolute_value = runLenght;
		DEC_TO_BC(bc, bcpos, runLenght)	//encode runLenght
		runLenght = 0;	
	}



    //// COMPRESSION ENDS ////////////


	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////	
	
// Amosamos o espacio ocupado
  totalSize = ((size_t) sizeof(VbyteCompressedPsi)) + 
  	sizeof(size_t)*((size_t)(numberOfSamples*sampleSize+WW-1)/WW) +
  	sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW) +
  	sizeof(char)*(bcsize);
  	
  	fprintf(stderr,"\n@@@@@@@@@ psi **bcpos=%zu, bcsize= %zu ", bcpos, bcsize);
  	fprintf(stderr,"\n@@@@@@@@@ psi samplePeriod= %u ", T);
  	fprintf(stderr,"\n@@@@@@@@@ psi size= [samples = %zu] bytes ", sizeof(size_t)*((size_t)(numberOfSamples*sampleSize +WW-1)/WW) );
  	fprintf(stderr,"\n@@@@@@@@@ psi size= [pointers = %zu] bytes ",sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW) );
  	fprintf(stderr,"\n@@@@@@@@@ psi size= [bcs = %zu] bytes ",     bcsize);
	printf("\n\t -------------------");
  	fprintf(stderr,"\n@@@@@@@@@ psi TOTAL= %zu bytes ",     totalSize);
	printf("\n\t -------------------");
  	
	fflush(stdout);fflush(stderr);

	
	// Asignamos os valores a cPsi e devolvemolo
	cPsi.T = T;
	cPsi.psiSize = psiSize;
	cPsi.maxrun = maxrun;
	cPsi.negoffset = negoffset;


	cPsi.numberOfSamples = numberOfSamples;
	cPsi.samples = samples;
	cPsi.sampleSize = sampleSize;
	cPsi.samplePointers = samplePointers;
	cPsi.pointerSize = pointerSize;

	
	cPsi.bc = bc;
	cPsi.bcsize = bcsize;
	cPsi.bitsbcsize = bits(bcsize);   
	cPsi.bitspsiSize = bits(psiSize);
	
	cPsi.totalMem = totalSize;
		

	//frees resources not needed in advance
	if (diffs)
		free(diffs);
	
	//returns the data structure that holds the compressed psi.
	return cPsi;	
}






/*
VbyteCompressedPsi vbyteCompressPsi((unsigned int *Psi, size_t psiSize, unsigned int T);//, unsigned int nS);
//VbyteCompressedPsi vbyteCompressPsi(unsigned int *Psi, size_t psiSize, uint vocSize,  bitmap bD, unsigned int T){
	
	VbyteCompressedPsi cPsi;
	
	printf("\n psiSize= %zu, vocsize = %u\n",psiSize,vocSize);
	
	uint *D= bD->data;   //comes from icsa, not allocated here.
		
	size_t *s0   = (size_t *) malloc (sizeof(size_t) *(vocSize+0));	// Punteiros das mostras a stream
	size_t *ptr0 = (size_t *) malloc (sizeof(size_t) *(vocSize+0));	
	size_t *off0 = (size_t *) malloc (sizeof(size_t) *(vocSize+0));
	size_t l1samples = vocSize+0;
	
	uint *D1; bitmap bD1;
	size_t *s1;	// Punteiros das mostras a stream
	size_t *ptr1;
	size_t *off1;
	
	byte *bc; size_t bcsize=0;
	size_t l2samples=0;	
	
	// Para estadistica 
	size_t totalSize;
	
	
	// Inicializamos diferencias	
	ssize_t *diffs = NULL;
	diffs =  (ssize_t *) malloc (sizeof(ssize_t) * psiSize);
	if (!diffs) {
		printf("\n unable to allocate %zu bytes\n",sizeof(ssize_t)*psiSize);
		exit(0);
	}

	
	diffs[0] = 0;
	size_t index;
	for(index=1; index<psiSize; index++) 
		diffs[index] = ((ssize_t)Psi[index]) - ((ssize_t)Psi[index-1]);
		
	////////////////
	size_t c=0;
	size_t runLenght = 0;
	size_t absolute_value;
	size_t numbccodes=0;  //for current symbol
	size_t tmpsize; 
	size_t end;
	
	for (c=0;c<vocSize;c++) {

		index= select_1(bD,c+1);	
		numbccodes =0;
		end= select_1(bD,c+2)-1;

		index ++;
					
		while (index <= end) {  //for each symbol
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght=1;index++;
				while((diffs[index]== ((ssize_t) 1))  && (index <=end))   {
					index++;
					runLenght++;
				}
				bcsize ++;  //encode [+0]
				SIZE_DEC_TO_BC(runLenght,tmpsize);   //encode runLenght
				bcsize += tmpsize;  
				numbccodes +=2;				
			}
			else {
				size_t absolute_value = (size_t)(diffs[index]);
				SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
				bcsize += tmpsize;
				numbccodes ++;
				index++;
			}
			
			if (numbccodes>T) {
				numbccodes=0;
				//size_t absolute_value = (size_t) Psi[index];
				//SIZE_DEC_TO_BC(absolute_value,tmpsize);   //encode absolute_value   
				//bcsize += tmpsize;
				l2samples ++;  //increses second level sample counter
				//index++;
			}			
		}
		
	}
	

	// Calculamos o espacio total ocupado pola secuencia BC + RLE
	fprintf(stderr,"\n\t\t *[3] bcsize = %zu bytes ", bcsize);
	fprintf(stderr,"\n\t\t *[3] level1 samples = %zu entries ", l1samples);
	fprintf(stderr,"\n\t\t *[3] level2 samples = %zu entries ", l2samples);

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////	
	s1   = (size_t *) malloc (sizeof(size_t) *(l2samples));	// Level 2 sampling
	ptr1 = (size_t *) malloc (sizeof(size_t) *(l2samples));
	off1 = (size_t *) malloc (sizeof(size_t) *(l2samples));
	
	bc = (unsigned char *) malloc (sizeof(unsigned char) * bcsize);

	// Contruimos D1
	D1 = (uint*) malloc (sizeof(uint) * ((psiSize+W-1)/W));		
	for(index=0;index<((psiSize+W-1)/W);index++) D1[index] = 0;	
	
	//bitset(D1,4);
	//bD1 = createBitmap(D1,psiSize);

	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////	
			
	size_t bcpos = 0;
	size_t l2idx = 0;
	size_t numtmp;
	c=0;
	runLenght=0;
	numbccodes=0; //for current symbol

	
	for (c=0;c<vocSize;c++) {
		index= select_1(bD,c+1);

//		if (c==vocSize)
//			index = psiSize-1;
//		else 
//			index= select_1(bD,c+1);		
		
		//store sampling for symbol c here
		s0[c]   = Psi[index];
		ptr0[c] = bcpos;
		off0[c] = index;
		numbccodes=0;			
		index ++;

		// now process the rest of entries in SA[index ..] for symbol c.
		numbccodes =0;
		if (c==vocSize) 
			end=psiSize-1;
		else
			end= select_1(bD,c+2)-1;
		
		while (index <= end) {  //for each symbol
			if(diffs[index]== ((ssize_t) 1) ) {
				runLenght=1;index++;
				while((diffs[index]== ((ssize_t) 1))  && (index <=end))   {
					index++;
					runLenght++;
				}
				numtmp=0; DEC_TO_BC(bc, bcpos, numtmp)	//encode [+0]
				DEC_TO_BC(bc, bcpos, runLenght)	//encode runLenght
				numbccodes +=2;
			}
			else {
				// positive gap value (>1)
				numtmp=diffs[index]; DEC_TO_BC(bc, bcpos, numtmp)  //encode [value]
				numbccodes ++;
				index++;
			}
			
			if (numbccodes>T) {   //sets L2 sample  (index is not increased!)
				s1[l2idx]   = Psi[index-1];
				ptr1[l2idx] = bcpos;
				off1[l2idx] = index-1;
				bitset(D1,index-1);
				l2idx++;                //increses second level sample counter
				numbccodes=0;			
			}			
		}
	
	}
	
//	s0[vocSize]   = Psi[psiSize-1];   //@@ not sure this is needed (actually not)
//	ptr0[c] = bcpos;                  //@@ not sure this is needed (actually not)   
//	off0[c] = psiSize-1;              //@@ not sure this is needed (actually not)
		
	bD1 = createBitmap(D1,psiSize);
		
	
	/////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////	
	
// Amosamos o espacio ocupado
//  totalSize = ((size_t) sizeof(HuffmanCompressedPsi)) + 
//  	sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W) + 
//  	sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW) +
//  	sizeof(int)*((size_t)(streamSize+W-1)/W) + 
//  	sizeHuff(diffsHT);
//  	
//  	fprintf(stderr,"\n@@@@@@@@@ psi samaplePeriod= %u, ns=%u ", T,nS);
//  	fprintf(stderr,"\n@@@@@@@@@ psi size= [samples = %lu] bytes ", sizeof(int)*((size_t)(numberOfSamples*sampleSize+W-1)/W));
//  	fprintf(stderr,"\n@@@@@@@@@ psi size= [pointers = %lu] bytes ",sizeof(size_t)*((size_t)(numberOfSamples*pointerSize+WW-1)/WW));
//  	fprintf(stderr,"\n@@@@@@@@@ psi size= [totalsize diffsHt.total  = %lu] bits \n",diffsHT.total);		
//  	fprintf(stderr,"\n@@@@@@@@@ psi size= [streamsize+largevalues =%lu] bytes ", sizeof(int)*((size_t)(streamSize+W-1)/W));
//  	fprintf(stderr,"\n@@@@@@@@@ psi size= [sizeHuff tree = %lu] bytes ", (ulong)sizeHuff(diffsHT));		
//  	
//  	fflush(stdout);fflush(stderr);

	size_t totalL1=0, totalL2=0, d1size=0 ;

//	totalL1 =  2  * (l1samples) * sizeof(uint) + 
//		#ifndef USE_SELECT1	   
//			   1 * (l1samples) * sizeof(uint)+ 
//		#endif


	totalL1 =  (1 * sizeof(size_t)*((size_t)(l1samples * bits(psiSize)+WW-1)/WW)) + 
		       (1 * sizeof(size_t)*((size_t)(l1samples * bits(bcsize)+WW-1)/WW)) +
		//#ifndef USE_SELECT1	   
			   (1 * sizeof(size_t)*((size_t)(l1samples * bits(psiSize)+WW-1)/WW)) + 
		//#endif
		       0;


	totalL2 =  (1 * sizeof(size_t)*((size_t)(l2samples * bits(psiSize)+WW-1)/WW)) + 
		       (1 * sizeof(size_t)*((size_t)(l2samples * bits(bcsize)+WW-1)/WW)) +
		//#ifndef USE_SELECT1	   
			   (1 * sizeof(size_t)*((size_t)(l2samples * bits(psiSize)+WW-1)/WW)) + 
		//#endif
		       0;
	
	d1size =   sizeof(uint) * ((psiSize+W-1)/W)  ;
	
	totalSize= bcsize + 
			   totalL1 +
			   totalL2 + 
			   //(sizeof(uint) * ((psiSize+31)/32)) + //D    //INCLUDED BY THE iCSA data structure !!
	           //bD->mem_usage +
			   d1size + //D1
	           bD1->mem_usage;

  	fprintf(stderr,"\n@@@@@@@@@ psi samaplePeriod= %u ", T);
  	fprintf(stderr,"\n@@@@@@@@@ psi size=  %zu entries  --> ptrsize = %u bits", psiSize , bits(psiSize) );  	
  	fprintf(stderr,"\n@@@@@@@@@ psi size=  %lu bytes ", totalSize  );  	
  	
  	fprintf(stderr,"\n@@@@@@@@@ psi size= [samples L0= %lu entries] :: %lu bytes ", l1samples,totalL1  );
  	fprintf(stderr,"\n@@@@@@@@@ psi size= [samples L1= %lu entries] :: %lu bytes ", l2samples,totalL2  );
  	fprintf(stderr,"\n@@@@@@@@@ psi size=   D1 = %lu bytes, bD1 = %u bytes ", d1size, bD1->mem_usage);
  	fprintf(stderr,"\n@@@@@@@@@ psi size=  Bytecodes = %lu bytes  (bitsbcsize = %u bits) ", bcsize , bits(bcsize) );  	

	
	
	size_t substract_size = 0 +
			(1 * sizeof(size_t)*((size_t)(l1samples * bits(psiSize)+WW-1)/WW)) +
			(1 * sizeof(size_t)*((size_t)(l2samples * bits(psiSize)+WW-1)/WW)) ;

	printf("\n\t -------------------");
	printf("\n\t Compressed Psi size (If select were used skip off0-off1) = %zu bytes", totalSize - substract_size);
	printf("\n\t Compressed Psi size = %zu bytes", totalSize);
	printf("\n\t -------------------");
	fflush(stdout);fflush(stderr);

	//@@printf("\n\t Compressed Psi size = %zu bytes, with %d different symbols.", totalSize, nS);
	
	// Asignamos os valores a cPsi e devolvemolo
	cPsi.T = T;
	cPsi.D = D;
	cPsi.bD = bD;
	cPsi.s0 = s0; 
	cPsi.ptr0 = ptr0;
	//#ifndef USE_SELECT1	
	cPsi.off0 = off0;
	//#else
	//free(off0);
	//#endif
	
    cPsi.l1samples = l1samples;
    cPsi.l2samples = l2samples;
	cPsi.D1 = D1;
	cPsi.bD1 = bD1;
	cPsi.s1 = s1; 
	cPsi.ptr1 = ptr1;

	//#ifndef USE_SELECT1
	cPsi.off1 = off1;
	//#else
	//free(off1);
	//#endif
	
	cPsi.bc = bc;
	cPsi.bcsize = bcsize;
	cPsi.bitsbcsize = bits(bcsize);   
	cPsi.bitspsiSize = bits(psiSize);
	
	cPsi.vocSize = vocSize;
	cPsi.psiSize = psiSize;
	cPsi.totalMem = totalSize;
		

	//frees resources not needed in advance
	if (diffs)
		free(diffs);
	
	//returns the data structure that holds the compressed psi.
	return cPsi;	
}
*/


unsigned long  getTotalMem(VbyteCompressedPsi *cPsi) {
	return cPsi->totalMem;
	
}





unsigned long  getVbytePsiSizeBitsUptoPosition(VbyteCompressedPsi *cPsi, size_t position) {
	register size_t index;
	size_t sampleIndex, positionsSinceSample, bcpos;

	size_t psiValue; 
	size_t runLenght;
	
	size_t maxrun= cPsi->maxrun;
	size_t negoffset = cPsi->negoffset;
		
	sampleIndex = position / cPsi->T;
	psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
	bcpos = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;
	
	for(index=0;index<positionsSinceSample;index++) {
		size_t num;
		BC_TO_DEC(cPsi->bc, bcpos, num);
		
		if (num <= maxrun) { // Estamos nun run
			runLenght = num;

			if(index+runLenght>=positionsSinceSample){
				//return psiValue+positionsSinceSample-index;									
				break;
			}
			else {
				psiValue += runLenght;
				index += runLenght-1;
			}			
		}
		else {
			if (num<=negoffset) {  //positive
				num-=maxrun;
			}
			else {
				num-=negoffset;    //negative
				num *= -1;
			}
			
			psiValue +=num;
		}
	}	

	return bcpos*8  + (sampleIndex * (cPsi->sampleSize + cPsi->pointerSize) );
 
}


unsigned int getVbytePsiValue(VbyteCompressedPsi *cPsi, size_t position) {
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, bcpos;

	size_t psiValue; 
	unsigned int runLenght;
	
	size_t maxrun= cPsi->maxrun;
	size_t negoffset = cPsi->negoffset;
		
	sampleIndex = position / cPsi->T;
	psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
	bcpos = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;
	
	for(index=0;index<positionsSinceSample;index++) {
		size_t num;
		BC_TO_DEC(cPsi->bc, bcpos, num);
		
		if (num <= maxrun) { // Estamos nun run
			runLenght = num;

			if(index+runLenght>=positionsSinceSample){
				return psiValue+positionsSinceSample-index;
			}
			else {
				psiValue += runLenght;
				index += runLenght-1;
			}			
		}
		else {
			if (num<=negoffset) {  //positive
				num-=maxrun;
			}
			else {
				num-=negoffset;    //negative
				num *= -1;
			}
			
			psiValue +=num;
		}
	}	
	return psiValue;
	
}




/**/  // version corrected 2015 (problems with runs).
void getVbytePsiValueBuffer(VbyteCompressedPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	//printf("\n CALL getVbytePsiValueBuffer [%zu,%zu]\n",ini,end);
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, bcpos;

	size_t psiValue; 
	unsigned int runLenght;
	
	size_t maxrun= cPsi->maxrun;
	size_t negoffset = cPsi->negoffset;

	size_t position = ini;		
	sampleIndex = position / cPsi->T;
	psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
	bcpos = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@

	for(index=0;index<positionsSinceSample;index++) {
		size_t num;
		BC_TO_DEC(cPsi->bc, bcpos, num);
		
		if (num <= maxrun) { // Estamos nun run
			runLenght = num;

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
		else {
			if (num<=negoffset) {  //positive
				num-=maxrun;
			}
			else {
				num-=negoffset;    //negative
				num *= -1;
			}
			
			psiValue +=num;
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
			psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);

		}
		else {  //non sampled value		
			size_t num;
			BC_TO_DEC(cPsi->bc, bcpos, num);
			if (num <= maxrun) { // Estamos nun run
				runLenght = num;
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
			else {
				if (num<=negoffset) {  //positive
					num-=maxrun;
				}
				else {
					num-=negoffset;    //negative
					num *= -1;
				}
				
				psiValue +=num;
			}
		}

		*buffer++ = psiValue;
		position ++;
	} 
			
	return;
}



void getVbytePsiValueBuffer_2(VbyteCompressedPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getVbytePsiValue(cPsi,i);
	return; 
}


void getVbytePsiValueBuffer_1(VbyteCompressedPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getVbytePsiValue(cPsi,i);
	return;
}
	







//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst   value >= fst  and <=sec
// sets in i2 de position in buffer of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found

int getfindVbytePsiValueBuffer(VbyteCompressedPsi *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
	//printf("\n CALL getfindVbytePsiValueBuffer [%zu,%zu]\n",ini,end);

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, bcpos;

	size_t psiValue; 
	unsigned int runLenght;
	
	size_t maxrun= cPsi->maxrun;
	size_t negoffset = cPsi->negoffset;

	size_t position = ini;		
	sampleIndex = position / cPsi->T;
	psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
	bcpos = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@

	for(index=0;index<positionsSinceSample;index++) {
		size_t num;
		BC_TO_DEC(cPsi->bc, bcpos, num);
		
		if (num <= maxrun) { // Estamos nun run
			runLenght = num;

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
		else {
			if (num<=negoffset) {  //positive
				num-=maxrun;
			}
			else {
				num-=negoffset;    //negative
				num *= -1;
			}
			
			psiValue +=num;
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
			psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);

		}
		else {  //non sampled value		
			size_t num;
			BC_TO_DEC(cPsi->bc, bcpos, num);
			if (num <= maxrun) { // Estamos nun run
				runLenght = num;
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
			else {
				if (num<=negoffset) {  //positive
					num-=maxrun;
				}
				else {
					num-=negoffset;    //negative
					num *= -1;
				}
				
				psiValue +=num;
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

int getfindVbytePsiValue(VbyteCompressedPsi *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
	//printf("\n CALL getfindVbytePsiValue [%zu,%zu]\n",ini,end);

	int count =0;
	int toreturn=0;
	
	ulong fstt=fst;
	ulong sect=sec;
	
	
	register size_t index;
	size_t sampleIndex, positionsSinceSample, bcpos;

	size_t psiValue; 
	unsigned int runLenght;
	
	size_t maxrun= cPsi->maxrun;
	size_t negoffset = cPsi->negoffset;

	size_t position = ini;		
	sampleIndex = position / cPsi->T;
	psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);
	bcpos = bitread64(cPsi->samplePointers,sampleIndex*cPsi->pointerSize,cPsi->pointerSize);
	
	positionsSinceSample = position%cPsi->T;
	
	int inRun=0;//@@
	int inRunRemain=0;//@@

	for(index=0;index<positionsSinceSample;index++) {
		size_t num;
		BC_TO_DEC(cPsi->bc, bcpos, num);
		
		if (num <= maxrun) { // Estamos nun run
			runLenght = num;

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
		else {
			if (num<=negoffset) {  //positive
				num-=maxrun;
			}
			else {
				num-=negoffset;    //negative
				num *= -1;
			}
			
			psiValue +=num;
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
			psiValue = bitread64(cPsi->samples,sampleIndex*cPsi->sampleSize,cPsi->sampleSize);

		}
		else {  //non sampled value		
			size_t num;
			BC_TO_DEC(cPsi->bc, bcpos, num);
			if (num <= maxrun) { // Estamos nun run
				runLenght = num;
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
			else {
				if (num<=negoffset) {  //positive
					num-=maxrun;
				}
				else {
					num-=negoffset;    //negative
					num *= -1;
				}
				
				psiValue +=num;
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







void storeVbyteCompressedPsi(VbyteCompressedPsi *compressedPsi, char *filename) {

	FILE *file;

	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	size_t write_err;

	
	write_err=fwrite(&(compressedPsi->T)        , sizeof(int),1,file);
	write_err=fwrite(&(compressedPsi->psiSize)  , sizeof(size_t),1,file);
	write_err=fwrite(&(compressedPsi->maxrun)   , sizeof(size_t),1,file);
	write_err=fwrite(&(compressedPsi->negoffset), sizeof(size_t),1,file);

	write_err=fwrite(  &(compressedPsi->numberOfSamples), sizeof(size_t),1,file);
	
	write_err=fwrite(  &(compressedPsi->sampleSize), sizeof(int),1,file);
	write_err=fwrite( 	compressedPsi->samples,          sizeof(size_t), (((size_t)compressedPsi->numberOfSamples*compressedPsi->sampleSize+WW -1)/WW), file);	
	
	write_err=fwrite(  &(compressedPsi->pointerSize), sizeof(int),1,file);
	write_err=fwrite( 	compressedPsi->samplePointers,   sizeof(size_t), (((size_t)compressedPsi->numberOfSamples*compressedPsi->pointerSize+WW-1)/WW), file);
	
	
	write_err=fwrite(&(compressedPsi->bcsize)    , sizeof(size_t),1,file);
	write_err=fwrite(compressedPsi->bc           , sizeof(char)  , (compressedPsi->bcsize), file);

	write_err=fwrite(&(compressedPsi->bitsbcsize), sizeof(size_t),1,file);
	write_err=fwrite(&(compressedPsi->bitspsiSize), sizeof(size_t),1,file);
	write_err=fwrite(&(compressedPsi->totalMem), sizeof(size_t),1,file);

	fclose(file);	

}



VbyteCompressedPsi loadVbyteCompressedPsi(char *filename , bitmap bD) {
	
	VbyteCompressedPsi compressedPsi;
	FILE *file;

	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}fflush(stdout);fflush(stderr);
	
	size_t read_err;
		
	read_err=fread(&(compressedPsi.T)         , sizeof(int),1,file);
	read_err=fread(&(compressedPsi.psiSize)   , sizeof(size_t),1,file);
	read_err=fread(&(compressedPsi.maxrun)    , sizeof(size_t),1,file);
	read_err=fread(&(compressedPsi.negoffset) , sizeof(size_t),1,file);


	read_err=fread( &(compressedPsi.numberOfSamples), sizeof(size_t), 1, file);
	
	read_err=fread( &(compressedPsi.sampleSize), sizeof(int), 1,file);
	
	compressedPsi.samples        = (size_t *)malloc((( (size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+WW-1)/WW)*sizeof(size_t));
	read_err=fread(        compressedPsi.samples, sizeof(size_t), (( (size_t)compressedPsi.numberOfSamples*compressedPsi.sampleSize+WW-1)/WW) , file );
	
	read_err=fread( &(compressedPsi.pointerSize), sizeof(int),1,file);
	
	compressedPsi.samplePointers = (size_t *)malloc((( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW)*sizeof(size_t));
	read_err=fread( compressedPsi.samplePointers, sizeof(size_t), (( (size_t)compressedPsi.numberOfSamples*compressedPsi.pointerSize+WW-1)/WW), file);



	read_err=fread(&(compressedPsi.bcsize), sizeof(size_t),1,file);
	compressedPsi.bc = (unsigned char *) malloc (sizeof(unsigned char) * (compressedPsi.bcsize));
	read_err=fread(compressedPsi.bc, sizeof(char), (compressedPsi.bcsize), file);



	read_err=fread(&(compressedPsi.bitsbcsize), sizeof(size_t),1,file);
	read_err=fread(&(compressedPsi.bitspsiSize), sizeof(size_t),1,file);
	read_err=fread(&(compressedPsi.totalMem), sizeof(size_t),1,file);

	fclose(file);			
	return compressedPsi;
}


