
/* bitmap.c
 * Copyright (C) 2011, Antonio Fariña, Eduardo Rodríguez all rights reserved.
 * Original version from pizza-chili code.
 *
 * bitmap.c: Basic operations over a bitmap using 37.5% approach.
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

#include "bitmap.h"


  // In theory, we should have superblocks of size s=log^2 n divided into
  // blocks of size b=(log n)/2. This takes 
  // O(n log n / log^2 n + n log log n / log n + log n sqrt n log log n) bits
  // In practice, we can have any s and b, and the needed amount of bits is
  // (n/s) log n + (n/b) log s + b 2^b log b bits
  // Optimizing it turns out that s should be exactly s = b log n
  // Optimizing b is more difficult but could be done numerically.
  // However, the exponential table does no more than popcounting, so why not
  // setting up a popcount algorithm tailored to the computer register size,
  // defining that size as b, and proceeding.

//unsigned char OnesInByte[] = 
const unsigned char popc[] =   //number of ones in one byte value [0..255].
{
0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6,3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,
3,4,4,5,4,5,5,6,4,5,5,6,5,6,6,7,4,5,5,6,5,6,6,7,5,6,6,7,6,7,7,8,
};

uint popcount (register uint x)

   { return popc[x&0xFF] + popc[(x>>8)&0xFF] + popc[(x>>16)&0xFF] + popc[x>>24];
   }

  
/******************************************************************/
// FUNCIONS DE EDU ...
/******************************************************************/
/* 
	Creates a bitmap and structures to rank and select 
*/

//bitmap createBitmapEdu (uint *string, uint n){  return createBitmap(string,n);}

bitmap createBitmap (uint *string, uint n){
    bitmap B;
    
	unsigned int nb;
	unsigned int ns;
	unsigned int countB, countS, blockIndex, superblockIndex;
    register unsigned int block;

	B =(struct sbitmap *) malloc (sizeof(struct sbitmap));
	B->data = string;
	B->n = n; 
	ns = (n/256)+1;
	nb = (n/32)+1;

    B->bSize = nb;
    B->sSize = ns;
	B->bdata =(byte *)malloc(nb*sizeof(byte));  //	Db = (unsigned char *)malloc(nb*sizeof(unsigned char));
	B->sdata = (uint*)malloc(ns*sizeof(uint));  // 	Ds = (unsigned int *)malloc(ns*sizeof(unsigned int));

	B->mem_usage = (ns*sizeof(uint)) + (nb*sizeof(byte)) + (sizeof(struct sbitmap));
	/* Ahora construimos los bloques */
    blockIndex = 0;
    superblockIndex = 0;
    countB = 0;
    countS = 0;

    while(blockIndex < nb) {
    	
       if(!(blockIndex%8)) {
          countS += countB;
          B->sdata[superblockIndex++] = countS;
          countB = 0;
       }
       
       B->bdata[blockIndex] = countB;
	   block = string[blockIndex++];
	   
	   countB += popcount(block);
    }

	B->pop = countS+countB;
	
//	{int i;     //fprintf(stderr,"\n");
//     for (i=0;i<ns;i++) {//fprintf(stderr,"%d ",B->sdata[i]);
//     	}
//     //fprintf(stderr,"\n");
//     for (i=0;i<8;i++) {//fprintf(stderr,"%d ",B->bdata[i]);
//     	}
//	}


// FARI 2023... faltaba añadir esto 
	//structura para agilizar selects en supercomputing
	B->nblocks=(B->n-1)/32;
	//Compute ones
	B->ones = (uint *) malloc((B->pop/256 + 3) * sizeof(uint));
	int i;
	for (i = 0; i <= (B->pop+255)/256; i++) {
		B->ones[i] = bselect_orig(B,i*256+1);
	}
	B->ones[i] = B->n;

	B->mem_usage = (sizeof(uint) * B->sSize) + (sizeof(byte) * B->bSize) + sizeof(uint)*(B->pop/256+2) + (sizeof(struct sbitmap));

// end fari 2023



    return B;
}	


