


/* buildFacade.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * BuildFacade.c: Implementation of the functions that the word-based self-index
 *   must implement following the "../pizza/interface.h" guidelines.
 *
 * See more details in:
 * Antonio Fariña, Nieves Brisaboa, Gonzalo Navarro, Francisco Claude, Ángeles Places, 
 * and Eduardo Rodríguez. Word-based Self-Indexes for Natural Language Text. ACM 
 * Transactions on Information Systems (TOIS), 2012. 
 * http://vios.dc.fi.udc.es/indexing/wsi/publications.html
 * http://www.dcc.uchile.cl/~gnavarro/ps/tois11.pdf	  
 * 
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
#include "utils/errors.c"
#include "graphReader.c"

// typedef struct {
// 	twcsa *spo;
// 	twcsa *ops;
// } tdualrdfcsa;
// 


	
/** ******************************************************************************
    * INI functions in Interface dual RDFCSA spo & ops.
*********************************************************************************/
#include "buildFacadeGraphOPS.c"


void testAdrian(uint32_t a, uint32_t b) {
		printf("\nAcabo de sumar %u + %u = %u\n", a,b, a+b);
}

#define FROM_HDT_PARSER_SOURCE_TYPE    1
#define FROM_ADRIAN_PARSER_SOURCE_TYPE 2


// returns 1, 2 
//     1: build_options does NOT include a param : sourceFormat=ADRIAN  (i.e. regular scripts from original)
//     2: build_options includes a param         : sourceFormat=ADRIAN: (i.e. source format for LTJ tests @ 2023).

int getSourceFileType(char *build_options) {
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;

	int sourceFormatType = FROM_HDT_PARSER_SOURCE_TYPE;
	
	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  
		if ((strcmp(parameters[j], "sourceFormat") == 0 ) && (j < num_parameters-1) ) {
			if(strcmp(parameters[j+1], "ADRIAN") ==0) sourceFormatType =  FROM_ADRIAN_PARSER_SOURCE_TYPE;			
		} 	
		j++;
	}
	free_parameters(num_parameters, &parameters);
	}		

	return sourceFormatType;
	
}

char * describeSourceFileType(int type) {
	static char msg1[] = "input graphDB format is the regular one  (from HDT-parser)";
	static char msg2[] = "input graphDB format is the Adrian's one (from LTJ-project)";
	static char msg3[] = "undefined source format data for graphDB";
	
	if (type == FROM_HDT_PARSER_SOURCE_TYPE)         { return msg1;}
	else if (type == FROM_ADRIAN_PARSER_SOURCE_TYPE) { return msg2;}
	return msg3;
}


