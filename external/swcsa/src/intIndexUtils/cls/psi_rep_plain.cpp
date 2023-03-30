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

#include "psi_rep_plain.h"

// IMPLEMENTACION DAS FUNCIONS

psi_rep_plain::~psi_rep_plain() {

#ifdef PSI_PLAIN_LOG_BASED	
	if (samples)
		free(samples);
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	if (psi)
		free(psi);
#endif		

}



psi_rep_plain::psi_rep_plain() {
	totalMem=0;
	n=0;
	sampleSize=0;
	#ifndef PSI_PLAIN_LOG_BASED
		//PSI as a uint32 array
		psi=NULL;
	#endif
	#ifdef PSI_PLAIN_LOG_BASED
		//PSI compressed as k-bit array
		samples=NULL;			    // Psi, comprimido a k-bits (sampleSize) por elmento
		OFFSET=0;
	#endif
}

psi_rep_plain::psi_rep_plain(unsigned int *Psi, size_t psiSize, size_t inioffset) {
	build(Psi,psiSize,inioffset,0,0);
}

//psi_rep_plain::psi_rep_plain(unsigned int *Psi, size_t psiSize) {
psi_rep_plain::psi_rep_plain(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=0, unsigned int nS=0, uint *psiTypes=NULL){
	build(Psi,psiSize,inioffset,0,0);
}
	
//	void psi_rep_plain::build(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=0, unsigned int nS=0) {
//		build(Psi,psiSize,inioffset);
//	}


/** Internal building function, same parameters as the base constructor. */
	void psi_rep_plain::build(unsigned int *Psi, size_t psiSize,  size_t inioffset=0, /*--> not used -->*/ unsigned int T=0, unsigned int nS=0) {
	
	
	printf("\n call to psi_rep_plain - compress Psi\n");fflush(stdout);fflush(stderr);
	
//	PlainPsi cPsi;
//	//	cPsi.samples=NULL;
//	//	cPsi.psi=NULL;
    	
	size_t n = psiSize;
	size_t sampleSize = sizeof(uint)*8;  //default
	size_t index;


#ifdef PSI_PLAIN_LOG_BASED		

	size_t max_value = 0;
	size_t min_value = 19999999999999L;
	for(index=0; index<n; index++) {
		if (Psi[index] > max_value) max_value = Psi[index];
		if (Psi[index] < min_value) min_value = Psi[index];		
	}
	size_t OFFSET= min_value;

	#ifdef R3H_WITHGAPS
		this->OFFSET = OFFSET;
	#else
		this->OFFSET=0;
	#endif

	
	sampleSize = bits(max_value-OFFSET);	 //sampleSize = bits(max_value);	
	
	fprintf(stderr,"\n espacio para bit-encoded-psi = %lu (bytes)", sizeof(size_t)* (ulong_len(sampleSize,n)) );
	fprintf(stderr,"\n n vale = %lu , bits_per_element= %zu",  n , sampleSize );
	size_t *samples;
	samples = (size_t *)malloc(sizeof(size_t)* (ulong_len(sampleSize,n)) );
		samples[ (ulong_len(sampleSize,n)) -1] = 00000000;  //initialized only to avoid valgrind warnings
	
	// Comprimimos secuencialmente (haber� que levar un punteiro desde o inicio)
	size_t samplesPtr = 0;
	for(index=0; index<psiSize; index++) {
		bitwrite64(samples,samplesPtr,sampleSize, Psi[index]- OFFSET);
		samplesPtr += sampleSize;
	}
	
	/*CHECK it works*/
	//		samplesPtr = 0;
	//		for(index=0; index<psiSize; index++) {
	//						/**/size_t tmp = bitread64(samples,samplesPtr,sampleSize) +OFFSET;			
	//						/**/if (tmp != Psi[index]) {printf("\n FAILED BWRITE64-PSI: %zu, %u",tmp, Psi[index]);		exit(0);}
	//						samplesPtr += sampleSize;					
	//		}
	//		printf("\n bitwrite64/bitread64 test worked \n");


	this->samples = samples;		
	this->totalMem = ((size_t) sizeof(psi_rep_plain)) + sizeof(size_t)* (ulong_len(sampleSize,n)) ;
#endif
#ifndef PSI_PLAIN_LOG_BASED		
    
	uint *psi = (uint *)malloc(sizeof(uint)*n) ;	
	for(index=0; index<n; index++) psi[index]=Psi[index];
	this->psi = psi;
	this->totalMem = ((size_t) sizeof(psi_rep_plain)) + sizeof(int)*n;
#endif		

	this->n = psiSize;
	this->sampleSize = sampleSize;

	fprintf(stderr,"\n@@@@@@@@@ psi size: bits-per-entry= %zu || %zu bytes ", sampleSize, this->totalMem);
	fflush(stdout);fflush(stderr);
	
/*
{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
	fprintf(stderr,"\n Test compress/uncompress PSI - psi_rep_plain - is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
	size_t i;
	uint val1,val2;	
	fflush(stdout);
	
	uint count=0;
	for (i=0; i<psiSize; i++) {
			val1= this->getPsiValue(i);
			val2=Psi[i];
			if (val1 != val2) { count++;
					fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
					//if (count > 50) 
						{ exit(0);;}
			}
	}
		fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
} /////////////////////////////////////////////////////////////////////////////////////
*/

	
	//returns the data structure that holds the compressed psi.
	//return cPsi;	
}





