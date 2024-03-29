/* interfaceIntIndex.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * interfaceIntIndex.h: Interface for an int-based self-index.
 *   Including functions to build, search(count/locate/display), save, load, size,...
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

// FUNCTION PROTOTYPES: SELF-INDEX ON INTEGERS.

int buildIntIndex (uint *aintVector, uint n, uint nEntries, char *build_options, void **index );
		
	//Saves the index to disk
int saveIntIndex(void *index, const char *pathname);

	//Returns number of elements in the indexed sequence of integers
int sourceLenIntIndex(void *index, uint *numInts);

	//Loads the index from disk.
int loadIntIndex(const char *pathname, void **index);
	
	//Frees the memory allocated to the int_index
int freeIntIndex(void *index);

	//Returns the size (in bytes) of the index over the sequence of integers.
int sizeIntIndex(void *index, size_t *numBytes);

	// Shows detailed summary info of the self-index (memory usage of each structure)
int printInfoIntIndex(void *index, const char tab[]);



//finds the range [left-right] in a zone of increasing values of psi[left-right], such 
//that for any $i \in [left-right] $, psi(i) maps into [tl,tr] 
int binSearchPsiTarget(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr);
int binSearchPsiTarget_samplesFirst(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr);

int binSearchPsiPsiTarget(void *index, ulong *left, ulong *right, ulong *numocc, ulong tl, ulong tr);

	// returns the rank_1 (D,i)	
uint getRankicsa(void *index, uint i);
	// returns the select_1 (D,i)	
uint getSelecticsa(void *index, uint i) ;
uint getSelecticsa375(void *index, uint i);
uint getSelecticsaNext(void *index, uint i, uint curval);
uint getSelectTablePredicates(void *index,uint i);
void geticsa_select_j_y_j_mas_1 (void *index, uint j, size_t *sj, size_t *sjmas1);


	// returns psi(i)	
uint getPsiicsa(void *index, uint i);
	// returns buffer[] <-- psi[ini..end]	
void getPsiBuffericsa(void *index, uint *buffer, size_t ini, size_t end);

	//recovers the sequence of integers kept by the ICSA.
void dumpICSASequence(void *index, uint **data, size_t *len);

	
	//Number of occurrences of the pattern, and the interval [left,right] in the suffix array
int countIntIndex(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right);
int countIntIndex_Buffer(void *index, uint *pattern, uint length, ulong *numocc, ulong *left, ulong *right);	
	
	
//int locateIntIndex(void *index, uint *pattern, uint length, ulong **occ, ulong *numocc);	
//int displayIntIndex(void *index, ulong position, uint *value);


