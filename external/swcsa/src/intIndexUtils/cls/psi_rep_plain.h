
#ifndef _PSI_REP_PLAIN_H
#define	_PSI_REP_PLAIN_H

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


/*

Compressing PSI with Huffman over gap-encoded values and RLE for the 1-runs.

Assuming that the maximum length of a run is R-1 (R is the sampling step in PSI)
We use a Huffman tree built over N symbols: Huffman codes are assigned to 
 the following 4 groups of gaps:

G1: Frequent "short positive values": gap values between 2 and 
    N - 32 <for negative> - 32 <for large positive> - (R -1)
G2: R-1 Huffman codes are enought to represent RUNS and their length.
G3: 32 Huffman codes are used to represent negative values. The 32 codes are needed
    not only to mark that they are negative values $V$, but to indicate also 
    the number of bits needed to represent abs($V$). 
G4: Similarly to G3. 32 Huffman codes are used to represent large positive numbers, 
	that is, numbers > those in G1.

From those groups (after decoding a huffman code)
 - values from G1 are directly obtained.
 - values from G2 are converted into a RUN of the len specified by the Huffman code. 
 - values from G3 are obtained by reading the K-bit binary representation of the number
   that follows the decoded codeword. (the Huffman codes especifies that it is a
   negative number, whose abs_value requires K bits (in binary).
 - values from G4 are obtained as those in G3, yet now re binary representation 
   includes the number itself rather than "abs(number)".
*/

/* options:
	- PSI_PLAIN_LOG_BASED not defined
	   * Representes Psi as an array of uint32 integers. No compression

	- PSI_PLAIN_LOG_BASED is defined :  R3H_WITHGAPS is not defined
	   * Representes Psi as a compacted array of log_k-bit integers. 
			* 	k = bits (max_value in Psi)

	- PSI_PLAIN_LOG_BASED is defined :  R3H_WITHGAPS is also defined
	   * Representes Psi as a compacted array of log_k-bit integers. 
	   * The values in Psi are substract the min-value-in-psi
			* 	k = bits (max_value in Psi-min_value in Psi)   
			*   i.e. all the values are in the range [0.. n-1]
	   
*/

//#define PSI_PLAIN_LOG_BASED

//	#define R3H_WITHGAPS     //all psi values to be decreased in "OFFSET = min-value-in-psi" 
                             //makes only sense if PSI_PLAIN_LOG_BASED IS DEFINED 

/** Plain representation for Psi 
 *  It uses the options (#defines) above.  
 * 
 *  @author Antonio Fariña
 */
 
class psi_rep_plain : public psi_rep {
  
public:

	//psi_rep_plain(unsigned int *Psi, size_t psiSize);
	psi_rep_plain(unsigned int *Psi, size_t psiSize, size_t inioffset);
	psi_rep_plain(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS, uint *psiTypes);	


	virtual ~psi_rep_plain(); 	
  
	virtual unsigned int getPsiValue(size_t position);

	virtual void getPsiValueBuffer(uint *buffer, size_t ini, size_t end);

	virtual int getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);

	virtual int getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);
	
	virtual ulong getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1);

	//for Nieves' statistics
	virtual ulong getPsiSizeBitsUptoPosition(size_t position);


	virtual void psi_save(char *filename);
	
	static psi_rep_plain * psi_load (char *filename);

	//unsigned long  getPsiTotalMem();


//protected:  <-- should be protected, but raised problems in load(). 
	
	/**constructor**/
	psi_rep_plain();
	/** Internal building function, same parameters as the base constructor. */
	//void build(unsigned int *Psi, size_t psiSize, size_t inioffset);
	void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS);
	
	
	
	size_t n;
	unsigned int sampleSize;		// Bits que ocupa cada mostra
	
	#ifndef PSI_PLAIN_LOG_BASED
		//PSI as a uint32 array
		uint *psi;
	#endif
	#ifdef PSI_PLAIN_LOG_BASED
		//PSI compressed as k-bit array
		size_t *samples;			    // Psi, comprimido a k-bits (sampleSize) por elmento
		size_t OFFSET;
	#endif	

private:
	virtual void getPsiValueBuffer_1(uint *buffer, size_t ini, size_t end);
	
};


#endif
