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
 *   Version **psiR3Hybrid**: 
 *   Psi is divided into three ranges:
 *    - range of Subjects  is Ps= Psi[0, n-1]
 *    - range of Predicate is Pp= Psi[n/3, 2n/3-1]
 *    - range of Objects   is Po= Psi[2n/3, n-1+1]
 *   They are compressed separately using psiPlain.c and psiHuffmanRLER3.c, actually
 *   using 2 PlainPsi structs (S,O) and 1 HuffmanCompressedPsiR3 struct (P): 
 *       plPsiS, hcPsiP, and plPsiO 
 *   The operations in this file (psiR3Hybrid) are delegated to the proper 
 *   structure, depending on the position of Psi we want to access.
 */
 

#include "psiR3Hybrid.h"

// IMPLEMENTACION DAS FUNCIONS

void destroyPsiR3Hybrid(CompressedPsiR3Hybrid *compressedPsi) {
	destroyPlainPsi(&(compressedPsi->plPsiS));
	destroyHuffmanCompressedPsiR3(&(compressedPsi->hcPsiP));
	destroyPlainPsi(&(compressedPsi->plPsiO));
}

//HuffmanCompressedPsi huffmanCompressPsi(unsigned int *Psi, size_t psiSize, unsigned int T, unsigned int nS) {
CompressedPsiR3Hybrid CompressPsiR3Hybrid(unsigned int *Psi, size_t psiSize, unsigned int T, unsigned int nS) {	
	CompressedPsiR3Hybrid cPsi;

	cPsi.T = T;
	cPsi.nS = nS;
	cPsi.n = psiSize;

	cPsi.inioffset[SUB]=0;	
	cPsi.inioffset[PRE]=psiSize/3;	
	cPsi.inioffset[OBJ]=psiSize/3*2;	

	cPsi.minPsivalue[SUB]=psiSize/3;	
	cPsi.minPsivalue[PRE]=psiSize/3*2;	
	cPsi.minPsivalue[OBJ]=0;	
 
 //HuffmanCompressedPsiR3 hc = huffmanCompressPsiR3 (Psi, psiSize , 0, T, nS);

	cPsi.plPsiS = createPlainPsi       (&Psi[0          ], psiSize/3  );
	cPsi.hcPsiP = huffmanCompressPsiR3 (&Psi[psiSize/3  ], psiSize/3  , cPsi.minPsivalue[PRE], T, nS);
	cPsi.plPsiO = createPlainPsi       (&Psi[psiSize/3*2], psiSize/3+1);	

	cPsi.totalMem = cPsi.plPsiS.totalMem + cPsi.hcPsiP.totalMem + cPsi.plPsiO.totalMem;

	printf("\n inioffset[]= < %zu, %zu, %zu >",cPsi.inioffset[SUB], cPsi.inioffset[PRE], cPsi.inioffset[OBJ]);
	printf("\n minPsiVal[]= < %zu, %zu, %zu >",cPsi.minPsivalue[SUB], cPsi.minPsivalue[PRE], cPsi.minPsivalue[OBJ]);
	fflush(stdout);fflush(stderr);

#ifndef SKIP_CHECK_PSI_WORKS_OK  //SEE MAKEFILE
	{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
		fprintf(stderr,"\n Test compress/uncompress PSI is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
		size_t i;
		uint val1,val2;	
		fflush(stdout);

		for (i=0; i<10; i++) {
				val1= getPsiR3HybridValue(&cPsi, i);
				fprintf(stderr,"\n psi[%zu] vale (compressed = %u) || (original= %u), ",i, val1,Psi[i]);
		}
		
		uint count=0;
		for (i=0; i<psiSize; i++) {

				val2=Psi[i];
				val1= getPsiR3HybridValue(&cPsi, i);
				//val1 =  getHuffmanPsiR3Value(&hc,i);


				if (val1 != val2) { count++;
						fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
						if (count > 50) { exit(0);}
				}
		}
			fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
	} /////////////////////////////////////////////////////////////////////////////////////
#endif
	
	//returns the data structure that holds the compressed psi.
	return cPsi;	
}


unsigned long  getPsiR3HybridSizeBitsUptoPosition(CompressedPsiR3Hybrid *cPsi, size_t position) {
	
	if (position < cPsi->inioffset[PRE]) {
		return getPlainPsiSizeBitsUptoPosition(&(cPsi->plPsiS),position);
	}
	else if (position < cPsi->inioffset[OBJ]) {
		return getPlainPsiSizeBitsUptoPosition    (&(cPsi->plPsiS),cPsi->n/3-1) +
			   getHuffmanPsiR3SizeBitsUptoPosition(&(cPsi->hcPsiP), position -cPsi->inioffset[PRE]);
//			   getHuffmanPsiR3SizeBitsUptoPosition(&(cPsi->hcPsiP), position % (cPsi->n/3));
	}
	else {
		return getPlainPsiSizeBitsUptoPosition      (&(cPsi->plPsiS),cPsi->n/3-1) +
			   getHuffmanPsiR3SizeBitsUptoPosition(&(cPsi->hcPsiP),cPsi->n/3-1) +
			   getPlainPsiSizeBitsUptoPosition      (&(cPsi->plPsiO), position -cPsi->inioffset[OBJ]);
//			   getHuffmanPsiR3SizeBitsUptoPosition(&(cPsi->plPsiO), position % (cPsi->n/3+1));
	}
}


unsigned int getPsiR3HybridValue(CompressedPsiR3Hybrid *cPsi, size_t position) {

	if (position < cPsi->inioffset[PRE]) {
		return getPlainPsiValue(&(cPsi->plPsiS),position);
	}
	else if (position < cPsi->inioffset[OBJ]) {
		return getHuffmanPsiR3Value(&(cPsi->hcPsiP), position - cPsi->inioffset[PRE]);
	}
	else {
		return getPlainPsiValue(&(cPsi->plPsiO), position - cPsi->inioffset[OBJ]);
	}
}

void getPsiR3HybridValueBuffer_1(CompressedPsiR3Hybrid *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getPsiR3HybridValue(cPsi,i);
	return;
}


void getPsiR3HybridValueBuffer(CompressedPsiR3Hybrid *cPsi, uint *buffer, size_t ini, size_t end) {

//getPsiR3HybridValueBuffer_1 (cPsi, buffer,ini,end);
//return;

	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ((ini < cPsi->inioffset[PRE]) && (end >=cPsi->inioffset[PRE])) {   
		//getPsiR3HybridValueBuffer_1 (cPsi, buffer,ini,end);
		getPlainPsiValueBuffer    (&(cPsi->plPsiS), buffer                      , ini , cPsi->n/3-1);
		getHuffmanPsiR3ValueBuffer (&(cPsi->hcPsiP), &buffer[cPsi->inioffset[PRE]], 0   , cPsi->n/3-1);
		getPlainPsiValueBuffer    (&(cPsi->plPsiO), &buffer[cPsi->inioffset[OBJ]], 0   , end-cPsi->inioffset[OBJ]);
		return;
	}
	
	//regular access (sequence of values for a SUBJECT, PRED, or OBJECT.

	if (ini < cPsi->inioffset[PRE]) {
		return getPlainPsiValueBuffer (&(cPsi->plPsiS), buffer,ini,end);
	}
	else if (ini < cPsi->inioffset[OBJ]) {
		ini-=cPsi->inioffset[PRE]; end -=cPsi->inioffset[PRE];
		return getHuffmanPsiR3ValueBuffer (&(cPsi->hcPsiP), buffer,ini,end);
	}
	else {
		ini-=cPsi->inioffset[OBJ]; end -=cPsi->inioffset[OBJ];
		return getPlainPsiValueBuffer (&(cPsi->plPsiO), buffer,ini,end);
	}

	//FALTA IMPLEMENTAR BEN!!
	//FALTA IMPLEMENTAR BEN!!
	//FALTA IMPLEMENTAR BEN!!
	//getPsiR3HybridValueBuffer_1 (cPsi, buffer,ini,end);
	//FALTA IMPLEMENTAR BEN!!
	//FALTA IMPLEMENTAR BEN!!
	//FALTA IMPLEMENTAR BEN!!
			
	return;
}



//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst   value >= fst  and <=sec
// sets in i2 de position in buffer of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found
int getfindPsiR3HybridValueBuffer(CompressedPsiR3Hybrid *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){

	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ( ((ini < cPsi->inioffset[PRE]) && (end >=cPsi->inioffset[PRE]) ) ||
		 ((ini < cPsi->inioffset[OBJ]) && (end >=cPsi->inioffset[OBJ]) )    ) {   
		printf("\n should not be here!! @getfindPsiR3HybridValueBuffer... exiting\n");
	}
	
	
	if (ini < cPsi->inioffset[PRE]) {
		return getfindPlainPsiValueBuffer(&(cPsi->plPsiS), buffer,ini,end,fst,sec,i1,i2);
	}
	else if (ini < cPsi->inioffset[OBJ]) {
		ini-=cPsi->inioffset[PRE]; end -=cPsi->inioffset[PRE];
		return getfindHuffmanPsiR3ValueBuffer(&(cPsi->hcPsiP), buffer,ini,end,fst,sec,i1,i2);
	}
	else {
		ini-=cPsi->inioffset[OBJ]; end -=cPsi->inioffset[OBJ];
		return getfindPlainPsiValueBuffer(&(cPsi->plPsiO), buffer,ini,end,fst,sec,i1,i2);
	}
}

//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// sets in i2 de position (from ini on) of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found
int getfindPsiR3HybridValue(CompressedPsiR3Hybrid *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){
  #ifndef NDEBUG

	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ( ((ini < cPsi->inioffset[PRE]) && (end >=cPsi->inioffset[PRE]) ) ||
		 ((ini < cPsi->inioffset[OBJ]) && (end >=cPsi->inioffset[OBJ]) )    ) {   
		printf("\n should not be here!! @getfindPsiR3HybridValue... exiting\n");
	}
  #endif	
	
	if (ini < cPsi->inioffset[PRE]) {
		return getfindPlainPsiValue (&(cPsi->plPsiS), ini,end,fst,sec,i1,i2);
	}
	else if (ini < cPsi->inioffset[OBJ]) {
		ini-=cPsi->inioffset[PRE]; end -=cPsi->inioffset[PRE];
		return getfindHuffmanPsiR3Value (&(cPsi->hcPsiP), ini,end,fst,sec,i1,i2);
	}
	else {
		ini-=cPsi->inioffset[OBJ]; end -=cPsi->inioffset[OBJ];
		return getfindPlainPsiValue (&(cPsi->plPsiO), ini,end,fst,sec,i1,i2);
	}
}


//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// stops if i1 was set
// returns 0 if all values are < fst.
// NO LONGER returns 1 if a value >= fst and <=sec was found
// returns x+1, where x is  the first value >= fst and <=sec was found    (+1 to ensure zero is not returned as a valid value);

ulong getfindPsiLeftOnlyR3HybridValue(CompressedPsiR3Hybrid *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {
 #ifndef NDEBUG
	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ( ((ini < cPsi->inioffset[PRE]) && (end >=cPsi->inioffset[PRE]) ) ||
		 ((ini < cPsi->inioffset[OBJ]) && (end >=cPsi->inioffset[OBJ]) )    ) {   
		printf("\n should not be here!! @getfindPsiR3HybridValue... exiting\n");
	}
 #endif
	
	if (ini < cPsi->inioffset[PRE]) {
		return getfindLeftOnlyPlainPsiValue (&(cPsi->plPsiS), ini,end,fst,sec,i1);
	}
	else if (ini < cPsi->inioffset[OBJ]) {
		ini-=cPsi->inioffset[PRE]; end -=cPsi->inioffset[PRE];
		return getfindLeftOnlyHuffmanPsiR3Value (&(cPsi->hcPsiP), ini,end,fst,sec,i1);
	}
	else {
		ini-=cPsi->inioffset[OBJ]; end -=cPsi->inioffset[OBJ];
		return getfindLeftOnlyPlainPsiValue (&(cPsi->plPsiO), ini,end,fst,sec,i1);
	}
}







void storePsiR3Hybrid(CompressedPsiR3Hybrid *compressedPsi, char *filename) {
	FILE *file;
	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	size_t write_err;
	write_err=fwrite(  &(compressedPsi->T) , sizeof(int)   ,1,file);
	write_err=fwrite(  &(compressedPsi->nS), sizeof(int)   ,1,file);
	write_err=fwrite(  &(compressedPsi->n) , sizeof(size_t),1,file);

	write_err=fwrite( 	compressedPsi->inioffset  , sizeof(size_t), 3, file);	
	write_err=fwrite( 	compressedPsi->minPsivalue, sizeof(size_t), 3, file);	
	fclose(file);
		
	char filenameS[2048], filenameP[2048], filenameO[2048];
	sprintf(filenameS,"%s.%s",filename,"S");
	sprintf(filenameP,"%s.%s",filename,"P");
	sprintf(filenameO,"%s.%s",filename,"O");
	storePlainPsi(&(compressedPsi->plPsiS), filenameS);
	storeHuffmanCompressedPsiR3(&(compressedPsi->hcPsiP), filenameP);
	storePlainPsi(&(compressedPsi->plPsiO), filenameO);

}

CompressedPsiR3Hybrid loadPsiR3Hybrid(char *filename) {
	CompressedPsiR3Hybrid compressedPsi;     
	FILE *file;
	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}
		fflush(stdout);fflush(stderr);
	
	size_t read_err;
	
	read_err=fread( &(compressedPsi.T) , sizeof(int)   , 1,file);
	read_err=fread( &(compressedPsi.nS), sizeof(int)   , 1,file);
	read_err=fread( &(compressedPsi.n) , sizeof(size_t), 1,file);

	read_err=fread( compressedPsi.inioffset  , sizeof(size_t), 3, file);	
	read_err=fread( compressedPsi.minPsivalue, sizeof(size_t), 3, file);		
	fclose(file);			

	char filenameS[2048], filenameP[2048], filenameO[2048];
	sprintf(filenameS,"%s.%s",filename,"S");
	sprintf(filenameP,"%s.%s",filename,"P");
	sprintf(filenameO,"%s.%s",filename,"O");
	compressedPsi.plPsiS = loadPlainPsi(filenameS);
	compressedPsi.hcPsiP = loadHuffmanCompressedPsiR3(filenameP);
	compressedPsi.plPsiO = loadPlainPsi(filenameO);

	compressedPsi.totalMem = compressedPsi.plPsiS.totalMem + compressedPsi.hcPsiP.totalMem + compressedPsi.plPsiO.totalMem;

	return compressedPsi;
}