/*
  Number of 1s in range [0,posicion]
*/
//uint rank1Edu(bitmap B, unsigned int position) {
//uint rank1Edu(bitmap B, unsigned int position) { return rank(B,position);}
uint rank_1(bitmap B, unsigned int position) {
    register unsigned int block;    
    if (position > B->n) return B->pop;    
	//position -=1;
	
	block = B->data[position/32] << (31-position%32);

    return B->sdata[position/256] + B->bdata[position/32] + 
           popc[block & 0xff] + popc[(block>>8) & 0xff] +
           popc[(block>>16) & 0xff] + popc[block>>24];
}   


/********************************************************************************************/
/**************************************************************************************/

#include "sel_b.h"


static uint binsearch (uint *data, uint size, uint val)

   { uint i,j,m;
     i = 0; j = size;
     while (i+1 < j)
	{ m = (i+j)/2;
	  if (data[m] >= val) j = m;
	  else i = m;
	}
     return i;
   }

static uint binsearch_aux (uint *data, uint left, uint right, uint val)

   { uint i,j,m;
     i = left; j = right;
     while (i+1 < j)
	{ m = (i+j)/2;
	  if (data[m] >= val) j = m;
	  else i = m;
	  //fprintf(stderr, "bs %d %d %d %d\n", i, j, m, data[m], val);
	}
     return i;
   }

