/* buildParser.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * buildParser.c: Program that Parses the source text and creates the sequence
 *   SE composed of uint32 IDs that map each source word to and integer ID.
 *   It also obtains the vocabulary of words (and some extra constants).
 *   All these data are saved to disk and will probably be used by the 
 *   "buildIntIndex" program.
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
#include "graphReader.c"


/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>
 
/* macro to detect and notify errors  */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

void print_usage(char *);
double getTime(void);

int main(int argc, char *argv[]) {

	char *infilebase, *outfile;
    //uchar *text;
	char *params = NULL;
	//ulong text_len;
	void *index;
	int error, i;
	double start, end;

    if (argc < 3) print_usage(argv[0]);
	if (argc > 3) { 
		int nchars, len;
		nchars = argc-3;
		for(i=2;i<argc;i++)
			nchars += strlen(argv[i]);
//		params = (char *) malloc((nchars+1)*sizeof(char));
		params = (char *) malloc((nchars+i+1)*sizeof(char));
		
		params[nchars] = '\0';
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

	outfile = argv[2];
// ---------- Set Graph properties and read input -----------------------

	infilebase = argv[1];

	start = getTime();


	char infileHDR[2048], infileTRIPLES[2048];
	sprintf(infileHDR,"%s.header",infilebase);
	sprintf(infileTRIPLES,"%s.triples",infilebase);


	struct graphDB graph;
	
	graph.nEntries = 3; // 3=RDF, 4=TemporalGraph,...  (number of entries per record/contact...)
	 	
	FILE *f = fopen(infileHDR, "r"); 
	if(f == NULL) return 1;
	error = gr_readHeader(&graph, f);
	IFERROR(error);
	fclose (f);

	//fprintf(stderr, "\nRead Nodes= %u, lifetime=%u, contacts=%zu\n", graph.nodes, graph.maxtime, graph.n);
	fprintf(stderr, "\nRDF.HEADER read: np= %u, ns=%u, no=%u,nso=%u, n=%zu\n", graph.np,graph.ns,graph.no,graph.nso, graph.n);


	f = fopen(infileTRIPLES, "r"); 
	if(f == NULL) return 1;
	error = gr_readRecords(&graph,f);
	IFERROR(error);
	fclose(f);
	

	gr_printRecords(&graph);

	
	if (gr_areSortedRecords(&graph))
		fprintf(stderr,"\n INPUT RECORDS ARE SORTED INCREASINGLY");
	else
		fprintf(stderr,"\n INPUT RECORDS ARE #not# SORTED INCREASINGLY");


		
	gr_sortRecords(&graph);	

	if (gr_areSortedRecords(&graph))
		fprintf(stderr,"\n SORTED RECORDS ARE SORTED INCREASINGLY");
	else
		fprintf(stderr,"\n SORTED RECORDS ARE #not# SORTED INCREASINGLY");


	gr_printRecords(&graph);

		/* // CHECK DE QUE NO HAY HUECOS 

		int *contar = (int *) malloc (10000000*sizeof(uint));
		int j;
		//test no huecos en los sujetos
		for (j=0;j<graph.ns;j++) contar[j]=0;	
		for (j=0;j<graph.n;j++)
			contar[graph.s[j*3]]++;

		for (j=0;j<graph.ns;j++) 
			if (contar[j]==0){
				printf("\n hay huecos en los sujetos\n");
				break;
			}
		if (j==graph.ns)
			printf("\n **sin huecos en sujetos**");
	

		//test no huecos en los predicados
		for (j=0;j<graph.np;j++) contar[j]=0;	
		for (j=0;j<graph.n;j++)
			contar[graph.s[j*3+1]]++;

		for (j=0;j<graph.np;j++) 
			if (contar[j]==0){
				printf("\n hay huecos en los predicados\n");
				break;
			}
		if (j==graph.np)
			printf("\n **sin huecos en predicados**");



		//test no huecos en los objetos
		for (j=0;j<graph.no;j++) contar[j]=0;	
		for (j=0;j<graph.n;j++)
			contar[graph.s[j*3+2]]++;

		for (j=0;j<graph.no;j++) 
			if (contar[j]==0){
				printf("\n hay huecos en los objetos\n");
				break;
			}
		if (j==graph.no)
			printf("\n **sin huecos en objetos**");


		free(contar);
		*/

	//printRecords(&graph);
	
	uint *gaps = (uint *) my_malloc (sizeof(uint) * (graph.nEntries+1));
	gaps[0] = 0;
	gaps[1] = graph.ns;
	gaps[2] = graph.ns + graph.np;
	gaps[3] = graph.ns + graph.np +graph.no;  //terminator for suffix sort
	
	//setGaps(&graph, gaps);
	graph.gaps = gaps;
	
	// ---------- End Set Graph properties and read input ---------------
	
			
			
    error = build_WCSA (&graph, params, &index);    		   	
	IFERROR(error);
	
	error = save_index(index, outfile);
	IFERROR(error);
	end = getTime();	
	
	ulong index_len;
	index_size(index, &index_len);

	error = free_index(index);
	IFERROR(error);

	free(params);
	
	//fprintf(stdout,"\n\n\t ## Building time (**parsing into integers + present_layer: %.3f secs", end-start );
	//fprintf(stdout,"\n\t ## Input: %lu bytes --> Output (pres_layer) %lu bytes.", text_len, index_len);
	//fprintf(stdout,"\n\t ## Overall compression --> %.2f%% (%.2f bits per char).\n\n",
    // 			(100.0*index_len)/text_len, (index_len*8.0)/text_len);


	exit(0);
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
	fprintf(stderr, "Usage: %s <source file> <index file> [<parameters>]\n", progname);
	fprintf(stderr, "\nIt builds the index for the text in file <source file>,\n");
	fprintf(stderr, "storing it in <index file>. Any additional <parameters> \n");
	fprintf(stderr, "will be passed to the construction function.\n");
	fprintf(stderr, "At the end, the program sends to the standard error \n");
	fprintf(stderr, "performance measures on time to build the index.\n\n");
	exit(1);
}