/* creates first the spo permutation and the reutilizes s to create the ops permutation index */
int build_index_dual (char *filename, char *build_options, void **index) {      
//int build_index_dual (struct graphDB *graph, char *build_options, void **index)  {
	
	struct graphDB graph;


	int sourceFormatType = getSourceFileType(build_options);
			printf("\033[1;31m\n");
		printf("\n declared source data type is: \n\t\t %s \n",describeSourceFileType(sourceFormatType));
			printf("\033[0m\n");
			fflush(stdout);fflush(stderr);	

	//reads source data and adds dummie triple and terminator.        //@ 2023 - 
	if(sourceFormatType == FROM_HDT_PARSER_SOURCE_TYPE) { 
		    parseFileHDTformat_IntoGraph(filename,&graph);
	}
	else {	parseFileADRIANformat_IntoGraph(filename,&graph);     /****************/
		printf("\n parsing pending ... exitting \n"); //exit(0);
	}
	
	
	tdualrdfcsa *dualrdf;
	dualrdf = (tdualrdfcsa *) my_malloc (sizeof (tdualrdfcsa) * 1);


	//backups up graph.s and sets new gaps2 for ops permutation.
	
	uint *s = (uint *) my_malloc ((graph.nEntries * (graph.n) +1) * sizeof(uint)) ;  
	memcpy(s,graph.s, ((graph.nEntries * (graph.n) +1) * sizeof(uint))  );
			
	int *gaps2 = (int *) my_malloc (sizeof(int) * (graph.nEntries+1));
	
//	if(sourceFormatType == FROM_HDT_PARSER_SOURCE_TYPE) { 
//		gaps2[0] = 0;                      //objects
//	}
//	else  {  //ADRIAN-TYPE-FORMAT.
		gaps2[0] = 0-graph.gapobjects; //0 for HDT-PARSER-FORMAT; //objects
//	}
		gaps2[1] = graph.no;                                   //predicates 
		gaps2[2] = graph.no + graph.np;                        //subjects    
		gaps2[3] = graph.ns + graph.np + graph.no;             //terminator for suffix sort!

    //------------- 
 
 
	//builds permutation spo
	build_index_from_graph (&graph, build_options,  (void **) &dualrdf->spo); 		


	//restores graph-> and sets new gaps2
	graph.gaps = gaps2;
	graph.s = s;
	
	//builds permutation ops
	shiftIntoOPS_order(s, graph.n, graph.nEntries);
	gr_sortRecordsOPS(&graph);
	
		if (gr_areSortedRecordsOPS(&graph)) {
			fprintf(stderr,"\nGRAPH OPS-- SORTED RECORDS ARE SORTED INCREASINGLY");
		} else {
			fprintf(stderr,"\nGRAPH OPS-- SORTED RECORDS ARE #not# SORTED INCREASINGLY");
		}
        //we reuse the same "build_index", and therefore, gaps [0,1,2] are offset for objects, predicates and subjects.
        //in the rdfcsa with ops permutation.
	build_index_from_graph (&graph, build_options,  (void **) &dualrdf->ops); 	
	
		//now updates wcsaops->gaps so that gaps [0,1,2] are respectively the offsets to subjects, predicates and objects.
		//this enables using a unique mapID and unmapID both for spo and ops rdfcsa's
		
	twcsa *wcsaops = (twcsa *) dualrdf->ops;
	int gaps_s_build=gaps2[2];    // offset for subjects ( = no + np)
	int gaps_o_build=gaps2[0];    // offset for objects  ( = zero  - graph.gapobjects  )
	
	//	wcsaops->gaps[0] = gaps2[2];       // offset for subjects ( = no + np)
	//	//wcsaops->gaps[1] = gaps2[1];     // offset for predicat ( = no     )  -- unchanged.
	//	//wcsaops->gaps[2] = 0       ;       // offset for objects  ( = zero   )
	//	wcsaops->gaps[2] = 0-graph.gapobjects;       ;       // offset for objects  ( = zero   )
	//	
	//	//wcsaops->gaps[2] = gaps2[0];  <-- si pongo esto falla !! PORQUE AHORA gaps2[0] lo he modificado porque wcsa->gaps apunta a gaps2
	//	

	wcsaops->gaps[0] = gaps_s_build;   // offset for subjects is shifted to gaps[0] from gaps[2]
	//wcsaops->gaps[1] = gaps2[1];     // offset for predicat remains unchanged.
	wcsaops->gaps[2] = gaps_o_build;       ;       // offset for objects  ( = zero   )

	
	
	
	    //>> save_index () properly saves this last "gaps[] array" for the ops-rdfcsa. 
		
	*index = dualrdf;
	return 0;
}

/* returns the name of the baseName for files storing twcsa for ops permutation */
char *dualbasename(const char *filename) {
	static char dualfile[10000];
	sprintf(dualfile,"%s-%s",filename,DUAL_FILE_EXT);
	return dualfile; 
}

int save_index_dual (void *index, char *filename) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	
	save_index ( (void *) dualrdf->spo, filename);
	save_index ( (void *) dualrdf->ops, dualbasename(filename)); 
	return 0;
}


int load_index_dual (char *filename, void **index) {
	tdualrdfcsa *dualrdf;
	dualrdf = (tdualrdfcsa *) my_malloc (sizeof (tdualrdfcsa) * 1);
	
	load_index (filename, (void **) &dualrdf->spo);
	load_index (filename, (void **) &dualrdf->ops);
	
	*index = dualrdf;
	return 0;
}

