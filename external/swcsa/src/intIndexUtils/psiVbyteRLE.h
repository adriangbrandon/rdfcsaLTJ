/* psiVbyteRLE.h
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
#include <stdlib.h>
#include <stdio.h>
//#include <malloc.h>
#include <assert.h>

#include "../utils/basics.h"
#include "../utils/bitmap.h"
#include "../utils/huff.h"
#include "../utils/vbyte.h"

/*
Compressing PSI with vbyte over gap-encoded values and RLE for the 1-runs.
*/


/**************************************************************************************/

#define MAX_RUN 16

typedef struct {
	unsigned int T;		 // Periodo de muestreo de PSI
	size_t psiSize;
	
	size_t maxrun;
	size_t negoffset;

	uint numberOfSamples;
	unsigned int sampleSize;		// Bits que ocupa cada mostra
	size_t *samples;			// Vector de mostras
	unsigned int pointerSize;		// Bits que ocupa cada punteiro	
	size_t *samplePointers;	// Punteiros das mostras a stream

	size_t bcsize;
	byte *bc;
		
	size_t bitsbcsize;   
	size_t bitspsiSize;	
	size_t totalMem;			// the size in bytes used;

} VbyteCompressedPsi;


// PROTOTIPOS DE FUNCIÓNS

//	Crea as estructuras de Psi comprimida:
//	
//	Psi: Funcion Psi original
//	psiSize: Numero de elementos de Psi
//	T: Periodo de muestreo en Psi
//	nS: Numero de simbolos que se utilizaran no arbol de Huffman
//
//	Devolve unha estructura CompressedPSI que representa a Psi comprimida

VbyteCompressedPsi vbyteCompressPsi(unsigned int *Psi, size_t psiSize, unsigned int T);//, unsigned int nS);


//	Obtén un valor de Psi
//
//	cPsi: A estructura que representa a Psi comprimida
//	position: A posicion da que queremos obter o valor de Psi
unsigned int getVbytePsiValue(VbyteCompressedPsi *cPsi, size_t position);

void getVbytePsiValueBuffer(VbyteCompressedPsi *cPsi, uint *buffer, size_t ini, size_t end);

//uncompresses a buffer from ini to end, and during the process:
// sets in i1 de position in buffer of the fst value greater than fst
// sets in i2 de position in buffer of the fst value greater than sec
// stops if i2 was set
// returns 1 normally if the searched values were found. 0 if no values where found.
int getfindVbytePsiValueBuffer(VbyteCompressedPsi *cPsi, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);
int getfindVbytePsiValue(VbyteCompressedPsi *cPsi, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);



void storeVbyteCompressedPsi(VbyteCompressedPsi *compressedPsi, char *filename);
VbyteCompressedPsi loadVbyteCompressedPsi(char *filename, bitmap bD);

//frees the memory used.	
void destroyVbyteCompressedPsi(VbyteCompressedPsi *compressedPsi);

//for Nieves' statistics
unsigned long  getVbytePsiSizeBitsUptoPosition(VbyteCompressedPsi *cPsi, size_t position);

unsigned long  getTotalMem(VbyteCompressedPsi *cPsi);
	
	
