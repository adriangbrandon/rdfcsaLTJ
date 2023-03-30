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

#include "psi_rep_hyb3r.h"

// IMPLEMENTACION DAS FUNCIONS

psi_rep_hyb3r::~psi_rep_hyb3r() {

	if (hcPsiS)  delete (psi_rep *) hcPsiS;
	if (hcPsiP)  delete (psi_rep *) hcPsiP;
	if (hcPsiO)  delete (psi_rep *) hcPsiO;
	
}



psi_rep_hyb3r::psi_rep_hyb3r() {
	totalMem=0;
	n=0;
	nS=0;
	T=0;

	hcPsiS = NULL;
	hcPsiP = NULL;
	hcPsiO = NULL;
	
	/// hcPsi[0]=NULL;hcPsi[1]=NULL;hcPsi[2]=NULL;
	/// rangeLen=0;
	
}


//PSIREP_PLAIN_UINT 1
//PSIREP_PLAIN      2
//PSIREP_PLAIN_RLE  3
//PSIREP_HUFF_RLE   4

//psi_rep_hyb3r::psi_rep_hyb3r(unsigned int *Psi, size_t psiSize) {
psi_rep_hyb3r::psi_rep_hyb3r(unsigned int *Psi, size_t psiSize, size_t inioffset=0, unsigned int T=0, unsigned int nS=0 , uint *psiTypes=NULL){
	//CompressedPsiR3H CompressPsiR3H(unsigned int *Psi, size_t psiSize, unsigned int T, unsigned int nS) {	
	if (psiTypes !=NULL) {

		  switch(psiTypes[0]) {
			case PSIREP_PLAIN:      this->hcPsiS = new psi_rep_plain();        break;
			case PSIREP_PLAIN_RLE:  this->hcPsiS = new psi_rep_plain_rle();    break;
			case PSIREP_HUFFMAN_RLE:   this->hcPsiS = new psi_rep_huffman_rle();  break;
			
			default: 				this->hcPsiS = new psi_rep_huffman_rle();
		  }

		  switch(psiTypes[1]) {
			case PSIREP_PLAIN:      this->hcPsiP = new psi_rep_plain();        break;
			case PSIREP_PLAIN_RLE:  this->hcPsiP = new psi_rep_plain_rle();    break;
			case PSIREP_HUFFMAN_RLE:   this->hcPsiP = new psi_rep_huffman_rle();  break;
			
			default: 				this->hcPsiP = new psi_rep_huffman_rle();
		  }

		  switch(psiTypes[2]) {
			case PSIREP_PLAIN:      this->hcPsiO = new psi_rep_plain();        break;
			case PSIREP_PLAIN_RLE:  this->hcPsiO = new psi_rep_plain_rle();    break;
			case PSIREP_HUFFMAN_RLE:   this->hcPsiO = new psi_rep_huffman_rle();  break;
			
			default: 				this->hcPsiO = new psi_rep_huffman_rle();
		  }
	}
	else {
		this->hcPsiS = new psi_rep_huffman_rle ();
		this->hcPsiP = new psi_rep_huffman_rle ();
		this->hcPsiO = new psi_rep_huffman_rle ();				
	}
	
	build(Psi,psiSize,inioffset,T,nS);
}