int free_index_dual (void *index) {
	
//	testRecoverAndCompareSPO_OPS(index);	

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	free_index ( (void *) dualrdf->spo);
	free_index ( (void *) dualrdf->ops);
	
	my_free(dualrdf);
	return 0;
}

int index_size_dual(void *index, ulong *size) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	ulong sizespo,sizeops;
	index_size( (void *) dualrdf->spo, &sizespo);
	index_size( (void *) dualrdf->ops, &sizeops);
	
	*size = sizespo+sizeops;
	return 0;
}
int get_length_dual(void *index, ulong *length) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	ulong lengthspo,lengthops;
	get_length( (void *) dualrdf->spo, &lengthspo);
	get_length( (void *) dualrdf->ops, &lengthops);
	
	*length = lengthspo+lengthops;
	return 0;
}


int testRecoverAndCompareSPO_OPS(void *index) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	
	uint   *data1, *data2;
	size_t   len1,   len2;

	fflush(stderr);fflush(stdout);
	printf("\n\n ------- INI: COMPARING SPO and OPS recovered sequences ---------\n");
	dumpSourceData(dualrdf->spo, &data1, &len1);     	
	dumpSourceDataOPS_to_SPO(dualrdf->ops, &data2, &len2);

			
	printf("\n len-spo = %zu, len-ops = %zu",len1,len2);
	size_t i;
	for (i=0; i< len1; i++) {
		if (data1[i] != data2[i]) {
			printf("\n --DISTINTOS (pos %zu)!! --\n",i);
			break;
		}
	}
	if (i==len1) {
		printf("\n \t **** SPO and OPS->SPO sequences are identical: %zu uints *** \n",len1);
		printf("\n \t\tSPO.data[0..6] = <%u, %u,%u><%u,%u,%u>", data1[0],data1[1],data1[2],data1[3],data1[4],data1[5]);
		printf("\n \t\tOPS-shifted-SPO.data[0..6] = <%u, %u,%u><%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5]);
		printf("\n \t\tthe above data1 and data2 should be identical :D\n");
	}
	printf("\n\n ------- END: COMPARING SPO and OPS recovered sequences ---------\n");
	fflush(stderr);fflush(stdout); 
	free(data1); free(data2);
	
	return 0;
}




/** ******************************************************************************
    * ENDS Interface dual RDFCSA spo & ops.
*********************************************************************************/








/** Building the index */

//private function called by build_index(filename,...) and build_index_dual(filename,...)

int build_index_from_graph (struct graphDB *graph, char *build_options, void **index) {
	int returnvalue;
	
	//struct graphDB graph;
    printf("\n parameters: \"%s\"\n",build_options); fflush(stderr);

//	//reads source data and adds dummie triple and terminator.
//	parseFileHDTformat_IntoGraph(filename,&graph);
        
    returnvalue = build_WCSA (graph, build_options, index);
 
    if (!returnvalue) 
    	returnvalue = build_iCSA (build_options,*index);

    return returnvalue;  
}



    /* Creates index from a file (or various files whose prefix-name is filename)
      Note that the index is an 
      opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned index is ready to be queried. */
      
int build_index (char *filename, char *build_options, void **index) {      
	//int build_index (struct graphDB *graph, char *build_options, void **index) {
	int returnvalue;
	
	struct graphDB graph;
	//     printf("\n parameters: \"%s\"\n",build_options); fflush(stderr);


	int sourceFormatType = getSourceFileType(build_options);
			printf("\033[1;31m\n");
		printf("\n declared source data type is: \n\t\t %s \n",describeSourceFileType(sourceFormatType));
			printf("\033[0m\n");
			fflush(stdout);fflush(stderr);	

	//reads source data and adds dummie triple and terminator.        //@ 2023 - 
	if(sourceFormatType == FROM_HDT_PARSER_SOURCE_TYPE) { 
		    parseFileHDTformat_IntoGraph(filename,&graph);
	}
	else {	parseFileADRIANformat_IntoGraph(filename,&graph);     /****************/
		printf("\n parsing pending ... exitting \n"); exit(0);
	}


	returnvalue = build_index_from_graph (&graph,build_options, index);
    return returnvalue;  
}


	/**  Saves index on disk by using single or multiple files, having 
	proper extensions. */
