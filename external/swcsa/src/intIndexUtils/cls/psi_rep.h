

#ifndef _PSI_REP_H
#define	_PSI_REP_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "../../utils/basics.h"

// types for constructor (using "uint psiTypes[3]" parameter in constructors)
// also used at psi_load/ psi_save functions.

#define PSIREP_HUFFMAN_RLE   1  //default option 
#define PSIREP_PLAIN      	 2
#define PSIREP_PLAIN_RLE  	 3


#define PSIREP_HYB3R   9

#define DEFAULT_T_PSI_VALUE 16  //FOR HUFFMAN_RLE

#ifndef PSI_PLAIN_LOG_BASED 
#define PSI_PLAIN_LOG_BASED 
#endif

/** Base class for Psi representation, contains many abstract functions, so this can't
 *  be instantiated. 
 *  It includes static load() function.
 * 
 *  @author Antonio Fariña
 */
class psi_rep {
  
public:
	virtual ~psi_rep() {};

	virtual void build(unsigned int *Psi, size_t psiSize, size_t inioffset, unsigned int T, unsigned int nS) =0;

	virtual unsigned int getPsiValue(size_t position) =0;

	virtual void getPsiValueBuffer(uint *buffer, size_t ini, size_t end)=0;

	virtual int getfindPsiValueBuffer(uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2)=0;
	
	virtual int getfindPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2)=0;
	
	virtual ulong getfindLeftOnlyPsiValue(size_t ini, size_t end, ulong fst, ulong sec, ulong *i1)=0;

	//for Nieves' statistics
	virtual ulong getPsiSizeBitsUptoPosition(size_t position)=0;


	virtual void psi_save(char *filename)=0;
	
	static psi_rep * psi_load (char *filename);

	unsigned long  getPsiTotalMem();


//protected:
	size_t totalMem;
	

/*
	//2023- dual  (called from BuildFacade:: get_one_dual_init() and get_one_dual_next().
	// allow to sequentially recover \Psi[i], i \in [l,r].
	// decodes data into a buffer of size [tpsi], and returns values from the buffer
	// when a buffer gets empty, the next tpsi values (are loaded into the buffer),
	// and the process continues.
	// After \Psi[r] is returned, get_one_psi_next() must return NO_MORE_VALUES_IN_PSI_RANGE;

	//initilization, and loading of the 1st buffer (returns no uint value).
	virtual void get_one_psi_init(uint *buffer, size_t l, size_t r);
	
	//returns an uint c. NO_MORE_VALUES_IN_PSI_RANGE if \Psi[r] was already returned previousle (interval exhausted).
	virtual uint get_one_psi_next();  
	
	//for supporting get_one_psi_init(buff,l,r) and get_one_psi_next();
	size_t state_psi_l, state_psi_r;
	size_t state_psi_pos;  //pos within buffer
	uint *buffer;
	uint nb;   //number of uints in buffer
*/
	
};
	
	
#include "psi_rep_plain.h"	
#include "psi_rep_plain_rle.h"	
#include "psi_rep_huffman_rle.h"	
#include "psi_rep_hyb3r.h"	
	
#endif