//void psi_rep_hyb3r::build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS){
void psi_rep_hyb3r::build(unsigned int *Psi, size_t psiSize,  size_t inioffset=0, unsigned int T=DEFAULT_T_PSI_VALUE,  unsigned int nS=16384) {



	printf("\n call to psi_rep_hyb3r - compress Psi\n");fflush(stdout);fflush(stderr);
	
	this->T = T;
	this->nS = nS;
	this->n = psiSize;

	this->inioffset[SUB]=0;	
	this->inioffset[PRE]=psiSize/3;	
	this->inioffset[OBJ]=psiSize/3*2;	

	this->minPsivalue[SUB]=psiSize/3;	
	this->minPsivalue[PRE]=psiSize/3*2;	
	this->minPsivalue[OBJ]=0;	
 
 //HuffmanCompressedPsiR3 hc = huffmanCompressPsiR3 (Psi, psiSize , 0, T, nS);

	//this->hcPsiS = new psi_rep_huffman_rle (&Psi[0          ], psiSize/3  , this->minPsivalue[SUB], T, nS);
	//this->hcPsiP = new psi_rep_huffman_rle (&Psi[psiSize/3  ], psiSize/3  , this->minPsivalue[PRE], T, nS);
	//this->hcPsiO = new psi_rep_huffman_rle (&Psi[psiSize/3*2], psiSize/3+1, this->minPsivalue[OBJ], T, nS);	
	
	this->hcPsiS->build(&Psi[0          ], psiSize/3  , this->minPsivalue[SUB], T, nS);
	this->hcPsiP->build(&Psi[psiSize/3  ], psiSize/3  , this->minPsivalue[PRE], T, nS);
	this->hcPsiO->build(&Psi[psiSize/3*2], psiSize/3+1, this->minPsivalue[OBJ], T, nS);	
	
///	this->hcPsi[0]=hcPsiS;
///	this->hcPsi[1]=hcPsiP;
///	this->hcPsi[2]=hcPsiO;
///	this->rangeLen = this->inioffset[PRE];  //= psiSize/3


////	this->totalMem = this->hcPsiS.totalMem + this->hcPsiP.totalMem + this->hcPsiO.totalMem;
//	this->totalMem = this->hcPsiS->getPsiTotalMem() + this->hcPsiP->getPsiTotalMem() + this->hcPsiO->getPsiTotalMem();
	this->totalMem =  this->hcPsiS->getPsiTotalMem();
	this->totalMem += this->hcPsiP->getPsiTotalMem();
	this->totalMem += this->hcPsiO->getPsiTotalMem();



	printf("\n inioffset[]= < %zu, %zu, %zu >",this->inioffset[SUB], this->inioffset[PRE], this->inioffset[OBJ]);
	printf("\n minPsiVal[]= < %zu, %zu, %zu >",this->minPsivalue[SUB], this->minPsivalue[PRE], this->minPsivalue[OBJ]);
	fflush(stdout);fflush(stderr);


#ifndef SKIP_CHECK_PSI_WORKS_OK  //SEE MAKEFILE
	{ ////////////////////////////// CHECKS IT WORKED ///////////////////////////////////////////////////////
		fprintf(stderr,"\n Test compress/uncompress PSI (psi_rep_hyb3r.cpp) is starting for all i in Psi[0..psiSize-1], "); fflush(stdout); fflush(stderr);
		size_t i;
		uint val1,val2;	
		fflush(stdout);

	//	for (i=0; i<10; i++) {
	//			//val1= getPsiR3HValue(&cPsi, i);
	//			val1=getPsiValue(i);
	//			fprintf(stderr,"\n psi[%zu] vale (compressed = %u) || (original= %u), ",i, val1,Psi[i]);
	//	}
    //    fprintf(stderr,"\n..");
	//	for (i=psiSize-10; i<psiSize; i++) {
	//			//val1= getPsiR3HValue(&cPsi, i);
	//			val1=getPsiValue(i);
	//			fprintf(stderr,"\n psi[%zu] vale (compressed = %u) || (original= %u), ",i, val1,Psi[i]);
	//	}
		
		uint count=0;
		for (i=0; i<psiSize; i++) {

				val2=Psi[i];
				//val1= getPsiR3HValue(&cPsi, i);
				val1=getPsiValue(i);
				
				if (val1 != val2) { count++;
						fprintf(stderr,"\n i=%zu,psi[i] vale (compressed = %u) <> (original= %u), ",i, val1,val2);
						if (count > 50) { exit(0);}
				}
		}
			fprintf(stderr,"\n Test compress/uncompress PSI passed *OK*, "); fflush(stdout); fflush(stderr);
	} /////////////////////////////////////////////////////////////////////////////////////
#endif

	
	//returns the data structure that holds the compressed psi.
	//return cPsi;	
}





