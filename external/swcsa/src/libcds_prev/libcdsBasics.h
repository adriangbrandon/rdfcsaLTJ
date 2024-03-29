/* basics.h
 * Copyright (C) 2005, Rodrigo Gonzalez, all rights reserved.
 *
 * Some preliminary stuff
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

#ifndef _BASICS_H
#define _BASICS_H

#include <sys/types.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include <sstream>
#include <cassert>
#include <stdint.h>

namespace cds_utils
{

	using namespace std;
	
	typedef unsigned char uchar;
	
	typedef unsigned long ulong;

	/** mask for obtaining the first 5 bits */
//@@  const uint mask31 = 0x0000001F;
	const uint mask31lcds = 0x0000001F;

	/** number of bits in a uint */
	const uint Wlcds = 32;
	/** Wlcds-1 */
	const uint Wlcdsminusone = 31;

	/** 2W*/
	const uint WWlcds = 64;

	/** number of bits per uchar */
	const uint bitsMlcds = 8;

	/** number of bytes per uint */
	const uint BWlcds = 4;

	/** number of different uchar values 0..255 */
//@@	const uint size_uchar = 256;

	/** popcount array for uchars */
	const unsigned char __popcount_tablcds[] = {
		0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6, 3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
		3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7, 4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8,
	};

	/** select array for uchars */
	const unsigned char select_tablcds[] = {
		0, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		8, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		7, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
		6, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1, 5, 1, 2, 1, 3, 1, 2, 1, 4, 1, 2, 1, 3, 1, 2, 1,
	};

	/** prev array for uchars */
	const unsigned char prev_tablcds[] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
	};

	/** bits needed to represent a number between 0 and n */
	inline uint bits_lcds(uint n) {
		uint b = 0;
		while (n) { b++; n >>= 1; }
		return b;
	}

	/** reads bit p from e */
	#define bitget_lcds(e,p) ((((e)[(p)/Wlcds] >> ((p)%Wlcds))) & 1)

	/** sets bit p in e */
	inline void bitset_lcds(uint * e, size_t p) {
		e[p/Wlcds] |= (1<<(p%Wlcds));
	}

	/** cleans bit p in e */
	inline void bitclean_lcds(uint * e, size_t p) {
		e[p/Wlcds] &= ~(1<<(p%Wlcds));
	}

	/** uints required to represent n integers of e bits each */
	inline uint uint_len(const uint e, const size_t n) {
								 //+((unsigned long long)e*n%Wlcds>0));
		return ((unsigned long long)e*n+Wlcds-1)/Wlcds;
	}

	/** Retrieve a given index from array A where every value uses len bits
	 * @param A Array
	 * @param len Length in bits of each field
	 * @param index Position to be retrieved
	 */
	inline uint get_field_lcds(const uint *A, const size_t len, const size_t index) {
		if(len==0) return 0;
		size_t i=index*len/Wlcds, j=index*len-Wlcds*i;
		uint result;
		if (j+len <= Wlcds)
			result = (A[i] << (Wlcds-j-len)) >> (Wlcds-len);
		else {
			result = A[i] >> j;
			result = result | (A[i+1] << (WWlcds-j-len)) >> (Wlcds-len);
		}
		return result;
	}

	/** Store a given value in index into array A where every value uses len bits
	 * @param A Array
	 * @param len Length in bits of each field
	 * @param index Position to store in
	 * @param x Value to be stored
	 */
	inline void set_field_lcds(uint *A, const size_t len, const size_t index, const uint x) {
		if(len==0) return;
		size_t i=index*len/Wlcds, j=index*len-i*Wlcds;
		uint mask = ((j+len) < Wlcds ? ~0u << (j+len) : 0)
			| ((Wlcds-j) < Wlcds ? ~0u >> (Wlcds-j) : 0);
		A[i] = (A[i] & mask) | x << j;
		if (j+len>Wlcds) {
			mask = ((~0u) << (len+j-Wlcds));
			A[i+1] = (A[i+1] & mask)| x >> (Wlcds-j);
		}
	}

	/** Retrieve a given bitsequence from array A
	 * @param A Array
	 * @param ini Starting position
	 * @param fin Retrieve until end-1
	 */
	inline uint get_var_field_lcds(const uint *A, const size_t ini, const size_t fin) {
		if(ini==fin+1) return 0;
		size_t i=ini/Wlcds, j=ini-Wlcds*i;
		uint result;
		uint len = (uint)(fin-ini+1);
		if (j+len <= Wlcds)
			result = (A[i] << (Wlcds-j-len)) >> (Wlcds-len);
		else {
			result = A[i] >> j;
			result = result | (A[i+1] << (WWlcds-j-len)) >> (Wlcds-len);
		}
		return result;
	}

	/** Stores a given bitsequence into array A
	 * @param A Array
	 * @param ini Starting position
	 * @param fin Store until end-1
	 * @param x Value to be stored
	 */
	inline void set_var_field_lcds(uint *A, const size_t ini, const size_t fin, const uint x) {
		if(ini==fin+1) return;
		uint i=ini/Wlcds, j=ini-i*Wlcds;
		uint len = (fin-ini+1);
		uint mask = ((j+len) < Wlcds ? ~0u << (j+len) : 0)
			| ((Wlcds-j) < Wlcds ? ~0u >> (Wlcds-j) : 0);
		A[i] = (A[i] & mask) | x << j;
		if (j+len>Wlcds) {
			mask = ((~0u) << (len+j-Wlcds));
			A[i+1] = (A[i+1] & mask)| x >> (Wlcds-j);
		}
	}

	/** Retrieve a given index from array A where every value uses 4 bits
	 * @param A Array
	 * @param index Position to be retrieved
	 */
	inline uint get_field4_lcds(const uint *A, const size_t index) {
		size_t i=index/8, j=(index&0x7)<<2;
		return (A[i] << (28-j)) >> (28);
	}

	/** Counts the number of 1s in x */
	inline uint popcount_lcds(const int x) {
		return __popcount_tablcds[(x >>  0) & 0xff]  + __popcount_tablcds[(x >>  8) & 0xff]
			+ __popcount_tablcds[(x >> 16) & 0xff] + __popcount_tablcds[(x >> 24) & 0xff];
	}

	/** Counts the number of 1s in the first 16 bits of x */
	inline uint popcount16_lcds(const int x) {
		return __popcount_tablcds[x & 0xff]  + __popcount_tablcds[(x >>  8) & 0xff];
	}

	/** Counts the number of 1s in the first 8 bits of x */
	inline uint popcount8_lcds(const int x) {
		return __popcount_tablcds[x & 0xff];
	}

};

#include "cppUtils.h"
#endif							 /* _BASICS_H */