int save_index (void *index, char *filename) {
	char *basename = filename;
	twcsa *wcsa=(twcsa *) index;

	char *outfilename;
	int file;

	printf("\n Saving structures to disk: %s.*",filename);			
	outfilename = (char *)my_malloc(sizeof(char)*(strlen(basename)+10));

	/**dummy file for dcaro-script --> NOT NECESSARY **/ 
	/*
	{
		strcpy(outfilename, basename);
		unlink(outfilename);
		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}	
		char msg[256];
		sprintf(msg, "G-icsa structure :: %s\n",filename);
		int write_err = write(file,msg,strlen(msg));
		close(file);
	}	
	*/

	/**File with some constants  */
	{
		strcpy(outfilename, basename);
		strcat(outfilename, ".");
		strcat(outfilename, CONSTANTS_FILE_EXT);
		unlink(outfilename);
		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}	
		int write_err=0;
		write_err=write(file, &(wcsa->nEntries), sizeof(uint));
		write_err=write(file, wcsa->gaps, sizeof(int) * (wcsa->nEntries+1));
		write_err=write(file, &(wcsa->ns), sizeof(uint));
		write_err=write(file, &(wcsa->np), sizeof(uint));
		write_err=write(file, &(wcsa->no), sizeof(uint));
		write_err=write(file, &(wcsa->nso), sizeof(uint));
		write_err=write(file, &(wcsa->n), sizeof(size_t));	
		write_err=write(file, &(wcsa->ssize), sizeof(size_t));
		
		close(file);		
	}	
	
			
	my_free_array(outfilename);

	if (wcsa->myicsa) {
		/******** saves index on integers (bottom layer) ******/
		saveIntIndex((void *) wcsa->myicsa, basename);		
		fprintf(stderr, " ** iCSA saved!\n");
	}	
	
	if (wcsa->s) {
		saveSEfile(basename, wcsa->s, (ulong) wcsa->ssize+1);
		//free(wcsa->se);	
		fprintf(stderr, " ** SE saved!\n");	
	}

	return 0;
}

    /**  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
int load_index(char *filename, void **index){
	twcsa *wcsa;
	wcsa = loadWCSA (filename);
	(*index) = (void *) wcsa;
	return 0;
}

	/** Frees the memory occupied by index. */
int free_index(void *index){
	twcsa *wcsa=(twcsa *) index;
	ulong size=0;
	index_size(index,&size);
	fflush(stderr);
	fflush(stdout);
	printf("\n[destroying index] ...Freed %lu bytes... RAM", size);	

	//frees gaps array
	my_free_array(wcsa->gaps);
	
	//frees the array SE.
	if (wcsa->s)
		my_free_array (wcsa->s);

	//the iCSA
	if (wcsa->myicsa) {
		int err = freeIntIndex((void *) wcsa->myicsa);
	}
	
	
	
	//the pointer to wcsa.		
	my_free(wcsa);
	return 0;
}

	/** Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size) {

	twcsa *wcsa=(twcsa *)index;

	*size=0;
	*size += sizeof(twcsa);	
	
	if (wcsa->myicsa) {
		size_t nbytes;
		int err = sizeIntIndex((void *) wcsa->myicsa, &nbytes);
		*size += nbytes;
	}
	return 0;	
}

   /** Gives the number of integers in the indexed sequence */
int get_length(void *index, ulong *length) {
	twcsa *wcsa=(twcsa *) index;
	*length = wcsa->ssize;
	return 0;
}

   /** Gives the number of integers in the indexed sequence */
int length (void *index, ulong *length) {
	return (get_length(index,length));
}
/** *******************************************************************************
  * Showing some statistics and info of the index
  * *******************************************************************************/

		/* Shows summary info of the index */
		/* Shows summary info of the index */