//for Nieves' statistics
ulong psi_rep_hyb3r::getPsiSizeBitsUptoPosition(size_t position) { 
	//unsigned long  getPsiR3HSizeBitsUptoPosition(CompressedPsiR3H *cPsi, size_t position) {

	if (position < this->inioffset[PRE]) {
		//return getHuffmanPsiR3SizeBitsUptoPosition(&(this->hcPsiS),position);
		return this->hcPsiS->getPsiSizeBitsUptoPosition(position);
	}
	else if (position < this->inioffset[OBJ]) {
		//return getHuffmanPsiR3SizeBitsUptoPosition(&(this->hcPsiS),this->n/3-1) +
		//	   getHuffmanPsiR3SizeBitsUptoPosition(&(this->hcPsiP), position -this->inioffset[PRE]);
		return  this->hcPsiS->getPsiSizeBitsUptoPosition(this->n/3-1) + 
				this->hcPsiP->getPsiSizeBitsUptoPosition(position -this->inioffset[PRE]);
	}
	else {
		//return getHuffmanPsiR3SizeBitsUptoPosition(&(this->hcPsiS),this->n/3-1) +
		//	     getHuffmanPsiR3SizeBitsUptoPosition(&(this->hcPsiP),this->n/3-1) +
		//	     getHuffmanPsiR3SizeBitsUptoPosition(&(this->hcPsiO), position -this->inioffset[OBJ]);
		return  this->hcPsiS->getPsiSizeBitsUptoPosition(this->n/3-1) + 
				this->hcPsiP->getPsiSizeBitsUptoPosition(this->n/3-1) +
				this->hcPsiO->getPsiSizeBitsUptoPosition(position -this->inioffset[OBJ]);		
	}
}



unsigned int psi_rep_hyb3r::getPsiValue( size_t position) {
	unsigned int psiValue;

//	uint range = position/this->rangeLen;
//	return this->hcPsi[range]->getPsiValue(position - this->inioffset[range]);
///*FARIRANGES		
	if (position < this->inioffset[PRE]) {
		//return getHuffmanPsiR3Value(&(this->hcPsiS),position);
		return this->hcPsiS->getPsiValue(position);
	}
	else if (position < this->inioffset[OBJ]) {
		//return getHuffmanPsiR3Value(&(this->hcPsiP), position - this->inioffset[PRE]);
		return this->hcPsiP->getPsiValue(position - this->inioffset[PRE]);
	}
	else {
		//return getHuffmanPsiR3Value(&(this->hcPsiO), position - this->inioffset[OBJ]);
		return this->hcPsiO->getPsiValue(position - this->inioffset[OBJ]);
	}
///*/ 
}


void psi_rep_hyb3r::getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end) {
//void getPlainPsiValueBuffer_1(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {
	size_t i;
	for (i=ini; i<=end;i++)
		*buffer++ = getPsiValue(i);
	return;
}


