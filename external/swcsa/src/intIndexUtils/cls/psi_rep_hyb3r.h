
#ifndef _psi_rep_hyb3r_H
#define	_psi_rep_hyb3r_H

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

//If R3H_WITHGAPS is defined, the values of each section are diferentially
//encoded with respect to the smallest value of each section.
// #define R3H_WITHGAPS  --> see "r3/psiHuffmanRLE3.h" !/*old */!


/** Hybrid representation for Psi 
 *  Splits Psi into 3 ranges.
 *  A different Psi representation can be used for each part.
 * 
 *  @author Antonio Fariña
 */
 
#define SUB 0
#define PRE 1
#define OBJ 2
 
 
class psi_rep_hyb3r : public psi_rep {
  
public:

	//psi_rep_hyb3r(unsigned int *Psi, size_t psiSize);
	psi_rep_hyb3r(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS, uint *psiTypes);


	virtual ~psi_rep_hyb3r(); 	
  
	virtual unsigned int getPsiValue(size_t position);

	virtual void getPsiValueBuffer(uint *buffer, size_t ini, size_t end);

	virtual int getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);

	virtual int getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);
	
	virtual ulong getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1);

	//for Nieves' statistics
	virtual ulong getPsiSizeBitsUptoPosition(size_t position);

	virtual void psi_save(char *filename);
	
	static psi_rep_hyb3r * psi_load (char *filename);


//protected:  <-- should be protected, but raised problems in load(). ¿?¿?

	/**constructor**/
	psi_rep_hyb3r();
	/** Internal building function, same parameters as the base constructor. */
	//void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS);
	void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS);

	
	
	unsigned int T;			// Periodo de muestreo de PSI
	unsigned int nS;		// Numero de simbolos para Huffman

	size_t n ;  			 //number of entries in each Psi
	size_t inioffset[3];     //initial pos in A  for sub,pre, and obj
	size_t minPsivalue[3];   //Min psi-value for sub,pre, and obj
	
	psi_rep *hcPsiS;
	psi_rep *hcPsiP;
	psi_rep *hcPsiO;
	
//	psi_rep *hcPsi[3];     --> tried this to avoid ifs in getpsivalue (and getfind*) using range (\in {0..2}) = pos/ (psilen/3)
//	unsigned int rangeLen; --> yet it worsened times: IFs are preferred :(




private:
	virtual void getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end);
	
};


#endif