int printInfo(void *index) {

	twcsa *wcsa = (twcsa *) index;
	
	unsigned long indexSize;
	size_t intIndexSize;
	int err;
	
	err = index_size(index, &indexSize);
	if (err!=0) return err;
	err = sizeIntIndex(wcsa->myicsa, &intIndexSize); 	
	if (err!=0) return err;
	
		printf("\n   Vocabulary info -----");
//		printf("\n      Num entries in map[]   (map_size)  = %zu",(size_t) wcsa->map_size);
//		printf("\n      Num entries in unmap[] (zeronode)  = %zu",(size_t) wcsa->zeronode);

//		printf("\n        vocabulary unmap size (zeronode*4)  =   %zu bytes",(size_t) wcsa->zeronode * sizeof(uint));
//		printf("\n        vocabulary map[] size <as uints>    =   %zu bytes",(size_t) (wcsa->zeronode + wcsa->map_size)*sizeof(uint) );
		
//		printf("\n   -----");
//		printf("\n   ");


		printf("\n   wcsa structure = %zu bytes", (size_t) sizeof(twcsa));
		printf("\n   no map[]/unmap[] structures needed (no vocabulary/permutation stored)");

		printf("\n\n @@ Summary of self-index on Integers:");
		err = printInfoIntIndex(wcsa->myicsa, " ");
		if (err!=0) return err;
 			
		printf("\n ========================================================:");		
		printf("\n ** TOTAL SIZE IN MEMORY WHOLE STRUCTURE= %zu bytes **", (size_t) indexSize);
		printf("\n ========================================================:");		
		printf("\n\n");
		return 0;
	}




/** ***********************************************************************************
	 CONSTRUCTION OF THE INDEX WCSA
    ***********************************************************************************/

/**
  * BUILDS THE WCSA INDEX
  */