void psi_rep_hyb3r::getPsiValueBuffer(uint *buffer, size_t ini, size_t end) {	
//void getPlainPsiValueBuffer(PlainPsi *cPsi, uint *buffer, size_t ini, size_t end) {

	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ((ini < this->inioffset[PRE]) && (end >=this->inioffset[PRE])) {   
		//getPsiR3HValueBuffer_1 (cPsi, buffer,ini,end);
		//getHuffmanPsiR3ValueBuffer(&(this->hcPsiS), buffer                       , ini , this->n/3-1);
		//getHuffmanPsiR3ValueBuffer(&(this->hcPsiP), &buffer[this->inioffset[PRE]], 0   , this->n/3-1);
		//getHuffmanPsiR3ValueBuffer(&(this->hcPsiO), &buffer[this->inioffset[OBJ]], 0   , end-this->inioffset[OBJ]);
		
		this->hcPsiS->getPsiValueBuffer(buffer                       , ini , this->n/3-1);
		this->hcPsiP->getPsiValueBuffer(&buffer[this->inioffset[PRE]], 0   , this->n/3-1);
		this->hcPsiO->getPsiValueBuffer(&buffer[this->inioffset[OBJ]], 0   , end-this->inioffset[OBJ]);
		
		return;
	}



	
	//regular access (sequence of values for a SUBJECT, PRED, or OBJECT.

//	uint range = ini/this->rangeLen;
//	ini-=this->inioffset[range]; end -=this->inioffset[range];
//	return this->hcPsi[range]->getPsiValueBuffer(buffer,ini,end);
//
//
///*FARIRANGES
	if (ini < this->inioffset[PRE]) {
		//return getHuffmanPsiR3ValueBuffer(&(this->hcPsiS), buffer,ini,end);
		this->hcPsiS->getPsiValueBuffer(buffer,ini,end);
	}
	else if (ini < this->inioffset[OBJ]) {
		ini-=this->inioffset[PRE]; end -=this->inioffset[PRE];
		//return getHuffmanPsiR3ValueBuffer(&(this->hcPsiP), buffer,ini,end);
		this->hcPsiP->getPsiValueBuffer(buffer,ini,end);
	}
	else {
		ini-=this->inioffset[OBJ]; end -=this->inioffset[OBJ];
		//return getHuffmanPsiR3ValueBuffer(&(this->hcPsiO), buffer,ini,end);
		this->hcPsiO->getPsiValueBuffer(buffer,ini,end);

	}
///*/



	//FALTA IMPLEMENTAR BEN, podría quitar if inicial (caso vvv) !!
	//FALTA IMPLEMENTAR BEN, podría quitar if inicial (caso vvv) !!
	//FALTA IMPLEMENTAR BEN, podría quitar if inicial (caso vvv) !!
	//FALTA IMPLEMENTAR BEN, podría quitar if inicial (caso vvv) !!
	//FALTA IMPLEMENTAR BEN, podría quitar if inicial (caso vvv) !!
	//FALTA IMPLEMENTAR BEN, podría quitar if inicial (caso vvv) !!
			
	return;
}





//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst   value >= fst  and <=sec
// sets in i2 de position in buffer of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found

int psi_rep_hyb3r::getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindPsiR3HValueBuffer(CompressedPsiR3H *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){

	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ( ((ini < this->inioffset[PRE]) && (end >=this->inioffset[PRE]) ) ||
		 ((ini < this->inioffset[OBJ]) && (end >=this->inioffset[OBJ]) )    ) {   
		printf("\n should not be here!! @getfindPsiR3HValueBuffer... exiting\n");
	}
	
	
	if (ini < this->inioffset[PRE]) {
		//return getfindHuffmanPsiR3ValueBuffer(&(this->hcPsiS), buffer,ini,end,fst,sec,i1,i2);
		return this->hcPsiS->getfindPsiValueBuffer( buffer,ini,end,fst,sec,i1,i2);
	}
	else if (ini < this->inioffset[OBJ]) {
		ini-=this->inioffset[PRE]; end -=this->inioffset[PRE];
		//return getfindHuffmanPsiR3ValueBuffer(&(this->hcPsiP), buffer,ini,end,fst,sec,i1,i2);
		return this->hcPsiP->getfindPsiValueBuffer( buffer,ini,end,fst,sec,i1,i2);

	}
	else {
		ini-=this->inioffset[OBJ]; end -=this->inioffset[OBJ];
		//return getfindHuffmanPsiR3ValueBuffer(&(this->hcPsiO), buffer,ini,end,fst,sec,i1,i2);
		return this->hcPsiO->getfindPsiValueBuffer( buffer,ini,end,fst,sec,i1,i2);		
	}
}

//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// sets in i2 de position (from ini on) of the last  value >= fst  and <=sec
// stops if i2 was set
// returns 0 if all values are < fst.
// returns 1 if a value >= fst was found, but no value >sec was found
// returns 2 if a value >= fst was found, and  a value >sec was found

