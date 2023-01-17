/*
 * This code was originally made by Guillermo de Bernardo <guillermo.debernardo@udc.es>
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>

#include "timing.h"
#include "interface.h"
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

#include "debug.h"
#include "arraysort.c"  //@@


#define Q_SPO 1
#define Q_SP 2
#define Q_PO 3
#define Q_SO 4
#define Q_S  5
#define Q_P  6
#define Q_O  7




int main(int argc, char ** argv) {        

	void *index;
	/**/ char operation[10];

	int totalres;
	if (argc < 3) {
			printf("Usage: %s <graphIdx> <tipoQuery> \n", argv[0]);
			exit(1);
	}

	char * filename = argv[1]; //file: associated to the graph-index.
/**/char *indexbasename = argv[1];

	ulong Index_size, Text_length;
	int error= load_index (filename, &index);
	IFERROR (error);
	error = index_size(index, &Index_size);
	IFERROR (error);
	error = get_length(index, &Text_length);
	Text_length *= sizeof(uint);
	IFERROR (error);
	fprintf(stderr, "Index size = %lu Kb\n", Index_size/1024);
	//@@

	int queryType = atoi(argv[2]);
		
	uint * res = (uint *) malloc (sizeof(uint) * (MAX_RESULTS * 3 +1));    //max_results constant defined in "interface.h" file
	if (!res) {
		perror("ERROR MALLOC");
		exit(0);
	}

	uint * res2 = (uint *) malloc (sizeof(uint) * (MAX_RESULTS * 3 +1));    //max_results constant defined in "interface.h" file
	if (!res2) {
		perror("ERROR MALLOC");
		exit(0);
	}

	// res[0] = noccs  = number of <triplets to return>
	// res[(1..MAX_RESULTS) + (MAX_RESULTS)*0] = Subjects
	// res[(1..MAX_RESULTS) + (MAX_RESULTS)*1] = Predicates
	// res[(1..MAX_RESULTS) + (MAX_RESULTS)*2] = Objects


		
	startClockTime();
	long results;

			switch(queryType) {
				case Q_SPO: {  results =test_spo(index, &res); break;}
				case Q_SP : {  results =test_sp (index, &res, &res2); break;}
				case Q_PO : {  results =test_po (index, &res, &res2); break;}
				case Q_SO : {  results =test_so (index, &res, &res2); break;}
				case Q_S  : {  results =test_s  (index, &res, &res2); break;}
				case Q_P  : {  results =test_p  (index, &res, &res2); break;}
				case Q_O  : {  results =test_o  (index, &res, &res2); break;}
			}

        
	
        
        ulong microsecs = endClockTime()/1000; //to microsecs

        
		printf("\n <index>\t<microsec>");
        printf("\n %s\t%ld\\n", argv[1], microsecs);

					
		free_index(index);
        free(res);
        free(res2);

        exit(0);
}












