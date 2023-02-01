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

#include "psiPlain.h"

// IMPLEMENTACION DAS FUNCIONS

void destroyPlainPsi(PlainPsi *compressedPsi) {
#ifdef PSI_PLAIN_LOG_BASED	
	if (compressedPsi->samples)
		free(compressedPsi->samples);
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	if (compressedPsi->psi)
		free(compressedPsi->psi);
#endif		
}

PlainPsi createPlainPsi(unsigned int *Psi, size_t psiSize) {
	
	PlainPsi cPsi;
//	cPsi.samples=NULL;
//	cPsi.psi=NULL;
    	
	size_t n = psiSize;
	size_t sampleSize = sizeof(uint)*8;  //default
	size_t index;

#ifdef PSI_PLAIN_LOG_BASED		

	size_t max_value = 0;
	for(index=0; index<n; index++) {
		if (Psi[index] > max_value) max_value = Psi[index];
	}

	sampleSize = bits(max_value);	
	fprintf(stderr,"\n espacio para bit-encoded-psi = %lu (bytes)", sizeof(size_t)* (ulong_len(sampleSize,n)) );
	fprintf(stderr,"\n n vale = %lu , bits_per_element= %zu",  n , sampleSize );
	size_t *samples;
	samples = (size_t *)malloc(sizeof(size_t)* (ulong_len(sampleSize,n)) );
		samples[ (ulong_len(sampleSize,n)) -1] = 00000000;  //initialized only to avoid valgrind warnings
	
	// Comprimimos secuencialmente (haber� que levar un punteiro desde o inicio)
	size_t samplesPtr = 0;
	for(index=0; index<psiSize; index++) {
		bitwrite64(samples,samplesPtr,sampleSize, Psi[index]);
		/**/size_t tmp = bitread64(samples,samplesPtr,sampleSize);			
		/**/if (tmp != Psi[index]) {printf("\n FAILED BWRITE64-PSI: %zu, %u",tmp, Psi[index]);		exit(0);}					
		samplesPtr += sampleSize;
	}

	cPsi.samples = samples;		
	cPsi.totalMem = ((size_t) sizeof(PlainPsi)) + sizeof(size_t)* (ulong_len(sampleSize,n)) ;
#endif
#ifndef PSI_PLAIN_LOG_BASED		
    
	uint *psi = (uint *)malloc(sizeof(uint)*n) ;	
	for(index=0; index<n; index++) psi[index]=Psi[index];
	cPsi.psi = psi;
	cPsi.totalMem = ((size_t) sizeof(PlainPsi)) + sizeof(int)*n;
#endif		

	cPsi.n = psiSize;
	cPsi.sampleSize = sampleSize;

	fprintf(stderr,"\n@@@@@@@@@ psi size: bits-per-entry= %zu || %zu bytes ", sampleSize, cPsi.totalMem);
	

	
#ifndef SKIP_CHECK_PSI_WORKS_OK  //SEE MAKEFILE
	{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
		fprintf(stderr,"\n Test compress/uncompress PSI is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
		size_t i;
		uint val1,val2;	
		fflush(stdout);

		for (i=0; i<10; i++) {
				val1= getPlainPsiValue(&cPsi, i);
				fprintf(stderr,"\n psi[%zu] vale (compressed = %u) || (original= %u), ",i, val1,Psi[i]);
		}
		
		uint count=0;
		for (i=0; i<psiSize; i++) {

				val2=Psi[i];
				val1= getPlainPsiValue(&cPsi, i);

				if (val1 != val2) { count++;
						fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
						if (count > 50) {i= i/0; exit(0);}
				}
		}
			fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
	} /////////////////////////////////////////////////////////////////////////////////////
#endif


	
	//returns the data structure that holds the compressed psi.
	return cPsi;	
}






unsigned long  getPlainPsiSizeBitsUptoPosition(PlainPsi *cPsi, size_t position) {	
	return (unsigned long) ( position * (cPsi->sampleSize));
}

unsigned int getPlainPsiValue(PlainPsi *cPsi, size_t position) {
	unsigned int psiValue;
		
#ifdef PSI_PLAIN_LOG_BASED	
	psiValue = bitread64(cPsi->samples,position*cPsi->sampleSize,cPsi->sampleSize);
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	psiValue = cPsi->psi[position];
#endif	
	return psiValue;

}

void getPlainPsiValueBuffer_1(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getPlainPsiValue(cPsi,i);
	return;
}

void getPlainPsiValueBuffer(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {

#ifdef PSI_PLAIN_LOG_BASED		
	{   register size_t i;
		unsigned int psiValue; 				
		size_t *samples = cPsi->samples;
		size_t sampleSize = cPsi->sampleSize;
		
		size_t pos= ini * sampleSize;
		for(i=ini;i<=end;i++) {						
			psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;		
			*buffer++ = psiValue;
		} 
	}
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	memcpy(buffer, &(cPsi->psi[ini]), sizeof(uint)*(end-ini+1));
#endif			
	return;
}

//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst   value >= fst  and <=sec
// sets in i2 de position in buffer of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found
int getfindPlainPsiValueBuffer(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
	
	ulong fstt=fst;
	ulong sect=sec;
	register size_t i;
	unsigned int psiValue;
	int toreturn=0;

#ifdef PSI_PLAIN_LOG_BASED	
	size_t *samples = cPsi->samples;
	size_t sampleSize = cPsi->sampleSize;
	size_t pos= ini * sampleSize;
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	uint *psi = cPsi->psi;
#endif
	
	size_t count=0;
	for(i=ini;i<=end;i++) {						
#ifdef PSI_PLAIN_LOG_BASED			
		psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;		
#endif
#ifndef PSI_PLAIN_LOG_BASED	
		psiValue = psi[i];		
#endif		
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
int getfindPlainPsiValue(PlainPsi *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
	ulong fstt=fst;
	ulong sect=sec;
	register size_t i;
	unsigned int psiValue;
	int toreturn=0;


#ifdef PSI_PLAIN_LOG_BASED	
	size_t *samples = cPsi->samples;
	size_t sampleSize = cPsi->sampleSize;
	size_t pos= ini * sampleSize;
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	uint *psi = cPsi->psi;
#endif

	
	size_t count=0;
	for(i=ini;i<=end;i++) {						
#ifdef PSI_PLAIN_LOG_BASED			
		psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;		
#endif
#ifndef PSI_PLAIN_LOG_BASED	
		psiValue = psi[i];		
#endif				
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
	return toreturn;
}



void storePlainPsi(PlainPsi *compressedPsi, char *filename) {
	FILE *file;
	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);		exit(0);
	}
	size_t write_err;
	
	write_err=fwrite(&(compressedPsi->n)         , sizeof(size_t),1,file);
	write_err=fwrite(&(compressedPsi->sampleSize), sizeof(uint)  ,1,file);
#ifdef PSI_PLAIN_LOG_BASED				
	write_err=fwrite( 	compressedPsi->samples, sizeof(size_t), (((size_t)compressedPsi->n * compressedPsi->sampleSize+WW-1)/WW), file);
#endif
#ifndef PSI_PLAIN_LOG_BASED			
	write_err=fwrite( 	compressedPsi->psi, sizeof(uint), compressedPsi->n, file);
#endif

	write_err=fwrite(&(compressedPsi->totalMem), sizeof(size_t),1,file);
	fclose(file);	
}



PlainPsi loadPlainPsi(char *filename) {	
	PlainPsi compressedPsi;
		compressedPsi.samples=NULL;
		compressedPsi.psi=NULL;

	FILE *file;
	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename); 		exit(0);
	}
	fflush(stdout);fflush(stderr);	
	
	size_t read_err;
	
	read_err=fread(&(compressedPsi.n)         , sizeof(size_t), 1, file);
	read_err=fread(&(compressedPsi.sampleSize), sizeof(uint)  , 1, file);

//#ifdef PSI_PLAIN_LOG_BASED			
//	compressedPsi.samples = (size_t *)malloc((( (size_t)compressedPsi.n * compressedPsi.sampleSize+WW-1)/WW)*sizeof(size_t));
//	read_err=fread( compressedPsi.samples, sizeof(size_t), (( (size_t)compressedPsi.n*compressedPsi.sampleSize+WW-1)/WW), file);
//	read_err=fread(&(compressedPsi.totalMem)       , sizeof(size_t), 1, file);
//	
//#endif
//#ifndef PSI_PLAIN_LOG_BASED	
//	compressedPsi.psi = (uint*) malloc ((compressedPsi.n) * sizeof(uint));	
//	read_err=fread( compressedPsi.psi , sizeof(uint),compressedPsi.n,file);
//	read_err=fread(&(compressedPsi.totalMem)       , sizeof(size_t), 1, file);	
//#endif		
//		
	
/********/	
	compressedPsi.psi = (uint*) malloc ((compressedPsi.n) * sizeof(uint));	
	read_err=fread( compressedPsi.psi , sizeof(uint),compressedPsi.n,file);
	read_err=fread(&(compressedPsi.totalMem)       , sizeof(size_t), 1, file);	

	size_t index;
	size_t max_value = 0;
//	size_t min_value = 19999999999999L;
	for(index=0; index<compressedPsi.n; index++) {
		if (compressedPsi.psi[index] > max_value) max_value = compressedPsi.psi[index];
//		if (compressedPsi.psi[index] < min_value) min_value = compressedPsi.psi[index];
	}

//	size_t OFFSET= min_value;
//	compressedPsi.sampleSize = bits(max_value-OFFSET);	

	compressedPsi.sampleSize = bits(max_value);	
	fprintf(stderr,"\n espacio para bit-encoded-psi = %lu (bytes)", sizeof(size_t)* (ulong_len(compressedPsi.sampleSize,compressedPsi.n)));
	fprintf(stderr,"\n n vale = %lu ,bits.sample= %zu\n",  compressedPsi.n , compressedPsi.sampleSize );
	compressedPsi.samples = (size_t *)malloc(sizeof(size_t)* (ulong_len(compressedPsi.sampleSize,compressedPsi.n)) );
		compressedPsi.samples[ (ulong_len(compressedPsi.sampleSize,compressedPsi.n)) -1] = 00000000;  //initialized only to avoid valgrind warnings
	
	// Comprimimos secuencialmente (haber� que levar un punteiro desde o inicio)
	size_t samplesPtr = 0;
	for(index=0; index<compressedPsi.n; index++) {
					bitwrite64(compressedPsi.samples,samplesPtr,compressedPsi.sampleSize, compressedPsi.psi[index]);
					samplesPtr += compressedPsi.sampleSize;
	}
	/*CHECK it works*/
			samplesPtr = 0;
			for(index=0; index<compressedPsi.n; index++) {
							/**/size_t tmp = bitread64(compressedPsi.samples,samplesPtr,compressedPsi.sampleSize);			
							/**/if (tmp != compressedPsi.psi[index]) {printf("\n FAILED BWRITE64-PSI: %zu, %u",tmp, compressedPsi.psi[index]);		exit(0);}
							samplesPtr += compressedPsi.sampleSize;					
			}
			printf("\n bitwrite64/bitread64 test worked \n");




	compressedPsi.totalMem = ((size_t) sizeof(PlainPsi)) + sizeof(size_t)* (ulong_len(compressedPsi.sampleSize,compressedPsi.n)) ;
	free(compressedPsi.psi);
	compressedPsi.psi=NULL;
	
/******/	


	
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