int build_WCSA (struct graphDB *graph, char *build_options, void **index) {
	twcsa *wcsa;
	wcsa = (twcsa *) my_malloc (sizeof (twcsa) * 1);

	size_t i,j;
	
	wcsa->nEntries = graph->nEntries;
	wcsa->gaps     = graph->gaps;
	//for (j=0;j<=wcsa->nEntries; j++) wcsa->gaps[j]+=1 ;  //we want ID=0 to use it as a marker for searches
	wcsa->ns    = graph->ns;
	wcsa->np    = graph->np;
	wcsa->no    = graph->no;
	wcsa->nso   = graph->nso;

	wcsa->n        = graph->n;       
	wcsa->s 	   = graph->s;
	wcsa->ssize	   = graph->nEntries*graph->n;

	{
		uint mins = 1999999999, minp = 1999999999, mino=1999999999;
		uint maxs = 0, maxp = 0, maxo=0;

		for (i=0; i< wcsa->n; i++) {
				if (wcsa->s[i*(wcsa->nEntries) + 0]  > maxs) maxs = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  > maxp) maxp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  > maxo) maxo = wcsa->s[i*(wcsa->nEntries) + 2];

				if (wcsa->s[i*(wcsa->nEntries) + 0]  < mins) mins = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  < minp) minp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  < mino) mino = wcsa->s[i*(wcsa->nEntries) + 2];
		}
		
		printf("\n SUBJECTS: minID found in source= %u, maxID= %u :: collection-S = %u", mins, maxs, wcsa->ns);
		printf("\n PREDICTS: minID found in source= %u, maxID= %u :: collection-P = %u", minp, maxp, wcsa->np);
		printf("\n OBJJECTS: minID found in source= %u, maxID= %u :: collection-O = %u", mino, maxo, wcsa->no);
		printf("\n SUBJECTS-OBJECTS is %u ", wcsa->nso);
	}

   printf("\n GAPS=  ");
   printf("\n GAPS[0]= %d", wcsa->gaps[0]);
   printf("\n GAPS[1]= %d", wcsa->gaps[1]);
   printf("\n GAPS[2]= %d", wcsa->gaps[2]);
   printf("\n GAPS[3]= %d", wcsa->gaps[3]);



   printf("\n\n antes de sumar gaps ");
   printf("\n SE[0]= %u", wcsa->s[0]);
   printf("\n SE[1]= %u", wcsa->s[1]);
   printf("\n SE[2]= %u", wcsa->s[2]);
   printf("\n SE[3]= %u", wcsa->s[3]);
   printf("\n SE[4]= %u", wcsa->s[4]);
   printf("\n SE[5]= %u", wcsa->s[5]);
   printf("\n SE[6]= %u", wcsa->s[6]);
   printf("\n SE[7]= %u", wcsa->s[7]);

	// ADDS + GAPS[j] to each S[i] value but the dummie node (the 1st one)
	i=0;
		for (j=0;j<wcsa->nEntries;j++)
			wcsa->s[0+j] += wcsa->gaps[j]; 			

	for (i=1; i< wcsa->n; i++) {
		for (j=0;j<wcsa->nEntries;j++)
			wcsa->s[i*(wcsa->nEntries) + j] += wcsa->gaps[j] ; // +1; 
	}


   printf("\n\n tras sumar gaps ");
   printf("\n SE[0]= %u", wcsa->s[0]);
   printf("\n SE[1]= %u", wcsa->s[1]);
   printf("\n SE[2]= %u", wcsa->s[2]);
   printf("\n SE[3]= %u", wcsa->s[3]);
   printf("\n SE[4]= %u", wcsa->s[4]);
   printf("\n SE[5]= %u", wcsa->s[5]);
   printf("\n SE[6]= %u", wcsa->s[6]);
   printf("\n SE[7]= %u", wcsa->s[7]);
    
   //terminator for suffix sorting (greater than all the other values in s[])   	
	wcsa->s[wcsa->ssize] = wcsa->gaps[wcsa->nEntries]; //terminator for suffix sorting (greater than all the other values in s[])



   printf("\n SE[%zu]= %u",wcsa->ssize-3, wcsa->s[wcsa->ssize-3]);
   printf("\n SE[%zu]= %u",wcsa->ssize-2, wcsa->s[wcsa->ssize-2]);
   printf("\n SE[%zu]= %u",wcsa->ssize-1, wcsa->s[wcsa->ssize-1]);
   printf("\n SE[%zu]= %u",wcsa->ssize-0, wcsa->s[wcsa->ssize]);


	{
		uint mins = 1999999999, minp = 1999999999, mino=1999999999;
		uint maxs = 0, maxp = 0, maxo=0;

		for (i=0; i< wcsa->n; i++) {
				if (wcsa->s[i*(wcsa->nEntries) + 0]  > maxs) maxs = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  > maxp) maxp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  > maxo) maxo = wcsa->s[i*(wcsa->nEntries) + 2];

				if (wcsa->s[i*(wcsa->nEntries) + 0]  < mins) mins = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  < minp) minp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  < mino) mino = wcsa->s[i*(wcsa->nEntries) + 2];
		}
		
		printf("\n SUBJECTS: minID found in source2= %u, maxID= %u :: collection-S = %u", mins, maxs, wcsa->ns);
		printf("\n PREDICTS: minID found in source2= %u, maxID= %u :: collection-S = %u", minp, maxp, wcsa->np);
		printf("\n OBJJECTS: minID found in source2= %u, maxID= %u :: collection-S = %u", mino, maxo, wcsa->no);
		printf("\n SUBJECTS-OBJECTS-2 is %u ", wcsa->nso);
		fflush(stdout);
	}

	
	//if no holes appear, that is enough... otherwise we need a dictionary.
	//for RDF it would not be needed...
		
	//wcsa->map=map;
	//wcsa->map_size = map_size;
	//wcsa->unmap = unmap;
	//wcsa->zeronode = zeronode;
	/**/
		
		
	wcsa->myicsa = NULL;
	
	*index = wcsa;
	return 0;
}


