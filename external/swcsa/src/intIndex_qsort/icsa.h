/* icsa.h * Copyright (C) 2011, Antonio Fariña and Eduardo Rodriguez, all rights reserved. * * icsa.h: Definition of the functions of the interface "../intIndex/interfaceIntIndex.h" *   that permits to represent a sequence of uint32 integers with an iCSA:  *   An integer-oriented Compressed Suffix Array. *   Such representation will be handled as a "ticsa" data structure, that *   the WCSA self-index will use (as an opaque type) to  *   create/save/load/search/recover/getSize of the representation of the  *   original sequence. *   Suffix sorting is done via q-sort() *  *     * See more details in: * Antonio Fariña, Nieves Brisaboa, Gonzalo Navarro, Francisco Claude, Ángeles Places,  * and Eduardo Rodríguez. Word-based Self-Indexes for Natural Language Text. ACM  * Transactions on Information Systems (TOIS), 2012.  * http://vios.dc.fi.udc.es/indexing/wsi/publications.html * http://www.dcc.uchile.cl/~gnavarro/ps/tois11.pdf	    *  * This library is free software; you can redistribute it and/or * modify it under the terms of the GNU Lesser General Public * License as published by the Free Software Foundation; either * version 2.1 of the License, or (at your option) any later version. * * This library is distributed in the hope that it will be useful, * but WITHOUT ANY WARRANTY; without even the implied warranty of * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU * Lesser General Public License for more details. * * You should have received a copy of the GNU Lesser General Public * License along with this library; if not, write to the Free Software * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA * */#include <stdio.h>#include <fcntl.h>#include <sys/stat.h>#include <time.h>#include <sys/time.h>#include "../intIndexUtils/defValues.h"#include "../utils/bitmap.h"#include "../utils/huff.h"#include "../utils/parameters.h"#ifdef PSI_HUFFMANRLE	#include "../intIndexUtils/psiHuffmanRLE.h"#endif#ifdef PSI_VBYTERLE		#include "../intIndexUtils/psiVbyteRLE.h"#endif#ifdef PSI_GONZALO	#include "../intIndexUtils/psiGonzalo.h"#endif#ifdef PSI_DELTACODES	#include "../intIndexUtils/psiDeltaCode.h"#endif//#ifdef PSI_R3H_GAPS   //Each region encodes values of size [0.. n/3] (subtract the minvalue of the region from the original psi values)//	#define R3H_WITHGAPS	--> defined in psiR3H.h-->psiHuffmanRLER3.h//	#define PSI_R3H//	//#include "../intIndexUtils/psiR3H.h"//#endif#ifdef PSI_R3H	#include "../intIndexUtils/psiR3H.h"#endif#ifdef PSI_PLAIN	#include "../intIndexUtils/psiPlain.h"#endif//@NOLONGER-RGK2023 #include "../libcds/libcdsBasics.h"//@NOLONGER-RGK2023 #include "../libcds/BitSequence.h"//@NOLONGER-RGK2023 #include "../libcds/BitSequenceBuilder.h"#ifdef PSI_R3HYBRID	#include "../intIndexUtils/psiR3Hybrid.h"#endif//@NOLONGER-RGK2023 using namespace std;//@NOLONGER-RGK2023 using namespace cds_static;//NOTES: -DBITMAP_375 AND -DBITMAP_RGK// BOTH TYPES ARE BUILT AND FREED   --> Indexing only once for both types// BUT ONLY ONE OF THEM WILL BE AVAILABLE FOR RANK/SELECT/ACCESS/GETSIZE // typedef struct {	uint nEntries;   //number of entries in each record being kept by the icsa.		uint suffixArraySize;	uint T_Psi;	//COMPILE WITH -DBITMAP_375		uint *D;	bitmap bD;		//COMPILE WITH -DBITMAP_RGK//@NOLONGERRGK-2023	BitSequence *Dmap;	//Table to solve select1_overPredicates with a single lookup	uint *Dselect1Predicates; // keeps explicitly the positions of the 1s in D	                          // for the predicates (lookup vs select1 in this range).	uint rankfirstPredicate;	uint numPredicates;       //to compute Dselect1Predicates size			//	uint T_A;//	uint T_AInv;//	uint *samplesA;//	uint samplesASize;//	uint *BA;//	bitmap bBA;	//	uint *samplesAInv;//	uint samplesAInvSize;//@@	uint displayCSAState;//@@	long displayCSAPrevPosition;	#ifdef PSI_HUFFMANRLE		HuffmanCompressedPsi hcPsi;	#endif		#ifdef PSI_VBYTERLE			VbyteCompressedPsi vbPsi;		#endif	#ifdef PSI_GONZALO	GonzaloCompressedPsi gcPsi;	#endif		#ifdef PSI_DELTACODES	DeltaCompressedPsi dcPsi;	#endif		#ifdef PSI_R3H	CompressedPsiR3H hcPsi;   	#endif		#ifdef PSI_R3HYBRID	CompressedPsiR3Hybrid hcPsi;   	#endif		#ifdef PSI_PLAIN	PlainPsi plPsi;	#endif	//only needed during "parse_parameters".	uint tempNSHUFF;	uint psiSearchFactorJump;  //factor of the T_Psi value.} ticsa;	void printPsiIndexType(uint t) ;  //used only on icsa.c// FUNCTION PROTOTYPES: BUILDING THE INDEX//Creates the ICSA		int buildIntIndex (uint *aintVector, uint n, uint nEntries, char *build_options, void **index ); 	//ticsa *createIntegerCSA (uint **aintVector, uint SAsize, char *build_options);//Returns number of elements in the indexed sequence of integers	int sourceLenIntIndex(void *index, uint *numInts);//Save the index to disk	int saveIntIndex(void *index, char *pathname); //void storeStructsCSA(ticsa *myicsa, char *basename);// Loads the index from disk.	int loadIntIndex(char *pathname, void **index);  //ticsa *loadCSA(char *basename);//  Frees memory		int freeIntIndex(void *index); //uint destroyStructsCSA(ticsa *myicsa);//Returns the size (in bytes) of the index over the sequence of integers.	int sizeIntIndex(void *index, size_t *numBytes); //uint CSA_size(ticsa *myicsa);		// Shows detailed summary info of the self-index (memory usage of each structure)int printInfoIntIndex(void *index, const char tab[]);//finds the range [left-right] in a zone of increasing values of psi[left-right], such //that for any $i \in [left-right] $, psi(i) maps into [tl,tr] int binSearchPsiTarget(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr);int binSearchPsiTarget_samplesFirst(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr);	// returns the rank_1 (D,i)	uint getRankicsa(void *index, uint i);	// returns the select_1 (D,i)	uint getSelecticsa(void *index, uint i);uint getSelecticsa375(void *index, uint i);uint getSelecticsaNext(void *index, uint i, uint curval);uint getSelectTablePredicates(void *index,uint i);void geticsa_select_j_y_j_mas_1 (void *index, uint j, size_t *sj, size_t *sjmas1);		// returns psi(i)	uint getPsiicsa(void *index, uint i);	// returns buffer[] <-- psi[ini..end]	void getPsiBuffericsa(void *index, uint *buffer, size_t ini, size_t end);int getFindPsiBuffericsa(void *index, uint *buffer, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);int getFindPsiicsa(void *index, size_t ini, size_t end, ulong fst, ulong sec, ulong *i1, ulong *i2);	//recovers the sequence of integers kept by the ICSA.void dumpICSASequence(void *index, uint **data, size_t *len);//Number of occurrences of the pattern, and the interval [left,right] in the suffix array.	int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right);	int countIntIndex_Buffer(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right);	uint countCSA(ticsa *myicsa, uint *pattern, uint patternSize, uint *left, uint *right);		// Exponential search	uint countCSABin(ticsa *myicsa, uint *pattern, uint patternSize, uint *left, uint *right);	// Binary search/* Private function prototypes ********************************************/uint parametersCSA(ticsa *myicsa, char *build_options);uint displayCSAFirst(ticsa *myicsa, uint position);uint displayCSANext(ticsa *myicsa);int  SadCSACompare(ticsa *myicsa, uint *pattern, uint patternSize, uint p);int SadCSACompareSecondOn (ticsa *myicsa, uint *pattern, uint patternSize, uint p, uint psiini);void showStructsCSA(ticsa *myicsa);		// For Debugging//@@ int locateIntIndex(void *index, uint *pattern, uint length, ulong **occ, ulong *numocc);//@@ int displayIntIndex(void *index, ulong position, uint *value);//@@ uint A(ticsa *myicsa, uint position);//@@ uint inverseA(ticsa *myicsa, uint offset);