//for Nieves' statistics
ulong psi_rep_plain::getPsiSizeBitsUptoPosition(size_t position) { 

	//unsigned long  getPlainPsiSizeBitsUptoPosition(PlainPsi *cPsi, size_t position) {	
#ifdef PSI_PLAIN_LOG_BASED		
	return (unsigned long) ( position * (this->sampleSize));
#endif
#ifndef PSI_PLAIN_LOG_BASED	
	return (unsigned long) ( position * sizeof(uint));
#endif
}



unsigned int psi_rep_plain::getPsiValue( size_t position) {
	unsigned int psiValue;
		
#ifdef PSI_PLAIN_LOG_BASED	
	#ifdef R3H_WITHGAPS
	psiValue = bitread64(this->samples,position*this->sampleSize,this->sampleSize) + this->OFFSET;
	#else
	psiValue = bitread64(this->samples,position*this->sampleSize,this->sampleSize);
	#endif	
#endif

#ifndef PSI_PLAIN_LOG_BASED		
	psiValue = this->psi[position];
#endif	

	return psiValue;
}


void psi_rep_plain::getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end) {
//void getPlainPsiValueBuffer_1(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getPsiValue(i);
	return;
}


void psi_rep_plain::getPsiValueBuffer(uint *buffer, size_t ini, size_t end) {	
//void getPlainPsiValueBuffer(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {

#ifdef PSI_PLAIN_LOG_BASED		
	{   register size_t i;
		unsigned int psiValue; 				
		size_t *samples = this->samples;
		size_t sampleSize = this->sampleSize;

	#ifdef R3H_WITHGAPS		
		size_t OFFSET = this->OFFSET;
	#endif
		
		size_t pos= ini * sampleSize;
		for(i=ini;i<=end;i++) {						
			psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;	
			#ifdef R3H_WITHGAPS
			*buffer++ = psiValue+OFFSET;
			#else
			*buffer++ = psiValue;
			#endif
		} 
	}
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	memcpy(buffer, &(this->psi[ini]), sizeof(uint)*(end-ini+1));
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

int psi_rep_plain::getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindPlainPsiValueBuffer(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
	
	ulong fstt=fst;
	ulong sect=sec;
	register size_t i;
	unsigned int psiValue;
	int toreturn=0;

#ifdef PSI_PLAIN_LOG_BASED	
	size_t *samples = this->samples;
	size_t sampleSize = this->sampleSize;
	size_t pos= ini * sampleSize;
			#ifdef R3H_WITHGAPS
	size_t OFFSET = this->OFFSET;	
			#endif
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	uint *psi = this->psi;
#endif
	
	size_t count=0;
	for(i=ini;i<=end;i++) {						
#ifdef PSI_PLAIN_LOG_BASED			
		psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;
			#ifdef R3H_WITHGAPS		
		psiValue+=OFFSET;	
			#endif
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

int psi_rep_plain::getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindPlainPsiValue(PlainPsi *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
	ulong fstt=fst;
	ulong sect=sec;
	register size_t i;
	unsigned int psiValue;
	int toreturn=0;


#ifdef PSI_PLAIN_LOG_BASED	
	size_t *samples = this->samples;
	size_t sampleSize = this->sampleSize;
	size_t pos= ini * sampleSize;
			#ifdef R3H_WITHGAPS	
	size_t OFFSET = this->OFFSET;
			#endif
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	uint *psi = this->psi;
#endif

	
	size_t count=0;
	for(i=ini;i<=end;i++) {						
#ifdef PSI_PLAIN_LOG_BASED			
		psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;
				#ifdef R3H_WITHGAPS
		psiValue +=OFFSET;	
				#endif
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



//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// stops if i1 was set
// returns 0 if all values are < fst.
// returns x+1, where x is  the first value >= fst and <=sec was found    (+1 to ensure zero is not returned as a valid value);

ulong psi_rep_plain::getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {
//ulong getfindLeftOnlyPlainPsiValue(PlainPsi *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {

	ulong fstt=fst;
	ulong sect=sec;
	register size_t i;
	unsigned int psiValue;
	int toreturn=0;


#ifdef PSI_PLAIN_LOG_BASED	
	size_t *samples = this->samples;
	size_t sampleSize = this->sampleSize;
	size_t pos= ini * sampleSize;
			#ifdef R3H_WITHGAPS	
	size_t OFFSET = this->OFFSET;
			#endif
#endif
#ifndef PSI_PLAIN_LOG_BASED		
	uint *psi = this->psi;
#endif

	
	size_t count=0;
	for(i=ini;i<=end;i++) {						
#ifdef PSI_PLAIN_LOG_BASED			
		psiValue = bitread64(samples, pos, sampleSize); pos+=sampleSize;
				#ifdef R3H_WITHGAPS
		psiValue +=OFFSET;	
				#endif
#endif
#ifndef PSI_PLAIN_LOG_BASED	
		psiValue = psi[i];		
#endif				
		{
			if ((psiValue >=fstt)&& (psiValue <=sec)) {
					*i1= count;   fstt = 0xFFFFFFFFFFFFFFF;
					toreturn=1;
					//return toreturn;  ///////////////////
					return psiValue +1;  //////////////////	 (+1 to ensure zero is not returned as a valid value);				
			}
			// if ((psiValue >=fst)&& (psiValue <=sec)) {
			// 	*i2= count; 
			// 	toreturn=2;
			// }
			if (psiValue >=sec) {
					return toreturn;
			}
			count ++;		
		}
	} 	
	return toreturn;
	
}




void psi_rep_plain::psi_save(char *filename) {
//void storePlainPsi(PlainPsi *compressedPsi, char *filename) {
	FILE *file;
	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);		exit(0);
	}
	size_t write_err;
	uint psitype = PSIREP_PLAIN;
	write_err=fwrite(&(psitype), sizeof(uint)  ,1,file);
	
	write_err=fwrite(&(this->n)         , sizeof(size_t),1,file);
	write_err=fwrite(&(this->sampleSize), sizeof(uint)  ,1,file);
	
#ifdef PSI_PLAIN_LOG_BASED				
	#ifdef R3H_WITHGAPS
		write_err=fwrite(&(this->OFFSET)         , sizeof(size_t),1,file);
	#endif

	write_err=fwrite( 	this->samples, sizeof(size_t), (((size_t)this->n * this->sampleSize+WW-1)/WW), file);
#endif
#ifndef PSI_PLAIN_LOG_BASED			
	write_err=fwrite( 	this->psi, sizeof(uint), this->n, file);
#endif

	write_err=fwrite(&(this->totalMem), sizeof(size_t),1,file);
	fclose(file);	
}


psi_rep_plain * psi_rep_plain::psi_load (char *filename) {
//PlainPsi loadPlainPsi(char *filename) {	
	//PlainPsi compressedPsi;
	//	//		compressedPsi.samples=NULL;
	//	//		compressedPsi.psi=NULL;

	psi_rep_plain *ret = new psi_rep_plain();

	FILE *file;
	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename); 		exit(0);
	}
	fflush(stdout);fflush(stderr);	
	
	size_t read_err;

	uint psitype;
	read_err=fread(&psitype, sizeof(int), 1, file);
	if (psitype !=PSIREP_PLAIN) {
		perror("\npsi_load failed (wrong type psi_rep_plain):");
		exit(0);
	}

	
	read_err=fread(&(ret->n)         , sizeof(size_t), 1, file);
	read_err=fread(&(ret->sampleSize), sizeof(uint)  , 1, file);

#ifdef PSI_PLAIN_LOG_BASED	
	printf("\n-loading PSI_PLAIN_LOG_BASED");
	#ifdef R3H_WITHGAPS
	printf(" (WITH-GAPS)");	
		read_err=fread(&(ret->OFFSET) , sizeof(size_t),1,file);
	#endif

	ret->samples = (size_t *)malloc((( (size_t)ret->n * ret->sampleSize+WW-1)/WW)*sizeof(size_t));
	read_err=fread( ret->samples, sizeof(size_t), (( (size_t)ret->n*ret->sampleSize+WW-1)/WW), file);
	read_err=fread(&(ret->totalMem)       , sizeof(size_t), 1, file);

	printf("::  total bytes = %zu",ret->totalMem);
	
#endif
#ifndef PSI_PLAIN_LOG_BASED	
	printf("\n-loading PSI_PLAIN_with_uint32");
	ret->psi = (uint*) malloc ((ret->n) * sizeof(uint));	
	read_err=fread( ret->psi , sizeof(uint),ret->n,file);
	read_err=fread(&(ret->totalMem)       , sizeof(size_t), 1, file);	
	printf("::  total bytes = %zu",ret->totalMem);
#endif		
	fflush(stderr);fflush(stdout);
		

	fclose(file);			
//	return compressedPsi;
	return ret;

}


