
/* bitmap.h
 * Copyright (C) 2011, Antonio Fariña, Eduardo Rodríguez all rights reserved.
 * Original version from pizza-chili code.
 *
 * bitmap.h: Basic operations over a bitmap using 37.5% approach.
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

// Implements operations over a bitmap

#ifndef BITMAPINCLUDED
#define BITMAPINCLUDED

#include "basics.h"

typedef struct sbitmap
   { uint *data;
     uint n;        // # of bits
     uint pop;	    // # bits set
     uint *sdata;   // superblock counters
     uint sSize;	//		size of sdata vector
     byte *bdata;   // block counters
     uint bSize; 	//     size of bdata vector

     uint *ones;   //structura agilizar selects en paper supercomputing   (sección 3.1.1) --> tamaño 0.375 --> 0.500
     uint nblocks; //structura agilizar selects en paper supercomputing

     uint mem_usage;
   } *bitmap;


	// creates a bitmap structure from a bitstring, which gets owned
bitmap createBitmap (uint *string, uint n);
	// rank(i): how many 1's are there before position i, not included
uint rank_1 (bitmap B, uint i);
	// select(i): position of i-th 1
uint bselect (bitmap B, uint i);
uint bselect_orig (bitmap B, uint i);
	// destroys the bitmap, freeing the original bitstream
void destroyBitmap (bitmap B);
	// popcounts 1's in x
uint popcount (register uint x);

void saveBitmap (char *filename, bitmap b);
bitmap loadBitmap (char *filename, uint *string, uint n);


	// returns the size, in bytes, of the bitmap
uint sizebytesBitmap (bitmap B);



////EDU'S functions included here.
//bitmap createBitmapEdu (uint *string, uint n);
//uint popcountEdu (register uint x);     //which is identical to popcount.
//uint rank1Edu(bitmap B, unsigned int position);
//unsigned int isActiveBit(uint *V, uint position);
void showBitVector(uint * V, int vectorSize);


//para next1_ana  //FINALMENTE NO USADO
const unsigned char next1_b[256] = {8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0};
size_t getNext1 (uint *b, size_t pos, size_t posLimTriple);


void bselect_j_y_j_mas_1 (bitmap B, uint j, size_t *sj, size_t *sjmas1);

#endif


