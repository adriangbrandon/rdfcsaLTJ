/* psiHuffmanRLE.h
 * Copyright (C) 2011, Antonio Fariña and Eduardo Rodriguez, all rights reserved.
 * Improved representation based on a previous version from Gonzalo Navarro's.
 *
 * psiHuffmanRLE.h: Compressed Representation for Psi in CSA.
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
 *   Version **psiR3H**: 
 *   Psi is divided into three ranges:
 *    - range of Subjects  is Ps= Psi[0, n-1]
 *    - range of Predicate is Pp= Psi[n/3, 2n/3-1]
 *    - range of Objects   is Po= Psi[2n/3, n-1+1]
 *   They are compressed separately using psiHuffmanRLER3.c, actually
 *   using three HuffmanCompressedPsiR3 structs: 
 *       hcPsiS, hcPsiP, and hcPsiO respectively
 *   The operations in this file (psiR3H) are delegated to the proper 
 *   structure, depending on the position of Psi we want to access.
 */


#include <stdlib.h>
#include <stdio.h>
//#include <malloc.h>
#include <assert.h>

//
//If R3H_WITHGAPS is defined, the values of each section are diferentially
//encoded with respect to the smallest value of each section.
// #define R3H_WITHGAPS  --> see "r3/psiHuffmanRLE3.h" !

#include "./r3/psiHuffmanRLER3.h"
//#include "./r3/psiPlainRLER3.h" 

#define SUB 0
#define PRE 1
#define OBJ 2


// ESTRUCTURA DE PSI COMPRIMIDA
typedef struct {
	unsigned int T;					// Periodo de muestreo de PSI
	unsigned int nS;				// Numero de simbolos para Huffman

	size_t n ;  //number of entries in each Psi
	size_t inioffset[3];     //initial pos in A  for sub,pre, and obj
	size_t minPsivalue[3];   //Min psi-value for sub,pre, and obj
	
	HuffmanCompressedPsiR3 hcPsiS;
	HuffmanCompressedPsiR3 hcPsiP;
	HuffmanCompressedPsiR3 hcPsiO;

/**/	size_t totalMem;			// the size in bytes used;
} CompressedPsiR3H;


// PROTOTIPOS DE FUNCIÓNS

//	Crea as estructuras de Psi comprimida:
//	
//	Psi: Funcion Psi original
//	psiSize: Numero de elementos de Psi
//	T: Periodo de muestreo en Psi
//	nS: Numero de simbolos que se utilizaran no arbol de Huffman
//
//	Devolve unha estructura CompressedPSI que representa a Psi comprimida
CompressedPsiR3H CompressPsiR3H(unsigned int *Psi, size_t psiSize, unsigned int T, unsigned int nS);

//	Obtén un valor de Psi
//
//	cPsi: A estructura que representa a Psi comprimida
//	position: A posicion da que queremos obter o valor de Psi
unsigned int getPsiR3HValue(CompressedPsiR3H *cPsi, size_t position);

void getPsiR3HValueBuffer(CompressedPsiR3H *cPsi, uint *buffer, size_t ini, size_t end);

//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst value greater than fst
// sets in i2 de position in buffer of the fst value greater than sec
// stops if i2 was set
// returns 1 normally if the searched values were found. 0 if no values where found.
int getfindPsiR3HValueBuffer(CompressedPsiR3H *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);
int getfindPsiR3HValue(CompressedPsiR3H *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);


//simulates decompression from ini to end, and during the process:
// sets in i1 de position (from ini on) of the fst   value >= fst  and <=sec
// stops if i1 was set
// returns 0 if all values are < fst.
// NO LONGER returns 1 if a value >= fst and <=sec was found
// returns x+1, where x is  the first value >= fst and <=sec was found    (+1 to ensure zero is not returned as a valid value);

ulong getfindPsiLeftOnlyR3HValue(CompressedPsiR3H *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1);


void storePsiR3H(CompressedPsiR3H *compressedPsi, char *filename);
CompressedPsiR3H loadPsiR3H(char *filename);

//frees the memory used.	
void destroyPsiR3H(CompressedPsiR3H *compressedPsi);


//for Nieves' statistics
unsigned long  getPsiR3HSizeBitsUptoPosition(CompressedPsiR3H *cPsi, size_t position);
	           
	
