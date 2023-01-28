#ifndef _GRAPHREADER
#define _GRAPHREADER

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>
#include <string.h>



#include "interface.h"
/* macro to detect and notify errors  */
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}


/* @2023: loads source data and formats it into a graph structu, adding both a dummie triple and a terminator */
int parseFileHDTformat_IntoGraph(const char *infilebase, struct graphDB *graph);

int parseFileADRIANformat_IntoGraph(const char *infilebase, struct graphDB *graph);
	int readNS_NO_NSO_gapobjects (const char * filename, char *extension, uint *ns, uint *nso, uint *no, uint *gapobjects);


int gr_readHeader	(struct graphDB *graph, FILE *f);
int gr_readRecords(struct graphDB *graph, FILE *f);
void gr_printRecords(struct graphDB *graph);
int gr_areSortedRecords(struct graphDB *graph);
void gr_sortRecords(struct graphDB *graph) ;



int gr_graphsuffixCmp(const void *arg1, const void *arg2) ;
int gr_graphsuffixCmp_PSO(const void *arg1, const void *arg2);

#endif