int build_iCSA (char *build_options, void *index)
{
	twcsa *wcsa = (twcsa *) index;
	/********* creates the self-index on ints (bottom layer) *********/
	//creating CSA from Edu's code...
	{
		size_t total;
		fprintf(stderr,"\n **** CREATING CSA-bottom-layer *****");
		void *bottomIntIndex;
		int err =  buildIntIndex(wcsa->s,wcsa->ssize+1, wcsa->nEntries, build_options,(void **)&bottomIntIndex);				
		wcsa->myicsa = bottomIntIndex;

		err = sizeIntIndex((void *) wcsa->myicsa, &total);

		printf("\n\t**** [iCSA built on %zu integers (included dummy (3)). Size = %zu bytes... RAM\n",(size_t) wcsa->ssize, total);fflush(stdout);
	}		
	return 0;
}


	/** saves the content of the file SE (ids of the source words) **/
int saveSEfile (char *basename, uint *v, ulong n) {
	char outfilename[255];
	FILE *file;
	sprintf(outfilename,"%s.%s",basename,SE_FILE_EXT);
	unlink(outfilename);
	if( (file = fopen(outfilename, "w")) ==NULL) {
		printf("Cannot open file %s\n", outfilename);
		exit(0);
	}

	//write(file, v, sizeof(uint) * n );
	int err= fwrite(v,sizeof(uint),n,file); 
	fclose(file);
	return 0;
}


/** ********************************************************************
  * Loading from disk
  **********************************************************************/ 

/**-----------------------------------------------------------------  
 * LoadWCSA.                                                       
 * Loads all the data structures of WCSA (included the icsa)     
 ----------------------------------------------------------------- */ 

twcsa *loadWCSA(char *filename) {
	twcsa *wcsa;
	
	wcsa = (twcsa *) my_malloc (sizeof (twcsa) * 1);
	wcsa->n=0;

	int err = loadIntIndex(filename, (void **)&wcsa->myicsa);
	
	int sel = getSelecticsa(wcsa->myicsa, 2822333);	
	printf("got sel = %d\n", sel);
	loadStructs(wcsa,filename);   									  

	return wcsa;
}

/** ------------------------------------------------------------------
 * LoadStructs.
 *	Reads files and loads all the data needed for searcherFacade
 ----------------------------------------------------------------- */ 
 void loadStructs(twcsa *wcsa, char *basename) {
	
	char *filename;
	int file;
		
	filename = (char *)my_malloc(sizeof(char)*(strlen(basename)+10));
	fprintf(stderr,"Loading Index from file %s.*\n", basename);
	
	/**File with some constants  */
	{	strcpy(filename, basename);
		strcat(filename, ".");
		strcat(filename, CONSTANTS_FILE_EXT);
		
		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}

	int read_err=0;
		read_err= read(file, &(wcsa->nEntries), sizeof(uint));
		wcsa->gaps = (int *) my_malloc(sizeof(int) * (wcsa->nEntries+1));
		read_err= read(file, wcsa->gaps, sizeof(int) * (wcsa->nEntries+1));			
		read_err= read(file, &(wcsa->ns), sizeof(uint));			
		read_err= read(file, &(wcsa->np), sizeof(uint));
		read_err= read(file, &(wcsa->no), sizeof(uint));
		read_err= read(file, &(wcsa->nso), sizeof(uint));
		read_err= read(file, &(wcsa->n), sizeof(size_t));			
		read_err= read(file, &(wcsa->ssize), sizeof(size_t));
				
		close(file);
	}		
	
	printf("\nLOAD: ns = %u, np=%u, no=%u, total = %u",wcsa->ns, wcsa->np, wcsa->no,   wcsa->ns + wcsa->np + wcsa->no); 	
	printf("\nLOAD: gaps=");
	{uint i;
		for (i=0;i<=3;i++) printf("[%d]",wcsa->gaps[i]);
	}
	printf("\n");
		
	wcsa->s= NULL;	
	my_free_array(filename);
}

	


/** ****************************************************************
  * Querying the index WCSA
  * ***************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//					   FUNCTIONS NEEDED FOR SEARCHING A PATTERN    					 //
///////////////////////////////////////////////////////////////////////////////////////




/** Querying the index =============================================================*/
	



