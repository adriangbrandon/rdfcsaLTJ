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

int main(int argc, char* argv[])
{
				
	char *infile; char *outbasename;
	twcsa *wcsa;
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
		
	//wcsa = loadWCSA(infile);
	load_index(infile, &Index);
	wcsa = (twcsa *) Index;
	
	{

		/** tells the mem used by the index */
		ulong indexsize;		
		
		index_size(Index, &indexsize);
		fprintf(stderr,"Index occupied %lu bytes, 2 extra mallocs = %zu",indexsize, (size_t) 2* sizeof(uint));


		/** recovering the source text from the index */
			char filename [256];
				double start, end;
			{
				start = getTime2();

			
				fprintf(stderr, "\nRecovering source file ");	fflush(stderr);
				sprintf(filename,"%s.source",outbasename);
				FILE *f;
				unlink( filename);
				f = fopen( filename,"w");

				uint numInts;
				sourceLenIntIndex(wcsa->myicsa,&numInts);
				
				twcsa *wcsa = (twcsa*)Index;
				uint *buffer;
				size_t size_buffer;

				//dumpSourceData_slow(wcsa, &buffer, &size_buffer);
				//dumpSourceDataSorted(wcsa, &buffer, &size_buffer);
				dumpSourceData(wcsa, &buffer, &size_buffer);

//extern uint gr_nentriesGicsa;
//gr_nentriesGicsa= wcsa->nEntries;
//qsort(buffer, wcsa->n, sizeof(uint)*wcsa->nEntries, gr_graphsuffixCmp_PSO);
				
				{//fwrite(buffer,sizeof(uint), wcsa->ssize,f);
					size_t i,j,z=0;
					z=0;
					z+=wcsa->nEntries;
					for (i=1; i<wcsa->n; i++){
						for (j=0;j<wcsa->nEntries;j++) {
							//fprintf(f,"%u ",buffer[z]);
							fwrite(&buffer[z],sizeof(uint),1,f);
							z++;
						}
						//fprintf(f,"%u\n",buffer[z]);
						//z++;
					}					
				}
				
				fclose(f);
				my_free_array(buffer);
				
				end = getTime2();	

			}		
		/** freeing the index */		 			
		free_index(Index);	 			
		fprintf(stderr, "\nRecovered source file [%s] (maybe in diff order) in %.1f secs\n\n", filename, end-start );					
	 		 
	}
}










