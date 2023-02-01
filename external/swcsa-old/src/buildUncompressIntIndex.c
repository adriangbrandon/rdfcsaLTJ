/* buildUncompress.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * buildUncompress.c: Program to recover the original data from a given
 *   self-index (wcsa). The text is saved to disk.
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
#include "buildFacade.h"

/**------------------------------------------------------------------ 
  *  MAIN PROGRAM.
  *------------------------------------------------------------------ */





void recoverIntSource2(void *Index, char *basename, char *ext) {		

	long start;long end;
	uint *cc;
	char *filename = (char *) malloc (strlen( basename)+ strlen(ext)+1);
	
	strcpy( filename,  basename);
	strcat( filename, ext);
	filename[strlen( basename)+ strlen(ext)]='\0';
	fprintf(stderr,"\n uncompressing text into file -->%s ",filename);fflush(stderr);

	FILE *salida;
	unlink( filename);
	salida = fopen( filename,"w");

	{  		
		uint *buffer;
	
		size_t size_buffer;
		dumpICSASequence(Index, &buffer, &size_buffer);
			
		fprintf(stderr,"\n NumInts_to_recover = %lu, size = %lu",size_buffer,size_buffer*4);
		fwrite(buffer,sizeof(uint),size_buffer,salida);
		fclose(salida);

		free(buffer);
		free(filename);
	}
}


int main(int argc, char* argv[])
{
				
	char *infile; char *outbasename;
	void *Index;

	
	printf("\n*Presentation level for CSA: Query Interface");
	printf("\n*CopyRight (c) 2007 [LBD & G.N.]\n\n");

	// Reads input parameters from command line.
	if(argc < 3) {
		printf("Use: %s <in basename> <out basename>  \n", argv[0]);
		exit(0);
	}
	// Leemos os argumentos (nome do ficheiro de entrada e do ficheiro de saida)
	infile = argv[1];
	outbasename = argv[2];
		
	int err = loadIntIndex(infile, &Index);

	
	{
		/** tells the mem used by the index */
		ulong indexsize; uint numInts;
		
		sizeIntIndex(Index, &indexsize);    //not used
	    sourceLenIntIndex(Index,&numInts);  //not used
		
		fprintf(stderr,"InT Index occupied %lu bytes, and has %lu integers",indexsize, (ulong) numInts);

		/** recovering the source text from the index */
			{
				double start, end;
				start = getTime2();
			
				fprintf(stderr, "\nRecovering source Integers ");	fflush(stderr);
				char ext1[10]=".source";
				
				recoverIntSource2(Index, outbasename,ext1);
				
				end = getTime2();	
				fprintf(stderr, " time: %.3f secs\n", end-start );				
			}		
		/** freeing the index */		 			
		freeIntIndex(Index);	 			
	 		 
	}
}





























//{	
//	bG = createBitmap (B,len);
//	bE = createBitmapEdu (B,len);
//	//fprintf(stderr,"\n RANK DE GONZALO DA %u, de EDU DA %u\n",rank(bG,33),rank1Edu(bE,33));
//	//fprintf(stderr,"\n SELECT1(2) DE GONZALO DA %u, de EDU DA %u\n",bselect(bG,4),bselect(bE,4));
//	
//	showBitVector(bitvector,34);	
//}



