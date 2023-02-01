/* buildIntIndex.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * buildIntIndex.c: Program that, using the files created by the "buildParser"
 *   program (SE sequence, the vocabulary of words, and some extra constants),
 *   creates a WCSA self-index over the sequence "SE".
 *   All the resulting structures are permanently saved to disk. 
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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buildFacade.h"

/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>
 
/* macro to detect and notify errors  */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

int loadNEntries (char *basename, uint *nEntries);
int loadSEfile (char *basename, uint **v, ulong *n);
void print_usage(char *);
double getTime(void);

int main(int argc, char *argv[]) {

	char *basenamefile;
	char *params = NULL;
	void *index; size_t index_len;
	int error, i;
	double start, end;
	uint *se;
	ulong seSize;

    if (argc < 2) print_usage(argv[0]);
	if (argc > 2) { 
		int nchars, len;
		nchars = argc-2;
		for(i=1;i<argc;i++)
			nchars += strlen(argv[i]);
//		params = (char *) malloc((nchars+1)*sizeof(char));
		params = (char *) malloc((nchars+i+1)*sizeof(char));
		params[nchars] = '\0';
		nchars = 0;
		for(i=2;i<argc;i++) {
			len = strlen(argv[i]);
			//strncpy(params+nchars,argv[i],len);   //@2020 due to gcc 8 stupid warning [-Wstringop-overflow=]
			strcpy(params+nchars,argv[i]);         //@2020 due to gcc 8 stupid warning [-Wstringop-overflow=]			
			params[nchars+len] = ' ';
			nchars += len+1;
		}
		params[nchars-1] = '\0';
	}

	basenamefile = argv[1];

	start = getTime();

	error = loadSEfile (basenamefile, &se, &seSize);
	IFERROR(error);
	
	uint nEntries;
	error = loadNEntries (basenamefile, &nEntries);
	
    //fprintf(stderr,"\n parameters (stopwordsFilename): \"%s\"\n",params); fflush(stderr);
    
	error =  buildIntIndex(se,seSize, nEntries, params,(void **)&index);				
	IFERROR(error);

	if (index) {
		error = saveIntIndex(index, basenamefile);		
	}	
		
	IFERROR(error);
	end = getTime();	
	
	error = sizeIntIndex(index, &index_len);
	IFERROR(error);

	printf("\n\n\t Freeing memory...");
		
	error = freeIntIndex(index);
	IFERROR(error);

	free(se); 
	free(params);
	
	fprintf(stdout,"\n\tBuilding time (**building self-index on ints: %.3f secs", end-start );
	fprintf(stdout,"\n\t ## Input: %zu bytes --> Output (pres_layer) %zu bytes.", (size_t) seSize*sizeof(uint), index_len);
	fprintf(stdout,"\n\t ## Overall compression --> %.3f%% (%.3f bits per char).\n\n",
     			(100.0*index_len)/(seSize*sizeof(uint)), (index_len*8.0)/(seSize*sizeof(uint)));

	exit(0);
}


int loadSEfile (char *basename, uint **v, ulong *n){
	char filename[255];
	FILE *file; 
	sprintf(filename,"%s.%s",basename,SE_FILE_EXT);
	
	size_t sizeFile = fileSize(filename);
	
	if( sizeFile <= 0) {
		printf("Cannot read information from file %s\n", filename);	return -1;
	}	
	
	if( (file = fopen(filename, "r")) ==NULL) {
		printf("Cannot open file %s\n", filename);
		return -1;
	}

	uint *se = (uint *) malloc (sizeFile);
	ulong seSize = ((size_t)sizeFile) / sizeof(uint);
	//read(file, se, sizeFile);       	//the samples
	size_t fread_err = fread(se,sizeof(unsigned char),sizeFile, file);
	fclose(file);
	*v=se;
	*n=seSize;	
	return 0;	
}


int loadNEntries (char *basename, uint *nEntries){
	char filename[255];
	FILE *file; 
	
	/**File with some constants  */
	{	sprintf(filename,"%s.%s",basename,CONSTANTS_FILE_EXT);		
		if( (file = fopen(filename, "r")) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}

		size_t read_err = fread(nEntries,sizeof(uint),1, file);
		fclose(file);
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

	return (usertime + systime);

}

void print_usage(char * progname) {
	fprintf(stderr, "Usage: %s <index file> [<parameters>]\n", progname);
	fprintf(stderr, "\nIt builds the index on Integer for the sequence in <index file>.se,\n");
	fprintf(stderr, "storing it in [<index file>.[*]]; Any additional <parameters> \n");
	fprintf(stderr, "will be passed to the construction function.\n");
	fprintf(stderr, "At the end, the program sends to the standard error \n");
	fprintf(stderr, "performance measures on time to build it.\n\n");
	exit(1);
}
