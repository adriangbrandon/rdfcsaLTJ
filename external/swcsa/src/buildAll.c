/* buildAll.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * BuildAll: Main program to check building and searching operations.
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
//#include "graphReader.c"


/**------------------------------------------------------------------ 
  *  MAIN PROGRAM.
  *------------------------------------------------------------------ */


void print_usage(char *);
double getTime(void);



int main(int argc, char* argv[])
{

	//struct graphDB graph;


	char *infilebase, *outfile;
	char *params = NULL;
	void *index;

	int  i;
	double start, end;

    if (argc < 3) print_usage(argv[0]);
	if (argc > 3) { 
		int nchars, len;
		nchars = argc-3;
		for(i=2;i<argc;i++)
			nchars += strlen(argv[i]);
			
		params = (char *) malloc((nchars+i+1)*sizeof(char));
		params[nchars+i] = '\0';
		nchars = 0;
		for(i=3;i<argc;i++) {
			len = strlen(argv[i]);
			//strncpy(params+nchars,argv[i],len);   //@2020 due to gcc 8 stupid warning [-Wstringop-overflow=]
			strcpy(params+nchars,argv[i]);         //@2020 due to gcc 8 stupid warning [-Wstringop-overflow=]
			params[nchars+len] = ' ';
			nchars += len+1;
		}

		params[nchars-1] = '\0';
	}
	infilebase = argv[1];
	outfile = argv[2];

	start = getTime();
		
	//reads source data and adds dummie triple and terminator.
//	parseFileHDTformat_IntoGraph(infilebase,&graph);

	{
		/** buiolding the index */
								
		//build_index (&graph, params,  &index);  
		build_index(infilebase,params,&index);
		fprintf(stderr,"\n Index BUILT !!\n");

		/** saving the index to disk */
		
		save_index (index, outfile);		
		fprintf(stderr,"\n Index SAVED !!\n");

		/** tells the mem used by the index */
		ulong indexsize;		
		index_size(index, &indexsize);
		fprintf(stderr,"\n Index occupied %lu bytes, 2 extra mallocs = %lu",indexsize, (ulong) 2* sizeof(uint));

/*
		// ** recovering the source text from the index 
			{
				double start, end;
				start = getTime2();
				ulong size;
				get_length(index, &size);

				fprintf(stderr, "\nRecovering source file ");	fflush(stderr);
				char ext1[10]=".source";
				recoverSourceText1((twcsa*) index, outbasename,ext1, size);
				end = getTime2();	
				fprintf(stderr, " time: %.3f secs\n", end-start );	

				start=end;
				char ext2[10]=".source2";
				fprintf(stderr, "\nRecovering source file ");	fflush(stderr);
				recoverSourceText2((twcsa*) index, outbasename,ext2,size);
				end = getTime2();	
				fprintf(stderr, " time: %.3f secs\n", end-start );	
				//fprintf(stderr, "\nRecovering source file time: %.3f secs\n", end-start );	
			}
	
*/


		/** freeing the index */	

		free_index(index);	 			
	 		 
	}
	

	free(params);
	
	//fprintf(stdout,"\n\n\t ## Building time (**parsing into integers + present_layer: %.3f secs", end-start );
	//fprintf(stdout,"\n\t ## Input: %lu bytes --> Output (pres_layer) %lu bytes.", text_len, index_len);
	//fprintf(stdout,"\n\t ## Overall compression --> %.2f%% (%.2f bits per char).\n\n",
    // 			(100.0*index_len)/text_len, (index_len*8.0)/text_len);


	exit(0);	
	
}



// --------------------------------------------------------------------
// --------------------------------------------------------------------

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
	fprintf(stderr, "Usage: %s <source file> <index file> [<parameters>]\n", progname);
	fprintf(stderr, "\nIt builds the index for the text in file <source file>,\n");
	fprintf(stderr, "storing it in <index file>. Any additional <parameters> \n");
	fprintf(stderr, "will be passed to the construction function.\n");
	fprintf(stderr, "At the end, the program sends to the standard error \n");
	fprintf(stderr, "performance measures on time to build the index.\n\n");
	exit(1);
}


























