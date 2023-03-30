
#ifndef _PSI_REP_PLAIN_RLE_H
#define	_PSI_REP_PLAIN_RLE_H

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
#include <stdlib.h>
#include <stdio.h>
//#include <malloc.h>
#include <assert.h>
#include "../../utils/huff.h"
#include "../../utils/basics.h"

#include "psi_rep.h"


/* About this Version:
 *   Psi divided into 3 regions from RDFCSA.
 *   Version psiR3H: 
 *   psiHuffmanRLER3c: differs only with psiHuffmanRle.c in the fact that
 *     the absolute samples must know have values greater than psiSize in
 *     the case or ranges for Subjects and Predicates, since all the ranges
 *     have n/3, n/3, and n/3+1 entries respectively, and the values of the
 *      range of Subjects  has values in [n/3, 2n/3-1]
 *      range of Predicate has values in [2n/3, n-1]
 *      range of Objects   has values in [0, n-1]
 *     due to that, the number of bits required must be computed over the 
 *    input array of values (line 200 aprox): sampleSize = bits(maxval3R);
 */


//If R3H_WITHGAPS is defined, the values of each section are diferentially
//encoded with respect to the smallest value of each section.
//#define R3H_WITHGAPS   <see icsa.h>
#ifdef PSI_R3H_GAPS
//	#define R3H_WITHGAPS	//--> defined in psiR3H.h <-- icsa.h 
#endif

//#define PSI_PLAIN_LOG_BASED

//	#define R3H_WITHGAPS     //all psi values to be decreased in "OFFSET = min-value-in-psi" 
                             //makes only sense if PSI_PLAIN_LOG_BASED IS DEFINED 

/** Plain + Rle representation for Psi 
 *  It uses the options (#defines) above.  
 * 
 *  @author Antonio Fariña
 */
 
class psi_rep_plain_rle : public psi_rep {
  
public:

	//psi_rep_plain_rle(unsigned int *Psi, size_t psiSize);
	//psi_rep_plain_rle(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS);
	psi_rep_plain_rle(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T);
	psi_rep_plain_rle(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS); // { //, uint *psiTypes=NULL){

	virtual ~psi_rep_plain_rle();
 	
  
	virtual unsigned int getPsiValue(size_t position);

	virtual void getPsiValueBuffer(uint *buffer, size_t ini, size_t end);

	virtual int getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);

	virtual int getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);
	
	virtual ulong getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1);

	//for Nieves' statistics
	virtual ulong getPsiSizeBitsUptoPosition(size_t position);


	virtual void psi_save(char *filename);
	
	static psi_rep_plain_rle * psi_load (char *filename);

	//unsigned long  getPsiTotalMem();




//protected:  <-- should be protected, but raised problems in load(). 
	/**constructor**/
	psi_rep_plain_rle();
	/** Internal building function, same parameters as the base constructor. */
	//void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T);
	//void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS);
	void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS);
	
	unsigned int T;					// Periodo de muestreo de PSI
	//THuff diffsHT;					// Arbol de Huffman (codifica stream)
	//unsigned int nS;				// Numero de simbolos para Huffman
	
#ifdef 	R3H_WITHGAPS
	size_t OFFSET;                  //INIOFFSET OF THE ZONE OF PSI REPRESENTED BY THIS COMPRESSED REPRESENTATION
#endif

	/**/	//unsigned int numberOfSamples;
	size_t numberOfSamples;

	unsigned int sampleSize;		// Bits que ocupa cada mostra
	unsigned int *samples;			// Vector de mostras
	
	unsigned int pointerSize;		// Bits que ocupa cada punteiro	
/**/	size_t *samplePointers;	// Punteiros das mostras a stream

/**/	size_t streamSize;		// Bits que ocupa stream		
	unsigned int *stream;			// Secuencia Huffman + RLE

///**/	size_t totalMem;			// the size in bytes used;

	
	unsigned int L;
	unsigned int LL;
	unsigned int Lpsi;
	

private:
	virtual void getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end);
	
};


#endif