int psi_rep_hyb3r::getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2) {
//int getfindPsiR3HValue(CompressedPsiR3H *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2){

#ifndef NDEBUG 
	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ( ((ini < this->inioffset[PRE]) && (end >=this->inioffset[PRE]) ) ||
		 ((ini < this->inioffset[OBJ]) && (end >=this->inioffset[OBJ]) )    ) {   
		printf("\n should not be here!! @getfindPsiR3HValue... exiting\n");
	}
#endif	
	
	if (ini < this->inioffset[PRE]) {
		//return getfindHuffmanPsiR3Value (&(this->hcPsiS), ini,end,fst,sec,i1,i2);
		return this->hcPsiS->getfindPsiValue(ini,end,fst,sec,i1,i2);
	}
	else if (ini < this->inioffset[OBJ]) {
		ini-=this->inioffset[PRE]; end -=this->inioffset[PRE];
		//return getfindHuffmanPsiR3Value (&(this->hcPsiP), ini,end,fst,sec,i1,i2);
		return this->hcPsiP->getfindPsiValue(ini,end,fst,sec,i1,i2);		
	}
	else {
		ini-=this->inioffset[OBJ]; end -=this->inioffset[OBJ];
		//return getfindHuffmanPsiR3Value (&(this->hcPsiO), ini,end,fst,sec,i1,i2);
		return this->hcPsiO->getfindPsiValue(ini,end,fst,sec,i1,i2);
	}
}



//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// stops if i1 was set
// returns 0 if all values are < fst.
// returns x+1, where x is  the first value >= fst and <=sec was found    (+1 to ensure zero is not returned as a valid value);

ulong psi_rep_hyb3r::getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1) {
//ulong getfindPsiLeftOnlyR3HValue(CompressedPsiR3H *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1){
#ifndef NDEBUG
	//for pattern VVV only!! IMPLEMENTAR Funcion propia para isto ??
	if ( ((ini < this->inioffset[PRE]) && (end >=this->inioffset[PRE]) ) ||
		 ((ini < this->inioffset[OBJ]) && (end >=this->inioffset[OBJ]) )    ) {   
		printf("\n should not be here!! @getfindPsiR3HValue... exiting\n");
	}
#endif


//	uint range = ini/this->rangeLen;
//	ini-=this->inioffset[range]; end -=this->inioffset[range];
//	return this->hcPsi[range]->getfindLeftOnlyPsiValue(ini,end,fst,sec,i1);
//
//
//
///*FARIRANGES
	
	
	if (ini < this->inioffset[PRE]) {
		//return getfindLeftOnlyHuffmanPsiR3Value (&(this->hcPsiS), ini,end,fst,sec,i1);
		return this->hcPsiS->getfindLeftOnlyPsiValue(ini,end,fst,sec,i1);
	}
	else if (ini < this->inioffset[OBJ]) {
		ini-=this->inioffset[PRE]; end -=this->inioffset[PRE];
		//return getfindLeftOnlyHuffmanPsiR3Value (&(this->hcPsiP), ini,end,fst,sec,i1);
		return this->hcPsiP->getfindLeftOnlyPsiValue(ini,end,fst,sec,i1);		
	}
	else {
		ini-=this->inioffset[OBJ]; end -=this->inioffset[OBJ];
		//return getfindLeftOnlyHuffmanPsiR3Value (&(this->hcPsiO), ini,end,fst,sec,i1);
		return this->hcPsiO->getfindLeftOnlyPsiValue(ini,end,fst,sec,i1);		
	}
///	*/
	
}