uint bselect_orig (bitmap B, uint j)

   { uint spos,bpos,pos,word,x;
     byte *blk;
     if (j > B->pop) return B->n;
     spos = binsearch(B->sdata,(B->n+256-1)/256,j);

     //fprintf(stderr, "sb %d\n", spos);
     
     //fprintf(stderr,"\n SPOS IS %d, and B->sdata[pos] = %d",spos,B->sdata[spos]);
     j -= B->sdata[spos];
     pos = spos<<8;
     blk = B->bdata + (pos>>5);
     bpos = 0;
    
    //while ((bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
    while ( ((spos*8+bpos) < ((B->n-1)/W)) && (bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
     
    
      //fprintf(stderr,"\n BPOS  = %d",bpos);
     pos += bpos<<5;
     word = B->data[pos>>5];
     j -= blk[bpos];
     //fprintf(stderr,"\n pos>>5 = %d ... pasou XXX con word = %d, and j= %d",pos>>5,word,j);
     while (1) 
    { x = popc[word & ((1<<8)-1)]; 
    	//fprintf(stderr,"\n word = %u popc vale %u",word & ((1<<8)-1),x);
	  if (j <= x) break;
	  j -= x; pos += 8;
	  word >>= 8;
	  
	}

	// select within the last byte   //@@fari
	/*	
		//original option	
     while (j) { if (word & 1) j--; word >>= 1; pos++; }

     //	fprintf(stderr,"\n\nBSELECT::: POSICIÓN FINAL = %u",pos-1);
     return pos-1;
	*/

     	//optimized lookup-based select 
    word &=0xFF;     	
	return pos + sel1_b[word][j];     
   }

uint bselect (bitmap B, uint j)

   { 
     uint spos,bpos,pos,word,x;

     byte *blk;
     if (j > B->pop) return B->n;
     bpos = 0;
     uint p = (j-1)/256;
     if (!((j-1)%256)) return B->ones[p];
     uint l = B->ones[p];
     uint r = B->ones[p+1];
     uint sx = l/256;
     uint sy = r/256; 
	 uint blockIndex = 0;
     
     //fprintf(stderr, "%d %d %d %d\n", l, r, sx, sy);

     uint sxi = sx;
//	 if (sx + 8 < sy) {
//          while(B->sdata[sxi+1] < j) sxi++;
//	  if (sx == sxi) {
//               bpos = (l%256)/32;
//	  }
//          spos = sxi;
//	 } else{
//	      spos = binsearch_aux(B->sdata, sx, sy+1, j);
//	 }
	      spos = binsearch_aux(B->sdata, sx, sy+1, j);

	//fprintf(stderr, "sb %d\n", spos);
//     spos = binsearch(B->sdata,(B->n+256-1)/256,j);
     
     //fprintf(stderr,"\n SPOS IS %d, and B->sdata[pos] = %d",spos,B->sdata[spos]);
     j -= B->sdata[spos];
     pos = spos<<8;
     blk = B->bdata + (pos>>5);
    
    //while ((bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
    while ( (spos*8+bpos < B->nblocks) && bpos < 7 && (blk[bpos+1] < j)) bpos++;
    //while ( ((spos*8+bpos) < ((B->n-1)/W)) && (bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
     
    
      //fprintf(stderr,"\n BPOS  = %d",bpos);
     pos += bpos<<5;
     word = B->data[pos>>5];
     j -= blk[bpos];
     //fprintf(stderr,"\n pos>>5 = %d ... pasou XXX con word = %d, and j= %d",pos>>5,word,j);
     while (1) 
    { x = popc[word & ((1<<8)-1)]; 
    	//fprintf(stderr,"\n word = %u popc vale %u",word & ((1<<8)-1),x);
	  if (j <= x) break;
	  j -= x; pos += 8;
	  word >>= 8;
	  
	}

	// select within the last byte   //@@fari
	/*	
		//original option	
     while (j) { if (word & 1) j--; word >>= 1; pos++; }

     //	fprintf(stderr,"\n\nBSELECT::: POSICIÓN FINAL = %u",pos-1);
     return pos-1;
	*/

     	//optimized lookup-based select 
    word &=0xFF;     	
	return pos + sel1_b[word][j];     
   }


void bselect_j_y_j_mas_1 (bitmap B, uint j, size_t *sj, size_t *sjmas1)   {
	 uint spos,bpos,pos,word,x;
     byte *blk;
     if (j >= B->pop) {
	     if (j > B->pop) *sj=B->n; 
	     else *sj = bselect(B,j);
	     *sjmas1=B->n;return; 
     };
     bpos = 0;

     uint p = (j-1)/256;
     uint l = B->ones[p];
     uint r = B->ones[p+1];
     uint sx = l/256;
     uint sy = r/256; 
	 uint blockIndex = 0;
     
     //fprintf(stderr, "%d %d %d %d\n", l, r, sx, sy);
     //fprintf(stderr, "%d %d %d %d\n", B->n, B->pop, B->sdata[sx], B->sdata[sy]);

     uint sxi = sx;
	// if (sx + 8 < sy) {
        //  while(B->sdata[sxi+1] < j) sxi++;
	//  if (sx == sxi) {
        //       bpos = (l%256)/32;
	//  }
        //  spos = sxi;
	// } else{
	//      spos = binsearch_aux(B->sdata, sx, sy+1, j);
	// }
	spos = binsearch_aux(B->sdata, sx, sy+1, j);

//	fprintf(stderr, "sb %d\n", spos);
	

     //spos = binsearch(B->sdata,(B->n+256-1)/256,j);
     
	 
 //to check if the next one is in the same superblock
 uint jmas1=j+1;
 uint samesbblock = (spos>=(B->sSize-1)) ? 1 : (B->sdata[spos+1]>(jmas1));
 uint nextsbblockinipos = (spos+1)<<8;
 
     //fprintf(stderr,"\n SPOS IS %d, and B->sdata[pos] = %d",spos,B->sdata[spos]);
     j -= B->sdata[spos];
     pos = spos<<8;
     blk = B->bdata + (pos>>5);
     //bpos = 0;
    
    //while ((bpos < (1<<3)-1) && (blk[bpos+1] < j)) bpos++;
    while ( (spos*8+bpos < B->nblocks) && bpos < 7 && (blk[bpos+1] < j)) bpos++;
     
    
      //fprintf(stderr,"\n BPOS  = %d",bpos);
     pos += bpos<<5;

uint *nextuint = B->data +(1+ (pos>>5));
size_t nextpos= pos +32;

     word = B->data[pos>>5];
     j -= blk[bpos];


     
     //fprintf(stderr,"\n pos>>5 = %d ... pasou XXX con word = %d, and j= %d",pos>>5,word,j);
     while (1) 
    { x = popc[word & ((1<<8)-1)]; 
    	//fprintf(stderr,"\n word = %u popc vale %u",word & ((1<<8)-1),x);
	  if (j <= x) break;
	  j -= x; pos += 8;
	  word >>= 8;	  
	}
	// select within the last byte   //@@fari
     	//optimized lookup-based select 
	*sj= pos + sel1_b[word&0xFF][j];
	// PRIMER SELECT COMPLETADO
	
	/////////////7
	//*sjmas1 = bselect(B,jmas1);  
	//return 0;
	//////////////////7
 	
	j++;  //locate select for jmas1 !!
	if (j <= x) {   //same byte
		*sjmas1= pos + sel1_b[word&0xFF][j];
	}
	else {
		
//		*sjmas1 = bselect(B,jmas1); return 0;
		
		if (samesbblock) { //ssame superblock
			int found=0;
			 while (word !=0)    //within the same integer/block ?
			{ x = popc[word & ((1<<8)-1)]; 
				//fprintf(stderr,"\n word = %u popc vale %u",word & ((1<<8)-1),x);
			  if (j <= x) { found=1;break;}
			  j -= x; pos += 8;
			  word >>= 8;	  
			}	
			if (found) {
				*sjmas1= pos + sel1_b[word&0xFF][j];  //within previous integer
			}
			else { //j+1: not within this integer
				
				
//				printf("\n  *********inipos %zu *********\n", (size_t) pos % 256);
//				printf("\n  *********inipos %zu -> %zu*********\n", (size_t) pos, nextsbblockinipos);
//					*sjmas1 = bselect(B,jmas1); return 0;



				j=1;  //next one!
				pos = nextpos;
				//printf("\n *********inipos %zu", (size_t) pos % 256);
				//while (pos < nextsbblockinipos) 
				{
					while(  (*nextuint==0) && (pos <nextsbblockinipos)) {
						nextuint++;
						pos +=32;
					}
					//one must be within *nextuint
					word = *nextuint;
//				printf("\n  *2********inipos %zu -> %zu*********\n", (size_t) pos, nextsbblockinipos);

					 //fprintf(stderr,"\n pos>>5 = %d ... pasou XXX con word = %d, and j= %d",pos>>5,word,j);
					 while (1) 
					{ x = popc[word & ((1<<8)-1)]; 
						//fprintf(stderr,"\n word = %u popc vale %u",word & ((1<<8)-1),x);
					  if (x!=0) break;
					  pos += 8;
					  word >>= 8;	  
					}
					// select within the last byte   //@@fari
						//optimized lookup-based select 
					*sjmas1= pos + sel1_b[word&0xFF][j];
					// SEGUNDO  SELECT COMPLETADO		
				}
				
			}
			
		}
		else {  //BSELECT FROM NEXT SUPERBLOCK ON...
			*sjmas1 = bselect(B,jmas1);				
		}
	}
	//return 0;
}



// destroys the bitmap, freeing the original bitstream
void destroyBitmap (bitmap B)

   { //free (B->data);
     free (B->bdata);
     free (B->sdata);
     
     free (B->ones);
     free (B);
   }
   
   
// Prints the bit vector
void showBitVector(uint * V, int vectorSize) {
     uint bitIndex=0;
     while(bitIndex<vectorSize) {
        fprintf(stderr,"%d",bitget(V,bitIndex));
        bitIndex++;
     }	 
}   
  
void saveBitmap (char *filename, bitmap b) {
	int file;
	unlink(filename);
	if( (file = open(filename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
		printf("Cannot open file %s\n", filename);
		exit(0);
	}
	ssize_t write_err;
	write_err=write(file, &(b->sSize), sizeof(uint));
	write_err=write(file, b->sdata, sizeof(int) * (b->sSize));
	write_err=write(file, &(b->bSize), sizeof(uint));
	write_err=write(file, b->bdata, sizeof(byte) * (b->bSize));

	write_err=write(file, &(b->pop), sizeof(uint));
	write_err=write(file, &(b->n), sizeof(uint));
	close(file);		
}

/* loads the Rank structures from disk, and sets Bitmap->data ptr to "string"
*/
bitmap loadBitmap (char *filename, uint *string, uint n) {  
	bitmap B;
	int file;
	
	if( (file = open(filename, O_RDONLY)) < 0) {
		printf("Cannot read file %s\n", filename);
		exit(0);
	}		
	
	B = (struct sbitmap *) malloc (sizeof(struct sbitmap));
	B->data = string;
	ssize_t read_err;
	read_err= read(file, &(B->sSize), sizeof(uint));
	B->sdata = (uint *) malloc(sizeof(uint) * B->sSize);
	read_err= read(file, B->sdata, sizeof(uint) * B->sSize);

	read_err= read(file, &(B->bSize), sizeof(uint));
	B->bdata = (byte *) malloc(sizeof(byte) * B->bSize);
	read_err= read(file, B->bdata, sizeof(byte) * B->bSize);	
	
	read_err= read(file, &(B->pop), sizeof(uint));
	read_err= read(file, &(B->n), sizeof(uint));	
	close(file);


	//structura para agilizar selects en supercomputing
	B->nblocks=(B->n-1)/32;
	//Compute ones
	B->ones = (uint *) malloc((B->pop/256 + 3) * sizeof(uint));
	int i;
	for (i = 0; i <= (B->pop+255)/256; i++) {
		B->ones[i] = bselect_orig(B,i*256+1);
	}
	B->ones[i] = B->n;

	B->mem_usage = (sizeof(uint) * B->sSize) + (sizeof(byte) * B->bSize) + sizeof(uint)*(B->pop/256+2) + (sizeof(struct sbitmap));


/*
	fprintf(stderr, "Checking bitmap\n");
	for (i = 0; i <= B->pop; i++) {
		if (!(i%1000000)) fprintf(stderr, "%d\n", i);
		//fprintf(stderr, "--- %d\n", i);
		uint v1 = bselect_orig(B,i);
		uint v2 = bselect(B,i);
		uint v3 = bselect(B,i+1);
		//fprintf(stderr, "-x- %d\n", i);
		size_t aux1, aux2;
		bselect_j_y_j_mas_1(B,i,&aux1, &aux2);
		//fprintf(stderr, "xx- %d\n", i);
		
		if (v1 != v2) {
			fprintf(stderr, "ERROR (%d: %d vs %d)\n", i, v1, v2);
			exit(0);
		}
		
		
		if (v2 != aux1 || v3 != aux2) {
			fprintf(stderr, "ERROR2 (%d: %d,%d vs %d,%d)\n", i, v2, v3, aux1, aux2);
			exit(0);
		}
		
	}
*/	
	//	printf("\n nbits set to 1 in bitmap is %u", B->pop);
	
	if (n != B->n) {printf("\n LoadBitmap failed: %u distinto de %u",n,B->n); exit(0);}
	return B;
		
} 
   

	// returns the size, in bytes, of the bitmap
uint sizebytesBitmap (bitmap B) {
	return B->mem_usage;
}

   
/********************************************************************************************/
/********************************************************************************************/




	// creates a bitmap structure from a bitstring, which is shared

bitmap createBitmapGONZA (uint *string, uint n)
//bitmap createBitmap (uint *string, uint n)

   { bitmap B;
     uint i,j,pop,bpop,pos;
     uint s,nb,ns,words;
     B = (struct sbitmap *) malloc (sizeof(struct sbitmap));
     B->data = string;
     
   
 	 B->n = n; words = (n+W-1)/W;
     ns = (n+256-1)/256; nb = 256/W; // adjustments          
     
     B->bSize = ns*nb;
     B->bdata = (byte *) malloc (ns*nb*sizeof(byte));
     B->sSize = ns;
     B->sdata = (uint *)malloc (ns*sizeof(int));

	 B->mem_usage = (ns*sizeof(int)) + (ns*nb*sizeof(byte)) + (sizeof(struct sbitmap));
#ifdef INDEXREPORT
     printf ("     Bitmap over %i bits took %i bits\n", n,n+ns*nb*8+ns*32);
#endif 
	  //fprintf (stderr,"     Bitmap over %i bits took %i bits\n", n,n+ns*nb*8+ns*32);
     pop = 0; pos = 0;
     for (i=0;i<ns;i++)
	{ bpop = 0;
	  B->sdata[i] = pop;
	  for (j=0;j<nb;j++)
	     { if (pos == words) break;
	       B->bdata[pos++] = bpop;
	       bpop += popcount(*string++);
	     }
	  pop += bpop;
	}
     B->pop = pop;
     
		//     //fprintf(stderr,"\n");
		//     for (i=0;i<ns;i++) {//fprintf(stderr,"%d ",B->sdata[i]);
		//     	}
		//     //fprintf(stderr,"\n");
		//     for (i=0;i<ns*nb;i++) {//fprintf(stderr,"%d ",B->bdata[i]);
		//     	}
		     
     return B;
   }

	// rank(i): how many 1's are there before position i, not included

//uint rank (bitmap B, uint i)
uint rankGONZA (bitmap B, uint i)

   { 
   	i++;
   	if (i > B->n) return B->pop;
   	return B->sdata[i>>8] + B->bdata[i>>5] +
	    popcount (B->data[i>>5] & ((1<<(i&0x1F))-1));
   }


   
   
   
   
   



		

/************************************************************************/
/*@ 2020 
 * 
*/



// getNext1 gives the position of the first 1 after pos 
// (excluded) and until posLimTriple (included)
size_t getNext1 (uint *b, size_t pos, size_t posLimTriple) {

	uint i;
	size_t j, next1; 
	// nE  : position of an uint in b
	// nES : position of the first uint to be checked in b
	// nEE : position of the last uint to be checked in b
	// nES_end : last position (included) of nES
	size_t nE, nES, nEE, nES_end;
	// shiftS  : right shift to align (from the beginning) the first position to be checked from nES
	// shiftE  : left shift to align (from the end) the last position to be checked from nEE
	uint shiftS, shiftE;
	// nBytes  : number of complete bytes to check
	uint nBytes;
	// word : the uint to be checked at each time
	uint word;

//	if (pos >= posLimTriple) {
//		//fprintf(stderr, "\tRETURN NO MORE 1s! without processing!\n");
//		return posLimTriple + 1;
//	} 

//	size_t posLimTriple = B->n;
//	if (pos > B->pop) return posLimTriple;	

	nES = (pos + 1) / W;
	shiftS = (pos + 1) % W;
	
	nEE = posLimTriple / W;
	shiftE = W - ((posLimTriple % W) + 1);

	//fprintf(stderr, "W = %u, pos = %zu, posLimTriple = %zu, nES = %zu, shiftS = %u, nEE = %zu, shiftE = %u\n", 
	//		W, pos, posLimTriple, nES, shiftS, nEE, shiftE);

	// 1. In case that just one uint should be checked
	if (nES == nEE) {
		//fprintf(stderr, "nES == nEE\n");
		word = b[nES];

		// 1.1. We first align the uint with the last position to be checked (left side)
		// and then we align it with the first one (right side)
		word <<= shiftE;
		word >>= shiftE + shiftS;

		j = pos + 1;
		// 1.2 We next try to get a byte limit
		while (j % 8 != 0) {	
			if (word & 1) {
				//fprintf(stderr, "\tRETURN %zu!\n", j);
				return j;
			} else {
				word >>= 1;
				j++;
			}
		}
		// 1.3 After getting a byte limit
		// We process as many complete bytes as possible (except the last one, probably)
		nBytes = ((posLimTriple - j) + 1) / 8;
		for (i = 0; i < nBytes; i++) {
			next1 = next1_b[word & 0xFF];
			if (next1 < 8) {
				//fprintf(stderr, "\tRETURN %zu + %zu!\n", j, next1);
				return (j + next1);
			} else {
				word >>= 8;
				j += 8;
			}
		}
		// We process the last incomplete byte (in case), or return 'NO next 1 ahead'
		while (j <= posLimTriple) {
			if (word & 1) {
				//fprintf(stderr, "\tRETURN %zu!\n", j);
				return j;
			} else {
				word >>= 1;
				j++;
			}
		}
		//fprintf(stderr, "\tRETURN NO MORE 1s!\n");
		return posLimTriple + 1;
		
	// 2. If nES and nEE are not the same uint, we must distinguish among
	// the first uint, those in the middle and the last one. Each one will
	// be processed in a different way
	} else {
		//fprintf(stderr, "nES != nEE\n");
		// ** First uint ** 
		//fprintf(stderr, "\t**First UINT! (%zu)\n", nES);
		// 2.1. We align it with the first position to be checked (right side)
		word = b[nES];
		word >>= shiftS;

		j = pos + 1;
		// 2.2. We next try to get a byte limit
		while (j % 8 != 0) {	
			if (word & 1) {
				//fprintf(stderr, "\tRETURN %zu!\n", j);
				return j;
			} else {
				word >>= 1;
				j++;
			}
		}

		// 2.3. After getting a byte limit, we can process all the bytes left
		// (as they are complete bytes till the end of the first uint)
		// -- How many bytes are left from 'j'?
		nES_end = ((nES + 1) * W) - 1; 
		nBytes = ((nES_end - j) + 1) / 8;
		for (i = 0; i < nBytes; i++) {
			next1 = next1_b[word & 0xFF];
			if (next1 < 8) {
				//fprintf(stderr, "\tRETURN %zu + %zu!\n", j, next1);
				return j + next1;
			} else {
				word >>= 8;
				j += 8;
			}
		}

		// ** Uints from the middle ** 

		// 2.1. All of them are complete uints to be checked, so we can process 
		// all their bytes in a same way
		nE = nES + 1;
		while (nE < nEE) {
			//fprintf(stderr, "\t**UINTs from the middle! (%zu)\n", nE);
			word = b[nE];
			for (i = 0; i < 4; i++) {
				next1 = next1_b[word & 0xFF];
				if (next1 < 8) {
					//fprintf(stderr, "\tRETURN %zu + %zu!\n", j, next1);
					return j + next1;
				} else {
					word >>= 8;
					j += 8;
				}
			}
			nE = nE + 1;
		}

		// ** Last uint **
		//fprintf(stderr, "\t**Last UINT! (%zu)\n", nEE);
		// 2.1. All its bytes but the last one are complete bytes, so we must align them
		// first to the end position (left side), and then move them all to the beginning 
		// (right side)
		word = b[nEE];
		word <<= shiftE;
		word >>= shiftE;

		// 2.2. Now we process as many complete bytes as possible (i.e. except the last one,
		// probably)
		nBytes = ((posLimTriple - j) + 1) / 8;
		for (i = 0; i < nBytes; i++) {
			next1 = next1_b[word & 0xFF];
			if (next1 < 8) {
				//fprintf(stderr, "\tRETURN %zu + %zu!\n", j, next1);
				return (j + next1);
			} else {
				word >>= 8;
				j += 8;
			}
		}
		// 2.3. We process the last incomplete byte (in case), or return 'NO next 1 ahead'
		while (j <= posLimTriple) {
			if (word & 1) {
				//fprintf(stderr, "\tRETURN %zu!\n", j);
				return j;
			} else {
				word >>= 1;
				j++;
			}
		}
		//fprintf(stderr, "\tRETURN NO MORE 1s!\n");
		return posLimTriple + 1;
		
	}
}
