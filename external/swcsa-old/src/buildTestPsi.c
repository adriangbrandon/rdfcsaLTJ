/* buildStats.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * buildStats.c: Program that shows detailed info about a given self-index.
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
#include <string.h>
#include <unistd.h>
#include "buildFacade.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>

/* macro to detect and to notify errors */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}


/* local headers */

double getTime (void);
void usage(char * progname);
  		
static void *Index;	 /* opauque data type */
static ulong Index_size;
static double Load_time;



//See icsa.c to allow psi.dump to be written to disk.
//	FILE *f = fopen("psi.dump","w");
//	fwrite(&psiSize,1,sizeof(size_t),f);
//	fwrite(Psi,sizeof(uint),psiSize,f);
//	fclose(f);

int main (int argc, char *argv[])
{
	int error = 0;
	char *filename;
	char querytype;

	//size_t inipos = 1182407833-100;
	//if (argc==2)
	//	inipos = atol(argv[1]);
	
	size_t inipos=0;



	
//	if (argc != 2)	{
//		usage(argv[0]);	
//		exit (1);
//	}
	
//	filename = argv[1];

     size_t psiSize;
	FILE *f = fopen("psi.dump","r");
	if (!f) {
		printf("\n a file psi.dump was not found\n modify icsa.c so that this file is output during index creation and then retry\n");
	}
	size_t err= fread(&psiSize,1,sizeof(size_t),f);
	
     uint *Psi = (uint *) malloc (psiSize * sizeof(uint));
     if (!Psi) printf("\n mallocs fails");
	  err= fread(Psi,sizeof(uint),psiSize,f);
	printf("\n psisize = %zu",psiSize);fflush(stdout);
	fclose(f);

	printf("\n I will test psi compression from i=%zu to end=%zu",inipos, psiSize);	  fflush(stdout);
	printf("\n compressing psi\n"); fflush(stdout);
	HuffmanCompressedPsi cPsi = huffmanCompressPsi(Psi,psiSize,16,16384);
	

	size_t end = psiSize-1;

	uint * psibuffer;
	/*********/
	//printf("\n recovering psi buffer psi\n"); fflush(stdout);
	//psibuffer = (uint *) malloc (sizeof(uint) * (psiSize ) );	 
	//getHuffmanPsiValueBuffer(&cPsi, &psibuffer[inipos],inipos,end);	
	/*********/
	psibuffer = (uint *) malloc (sizeof(uint) * (psiSize ) );	 
	{
		size_t step = 5;
		size_t j = inipos;
		printf("\n recovering psi buffer psi with steps of %zu\n",step); fflush(stdout);
		for ( ; j < end-step-step ; ) {
			getHuffmanPsiValueBuffer(&cPsi, &psibuffer[j],j,j+step-1);
			j +=step;
		}
		getHuffmanPsiValueBuffer(&cPsi, &psibuffer[j],j,end);	
	}
	
	/*********/
	//  psibuffer=Psi;
	/*********/
	 
	 size_t j;

	char filenametmp[256]= "tmpFilename.psi.compressed";
	storeHuffmanCompressedPsi(&cPsi,filenametmp);
	HuffmanCompressedPsi cPsi2 = loadHuffmanCompressedPsi(filenametmp);
	unlink(filenametmp);

	printf("\n Now comparing values \n"); fflush(stdout);

	 err=0;
	 int count =0;
	 size_t salto = (end-inipos) /1000;
	 for (j=inipos;j<=end;j++) {
		 if ( !((j-inipos) %salto))
				fprintf(stderr, "Processing %.1f%%\r", (float)(j-inipos)/(end-inipos)*100); fflush (stderr);
			

		 uint value = psibuffer[j];  
		 uint value2 = getHuffmanPsiValue(&cPsi,j); 
		 uint value3 = getHuffmanPsiValue(&cPsi2,j);
		 if ( !(( Psi[j] == value2)   &&  ( Psi[j] == value)  &&  ( Psi[j] == value3))  ) {
			printf("\n original psi value [j=%zu] is %u , uncompressedBuffer is [%u] , uncompressed is [%u]  , uncompressedAfterSave/load is [%u]",j, Psi[j],value ,value2,value3);
			err=1;
			count++;
			if (count > 25) exit(0);
		}
	 }
	 
	 if (!err) { printf("\n psi uncompressed Ok \n");
		{
		  j=0;
		  uint value = psibuffer[j];  uint value2 = getHuffmanPsiValue(&cPsi,j); uint value3 = getHuffmanPsiValue(&cPsi2,j);
		  printf("\n original psi value [j=%zu] is %u , uncompressedBuffer is [%u] , uncompressed is [%u]  , uncompressedAfterSave/load is [%u]\n....\n",j, Psi[j],value ,value2,value3);
		}
		{
		  j=end;
		  uint value = psibuffer[j];  uint value2 = getHuffmanPsiValue(&cPsi,j); uint value3 = getHuffmanPsiValue(&cPsi2,j);
		  printf("\n original psi value [j=%zu] is %u , uncompressedBuffer is [%u] , uncompressed is [%u]  , uncompressedAfterSave/load is [%u]\n....\n",j, Psi[j],value ,value2,value3);
		}
	}




	return 0;
}

double
getTime (void)
{
	double usertime, systime;
	struct rusage usage;

	getrusage (RUSAGE_SELF, &usage);

	usertime = (double) usage.ru_utime.tv_sec +
		(double) usage.ru_utime.tv_usec / 1000000.0;

	systime = (double) usage.ru_stime.tv_sec +
		(double) usage.ru_stime.tv_usec / 1000000.0;

	//return (usertime + systime);
	return (usertime );
}


void usage(char * progname) {	
	fprintf(stderr, "\nThe program loads <index> and shows some stats on it\n");
	fprintf(stderr, "Usage:  %s <index> \n", progname);
}
