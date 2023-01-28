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



/*
 * Temporary usage: run_queries <index file> <type> [length] [V]
 */
int main (int argc, char *argv[])
{
	int error = 0;
	char *filename;
	char querytype;
	
	if (argc != 2)	{
		usage(argv[0]);	
		exit (1);
	}
	
	filename = argv[1];

	printf("\n Stats of index-dual-RDFCSA: %s\n",argv[1]);
	
	Load_time = getTime ();
	error = load_index_dual (filename, &Index);
	IFERROR (error);
	Load_time = getTime () - Load_time;
	fprintf (stderr, "\tLoad index-dual-RDFCSA time = %.2f secs\n", Load_time);

	error = index_size_dual(Index, &Index_size);
	IFERROR (error);


	
	
	ulong text_len;
	error = get_length_dual(Index, &text_len);
	text_len *= sizeof(uint);
	IFERROR (error);
	
	error = printInfo_dual(Index);
	IFERROR(error);		
	
	error = free_index_dual(Index);
	IFERROR(error);
	


	fprintf(stdout,"\t===============================================\n");
	fprintf(stdout,"\tInput: %lu bytes (assuming uint-values) --> Output %lu bytes (wcsa).\n", text_len, Index_size);
	fprintf(stderr,"\tIndex size = %lu Kb\n", Index_size/1024);
	fprintf(stdout,"\tOverall compression --> %.2f%% (%.2f bits per char).\n",
     			(100.0*Index_size)/text_len, (Index_size*8.0)/text_len);
	fprintf(stdout,"\tOverall compression --> %.2f%% (%.2f bits per triple).\n",
     			(100.0*Index_size)/text_len, (Index_size*12.0)/text_len);
	fprintf(stdout,"\t===============================================\n");


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