/*	

	//USING A HASH TABLE
	//	{
	//	char a[20]="beginnings";char b[20]="HOSTIAS";
	//	char *w;	
	//	int i;
	//	w=a;
	//	i = inHashTable(stopwordshash,w, strlen(w), &addrInTH );
	//	if (!i) insertElement (stopwordshash, w, strlen(w), &addrInTH);
	//	else stopwordshash->hash[addrInTH].freq++;
	//	//fprintf(stderr,"\n i = %ld, addrInTh = %ld ",i,addrInTH);
	//	//fprintf(stderr,"\n word in hash[%ld]= %s , freq = %ld, posinvoc =%ld",addrInTH, stopwordshash->hash[addrInTH].word, stopwordshash->hash[addrInTH].freq, stopwordshash->hash[addrInTH].posInVoc);	
	//  }



	/// ENCODING THE separators  ...
{	
	freeHuff(gapsHuffman);
	uint i;
	uint *bitvector;
	uint bitvectorSize;
	uint ptr;
	bitmap bG,bE;
	uint len;
	len = 1000; //number of bits
	bitvector = (uint *) malloc ((len/32 +1)* sizeof(uint));
	
	byte texto[100] = "####@?*";
	uint freqs[256];
	
	//fprintf(stderr,"\n este es el texto a codificar: %s",texto);
	for (i=0;i<256;i++) freqs[i]=0;
	for (i=0;i<strlen(texto);i++) freqs[texto[i]]++;
	gapsHuffman = createHuff (freqs,255,UNSORTED);
		
	ptr=0;
	for (i=0;i<strlen(texto);i++) {
		//fprintf(stderr,"\n ENCODING seprators !!\n");
		//fprintf(stderr,"%d. \n",ptr=encodeHuff(gapsHuffman, texto[i],bitvector,ptr) );		
	}	

	prepareToDecode(&(gapsHuffman));
	bitvectorSize = ptr;
	showBitVector(bitvector,bitvectorSize);	
	uint pos;
	//fprintf(stderr,"\n DECODING !!\n");
	ptr=0;
	while (ptr < bitvectorSize) {
		ptr=decodeHuff (gapsHuffman, &pos, bitvector, ptr);
		//fprintf(stderr,"\n DECODING pos is %ld!!\n",pos);
		//fprintf(stderr,"%c. \n",pos);
	}
	exit(0);	
}															
	
/// ENCODING THE CANONICAL WORDS ...
{	uint i;
	uint *bitvector;
	uint bitvectorSize;
	uint ptr;
	bitmap bG,bE;
	uint len;
	len = 1000; //number of bits
	bitvector = (uint *) malloc ((len/32 +1)* sizeof(uint));
	
	
	ptr=0;
	//fprintf(stderr,"\n ENCODING VARIANTS !!\n");
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 0,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 1,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 2,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 3,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 4,bitvector,ptr) );
	//fprintf(stderr,"%d. \n",ptr=encodeHuff(posInHT[0].huffman, 4,bitvector,ptr) );
			
	//	FILE *f;
	//	f = fopen("huff","w");
	//	saveHuff(posInHT[0].huffman,f);
	//	fclose(f);
	//	
	//	f = fopen("huff","r");
	//	posInHT[0].huffman = loadHuff (f,0);
	//	fclose(f);
	//	
	
	prepareToDecode(&(posInHT[0].huffman));

	FILE *f;
	f = fopen("huff","w");
	saveHuffAfterDecode(posInHT[0].huffman,f);
	fclose(f);
	
	f = fopen("huff","r");
	//posInHT[0].huffman = loadHuffAfterDecode(f,0);
	loadHuffAfterDecode2 (&(posInHT[0].huffman),f,0);
	fclose(f);
	


	bitvectorSize = ptr;
	showBitVector(bitvector,bitvectorSize);	
	uint pos;
	//fprintf(stderr,"\n DECODING !!\n");
	ptr=0;
	while (ptr < bitvectorSize) {
		ptr=decodeHuff (posInHT[0].huffman, &pos, bitvector, ptr);
		//fprintf(stderr,"\n DECODING pos is %ld!!\n",pos);
		//fprintf(stderr,"%s. \n",posInHT[0].variants[pos]);
	}
	exit(0);
}	
	
	
			
{	uint i;
	uint *bitvector;
	bitmap bG,bE;
	uint len;
	len = 101; //number of bits
	bitvector = (uint *) malloc ((len/32 +1)* sizeof(uint));
	//bitvector[0]=0;
	//bitvector[1]=0;
	bitzero (bitvector,0,101-1);
	for (i=0; i<len;i++) setBit (bitvector,len,i,0);
	 


	bitset(bitvector,1); 
	bitset(bitvector,10);
	bitset(bitvector,12);
	//activateBit(bitvector,1); 
	//activateBit(bitvector,10); 
	
	bG = createBitmap (bitvector,len);
	bE = createBitmapEdu (bitvector,len);
	//fprintf(stderr,"\n RANK DE GONZALO DA %u, de EDU DA %u\n",rank(bG,33),rank1Edu(bE,33));
	//fprintf(stderr,"\n SELECT1(2) DE GONZALO DA %u, de EDU DA %u\n",bselect(bG,4),bselect(bE,4));
	
	showBitVector(bitvector,34);
}	
*/		
	