uint mapID (twcsa *g, uint value, uint type) {
	return value + g->gaps[type];	
}

uint unmapID (twcsa *g, uint value, uint type) {
	return value - g->gaps[type];
	
}





/**************************/
//** debugging only

uint get_num_subjects(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->ns;
}

uint get_num_predicates(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->np;
}

uint get_num_objects(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->no;
}

uint get_num_subjects_objects(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->nso;
}

uint get_num_nodes(void *gindex) {   //para borrar no se usa!
	return 0;
}

/*  ------------*/
/******** end debugging only **********************/





/*************************************************************************/
/******** RDF GRAPHS ****************************************************/
/*************************************************************************/
/*
#ifdef EXPERIMENTS
#include "opssearchRDF_experiments.c"
    //more optimized ... but little improvements !!
#else
#include "opssearchRDF.c"
	//with some tests ... and only slightly slower !!
#endif
*/
/*************************************************************************/

#include "opssearchRDF_experiments.c"




















	/** ------------------------------------------------------------------
	 * recovers the source text by calling display(0,fileSize);
	 * ------------------------------------------------------------------ */


//-----------------------------------------------------------------------------------//
// needed for qsort of entries in dumpSourceDataSorted function
uint nentries_buildFacade; // Global variable used in quicksort graphsuffixCmp function
int graphsuffixCmp_buildFacade(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	for (i=0; i<nentries_buildFacade;i++) {
		if (*a <*b) return -1;
		if (*a >*b) return +1;
		a++;b++;
	}
	//if (i== nentries_buildFacade) 
	return 0;

}

void sortRecords_buildFacade(twcsa *wcsa, uint*buffer) {
	nentries_buildFacade= wcsa->nEntries;
	qsort(buffer, wcsa->n, sizeof(uint)*wcsa->nEntries, graphsuffixCmp_buildFacade);
}
//-----------------------------------------------------------------------------------//
	


void dumpSourceData(twcsa *wcsa, uint **data, size_t *len) {
	uint * buffer;
	dumpICSASequence(wcsa->myicsa, &buffer, len);

	size_t i,j,z=0;
	for (i=0; i<wcsa->n; i++){
		for (j=0;j<wcsa->nEntries;j++) {
			//buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			buffer[z] = unmapID(wcsa,buffer[z],j);
			z++;
		}
	}		
	uint *data2 = buffer;
	printf("\n \t\tSPO.in.SPOorder[0..6] = <%u, %u,%u> <%u,%u,%u> <%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5],data2[6],data2[7],data2[8]);
	
	*data = buffer;
	*len = *len -1;
}


//dual 2023 --
//dumps data from a twcsa in OPS order, and recreates a sort SPO sequence (in spo order).
void dumpSourceDataOPS_to_SPO(twcsa *wcsa, uint **data, size_t *len){
	uint * buffer;
	dumpICSASequence(wcsa->myicsa, &buffer, len);

	size_t i,j,z=0;
	for (i=0; i<wcsa->n; i++){
		for (j=0;j<wcsa->nEntries;j++) {
			//buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			buffer[z] = unmapID(wcsa,buffer[z],wcsa->nEntries-1-j);
			z++;
		}
	}		
	
	uint *data2 = buffer;
	
	printf("\n \t\tCALL TO dumpSourceOPS_to_SPO DATA \n");
	printf("\n \t\tOPS.data[0..6] = <%u, %u,%u><%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5]);

	shiftIntoOPS_order(buffer,wcsa->n, wcsa->nEntries);     //shifts OPS sequence into an SPO sequence
	array_sortRecordsSPO(buffer, wcsa->n, wcsa->nEntries);  //orders SPO sequence by s->p->o order.
	printf("\n \t\tOPS.data-shiftedSPO-and-inSPOorder[0..6] = <%u, %u,%u> <%u,%u,%u> <%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5],data2[6],data2[7],data2[8]);
	
	*data = buffer;
	*len = *len -1;
}