void psi_rep_hyb3r::psi_save(char *filename) {
//void storePsiR3H(PlainPsi *compressedPsi, char *filename) {
	FILE *file;
	if( (file = fopen(filename, "w")) ==NULL) {
		printf("Cannot open file %s\n", filename);		exit(0);
	}
	size_t write_err;
	uint psitype = PSIREP_HYB3R;
	write_err=fwrite(&(psitype), sizeof(uint)  ,1,file);
	
	write_err=fwrite(  &(this->T) , sizeof(int)   ,1,file);
	write_err=fwrite(  &(this->nS), sizeof(int)   ,1,file);
	write_err=fwrite(  &(this->n) , sizeof(size_t),1,file);
	write_err=fwrite(  &(this->totalMem), sizeof(size_t), 1, file);	

	write_err=fwrite( 	this->inioffset  , sizeof(size_t), 3, file);	
	write_err=fwrite( 	this->minPsivalue, sizeof(size_t), 3, file);	
	fclose(file);
		
	char filenameS[2048], filenameP[2048], filenameO[2048];
	sprintf(filenameS,"%s.%s",filename,"S");
	sprintf(filenameP,"%s.%s",filename,"P");
	sprintf(filenameO,"%s.%s",filename,"O");
	
	this->hcPsiS->psi_save(filenameS);
	this->hcPsiP->psi_save(filenameP);
	this->hcPsiO->psi_save(filenameO);
	//storeHuffmanCompressedPsiR3(&(this->hcPsiS), filenameS);
	//storeHuffmanCompressedPsiR3(&(this->hcPsiP), filenameP);
	//storeHuffmanCompressedPsiR3(&(this->hcPsiO), filenameO);	
}


psi_rep_hyb3r * psi_rep_hyb3r::psi_load (char *filename) {
//PlainPsi loadPlainPsi(char *filename) {	
	//PlainPsi compressedPsi;
	//	//		compressedPsi.samples=NULL;
	//	//		compressedPsi.psi=NULL;



	psi_rep_hyb3r *ret = new psi_rep_hyb3r();

	FILE *file;
	if( (file = fopen(filename,"r"))==NULL ) {
		printf("Cannot read file %s\n", filename); 		exit(0);
	}
	fflush(stdout);fflush(stderr);	
	
	size_t read_err;

	uint psitype;
	read_err=fread(&psitype, sizeof(int), 1, file);
	if (psitype !=PSIREP_HYB3R) {
		perror("\npsi_load failed (wrong type psi_rep_hyb3r):");
		exit(0);
	}

	printf("\n-loading PSI_HYB3R data");
	
	read_err=fread( &(ret->T) , sizeof(int)   , 1,file);
	read_err=fread( &(ret->nS), sizeof(int)   , 1,file);
	read_err=fread( &(ret->n) , sizeof(size_t), 1,file);
	read_err=fread( &(ret->totalMem) , sizeof(size_t), 1,file);

	read_err=fread( ret->inioffset  , sizeof(size_t), 3, file);	
	read_err=fread( ret->minPsivalue, sizeof(size_t), 3, file);		
	fclose(file);			

	char filenameS[2048], filenameP[2048], filenameO[2048];
	sprintf(filenameS,"%s.%s",filename,"S");
	sprintf(filenameP,"%s.%s",filename,"P");
	sprintf(filenameO,"%s.%s",filename,"O");
	ret->hcPsiS = psi_rep::psi_load(filenameS);
	ret->hcPsiP = psi_rep::psi_load(filenameP);
	ret->hcPsiO = psi_rep::psi_load(filenameO);
	
///	ret->hcPsi[0]=ret->hcPsiS;
///	ret->hcPsi[1]=ret->hcPsiP;
///	ret->hcPsi[2]=ret->hcPsiO;
///	ret->rangeLen = ret->inioffset[PRE];
		
	//printf("\n xxxxxxxxxxxxxxxxxxxxx ret->hcPsiS->totalMem= %zu", ret->hcPsiS->totalMem);
	//printf("\n xxxxxxxxxxxxxxxxxxxxx ret->hcPsiP->totalMem= %zu", ret->hcPsiP->totalMem);
	//printf("\n xxxxxxxxxxxxxxxxxxxxx ret->hcPsiO->totalMem= %zu", ret->hcPsiO->totalMem);
	printf("\n");
	fflush(stdout);fflush(stderr);

	//ret->totalMem = ret->hcPsiS.totalMem + ret->hcPsiP.totalMem + ret->hcPsiO.totalMem;
	//ret->totalMem = ret->hcPsiS->getPsiTotalMem() + ret->hcPsiP->getPsiTotalMem() + ret->hcPsiO->getPsiTotalMem();
	printf("  \n::  PSI_HYB3R:: total bytes = %zu\n",ret->totalMem);			

//	return compressedPsi;
	return ret;

}


