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


/* given a source symbol, returns the id associated to it in the vocabulary of used IDS */
uint mapsourcesymbol(uint symbol, void *index) {
	twcsa *wcsa=(twcsa *) index;
	return wcsa->map[symbol];
}

/* given an id (used in SE array), return the source symbol associated to it */
uint unmapsymbol(uint symbol, void *index) {
	twcsa *wcsa=(twcsa *) index;
	return wcsa->unmap[symbol];
	
}


int createCompressedDictionary(void *index, uint samplingUnmap, uint nsHUFF) {
	twcsa *wcsa=(twcsa *) index;
	
	wcsa->cunmap = huffmanCompressPsi(wcsa->unmap,wcsa->zeronode,samplingUnmap,nsHUFF);
	
}

/** Building the index */

    /* Creates index from text[0..length-1]. Note that the index is an 
      opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned index is ready to be queried. */
int build_index (struct graphDB *graph, char *build_options, void **index) {
	int returnvalue;

     printf("\n parameters: \"%s\"\n",build_options); fflush(stderr);
        
    returnvalue = build_WCSA (graph, build_options, index);
 
    if (!returnvalue) 
    	returnvalue = build_iCSA (build_options,*index);

	
	uint samplingUnmap=32;
	uint nsHUFF= 16384;
	
	createCompressedDictionary(*index,  samplingUnmap, nsHUFF);
	
    return returnvalue;  
}


/**  Saves index on disk by using single or multiple files, having 
	proper extensions. */
int save_index (void *index, char *filename) {
	char *basename = filename;
	twcsa *wcsa=(twcsa *) index;

	uint i,j;
	char *outfilename;
	int file;
	char c;

	printf("\n Saving structures to disk: %s.*",filename);			
	outfilename = (char *)my_malloc(sizeof(char)*(strlen(basename)+10));

	/**dummy file for dcaro-script **/
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
	

	/**File with some constants  */
	{ uint number;
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
		write_err=write(file, wcsa->gaps, sizeof(uint) * (wcsa->nEntries+1));
		write_err=write(file, &(wcsa->nodes), sizeof(uint));
		write_err=write(file, &(wcsa->maxtime), sizeof(uint));
		write_err=write(file, &(wcsa->n), sizeof(size_t));	
		write_err=write(file, &(wcsa->ssize), sizeof(size_t));
		
		close(file);		
	}	
	
	/** File with the words from the vocabulary (sorted alphabetically) */
	{			
		strcpy(outfilename, basename);
		strcat(outfilename, ".");
		strcat(outfilename, VOCABULARY_WORDS_FILE_EXT);
		unlink(outfilename);

		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}
		
		//the number of canonical words
		int write_err=0;
		write_err = write(file, &wcsa->map_size, sizeof(uint));
		write_err = write(file, &wcsa->zeronode, sizeof(uint));
		
		write_err = write(file, wcsa->map, sizeof(uint) * wcsa->map_size);
		write_err = write(file, wcsa->unmap, sizeof(uint) * wcsa->zeronode);

		close(file);
	}
	
			
	my_free_array(outfilename);

	if (wcsa->myicsa) {
		/******** saves index on integers (bottom) ******/
		saveIntIndex((void *) wcsa->myicsa, basename);		
	}	
	
	if (wcsa->s) {
		saveSEfile(basename, wcsa->s, (ulong) wcsa->ssize+1);
		//free(wcsa->se);		
	}

	return 0;
}

    /**  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
int load_index(char *filename, void **index){
	twcsa *wcsa;
	wcsa = loadWCSA (filename);
	(*index) = (void *) wcsa;

	uint samplingUnmap=32;
	uint nsHUFF= 16384;	
	createCompressedDictionary(*index,  samplingUnmap, nsHUFF);
	
	return 0;
}

	/** Frees the memory occupied by index. */
int free_index(void *index){
	twcsa *wcsa=(twcsa *) index;
	ulong size;
	index_size(index,&size);
	fflush(stderr);
	fflush(stdout);
	printf("\n[destroying index] ...Freed %lu bytes... RAM", size);	

	//frees gaps array
	my_free_array(wcsa->gaps);
	
	//frees the array SE.
	if (wcsa->s)
		my_free_array (wcsa->s);

	if (wcsa->map)
		my_free_array (wcsa->map);

	if (wcsa->unmap)
		my_free_array (wcsa->unmap);


	destroyHuffmanCompressedPsi(&(wcsa->cunmap));


	//the iCSA.
	if (wcsa->myicsa) {
		int err = freeIntIndex((void *) wcsa->myicsa);
	}
	
	
	
	//the pointer to wcsa.		
	my_free(wcsa);
	return 0;
}

	/** Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size) {
	ulong totaltmp;
	twcsa *wcsa=(twcsa *)index;
	uint n= wcsa->n;
	*size=0;
	*size += sizeof(twcsa);	
	//*size += sizeof(uint) * (wcsa->map_size + wcsa->zeronode);

	//PLAIN INTEGERS
	//*size += sizeof(uint) * (wcsa->zeronode);  // used values
	
	//COMPRESSED UNMAP
	*size += wcsa->cunmap.totalMem;

	//INTEGERS
	//*size += sizeof(uint) * (wcsa->map_size);  //all values
	
	//BITMAP  n + o(n)
	 *size += wcsa->map_size/8 + (wcsa->map_size/256 * sizeof(uint))   +   ((wcsa->map_size/32) * sizeof(char));
	
	
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
	uint n;

	twcsa *wcsa = (twcsa *) index;
	
	unsigned long indexSize;
	size_t intIndexSize;
	int err;
	
	err = index_size(index, &indexSize);
	if (err!=0) return err;
	err = sizeIntIndex(wcsa->myicsa, &intIndexSize); 	
	if (err!=0) return err;
	
	
		printf("\n ===================================================:");		
		printf("\n Summary of indexed Graph-DB-iCSA:");		
		printf("\n   Number of entries (nEntries) per record = %zu",(size_t) wcsa->nEntries);
		printf("\n   Number of records/contacts (n) = %zu",(size_t) wcsa->n);
		printf("\n   Number of indexed integers (size) = %zu",(size_t) wcsa->ssize);

		printf("\n   ");
		
		printf("\n   Vocabulary info -----");
		printf("\n      Num entries in map[]   (map_size)  = %zu",(size_t) wcsa->map_size);
		printf("\n      Num entries in unmap[] (zeronode)  = %zu",(size_t) wcsa->zeronode);

		printf("\n        vocabulary unmap size (zeronode*4)  =   %zu bytes",(size_t) wcsa->zeronode * sizeof(uint));
		printf("\n      ->vocabulary unmap size (compressed)  = **%zu bytes**",(size_t) wcsa->cunmap.totalMem);
			
		printf("\n        vocabulary map[] size <as uints>    =   %zu bytes",(size_t) (wcsa->zeronode + wcsa->map_size)*sizeof(uint) );
		printf("\n      ->vocabulary map[] size <with bitmap> = **%zu bytes**",(size_t) (wcsa->map_size/8 + (wcsa->map_size/256 * sizeof(uint))   +   ((wcsa->map_size/32) * sizeof(char)))  );
		printf("\n   Total vocabulary size = %zu bytes",(size_t) wcsa->cunmap.totalMem + 
		                                                    (wcsa->map_size/8 + (wcsa->map_size/256 * sizeof(uint))   +   ((wcsa->map_size/32) * sizeof(char)))  );
		printf("\n   -----");
		
		printf("\n   ");

		printf("\n   wcsa structure = %zu bytes", (size_t) sizeof(twcsa));


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
	wcsa->nodes    = graph->nodes;     
	wcsa->maxtime  = graph->maxtime;   

	wcsa->n        = graph->n;       
	wcsa->s 	   = graph->s;
	wcsa->ssize	   = graph->nEntries*graph->n;

	// ADDS - GAPS[j] to each S[i] value
	for (i=0; i< wcsa->n; i++) {
		for (j=0;j<wcsa->nEntries;j++)
			wcsa->s[i*(wcsa->nEntries) + j] += wcsa->gaps[j]; 			
	}
	
	//wcsa->s[wcsa->ssize] = wcsa->gaps[wcsa->nEntries]; //terminator for suffix sorting (greater than all the other values in s[])
	
	//if no holes appear, that is enough... otherwise we need a dictionary.
	//for RDF it would not be needed...
	
	
	/** creates the vocabulary of "used values" **/
	// map, unmap, map_size, zeronode are set, and entries in s[] are remapped.
	uint map_size = wcsa->gaps[wcsa->nEntries];
	uint *wordscount = (uint *) my_malloc(sizeof(uint) * map_size);
	uint *map        = (uint *) my_malloc(sizeof(uint) * map_size);

	for (i=0; i< map_size; i++) wordscount[i]=0;
	
	for (i=0; i< wcsa->ssize ; i++)
		wordscount[wcsa->s[i]]++;
		

	uint zeronode=0;
	for (i=0; i< map_size; i++) 
		if (wordscount[i]!=0) zeronode++;
	fprintf(stderr,"\n zeronode %u ",zeronode);
	
		/*
			for (i=0; i< map_size; i++) 
				if (wordscount[i]==0)
					fprintf(stdout,"\n %5zu counts=%u",i,wordscount[i]);
				else 	fprintf(stderr,"\n %5zu counts=%u",i,wordscount[i]);
		*/


	uint *unmap = (uint *) my_malloc(sizeof(uint) * zeronode);
	
	uint mapcount=0;
	for (i=0; i< map_size; i++) {
		map[i]= mapcount;
		if (wordscount[i]!=0) {
				unmap[mapcount]=i;
				mapcount++;
		}
	}
    my_free_array(wordscount);

	assert(mapcount == zeronode);		

		//for checking map/unmmap
		uint *se = (uint *) malloc (sizeof(uint) * wcsa->ssize);
		for (int i=0; i<wcsa->ssize;i++) se[i]=wcsa->s[i];
	
	//remap values in s[]
	for (i=0; i< wcsa->ssize ; i++)
		wcsa->s[i] = map[wcsa->s[i]];
	wcsa->s[wcsa->ssize] = zeronode;
	
		for (int i=0; i<wcsa->ssize;i++) assert (se[i]== unmap[wcsa->s[i]]);
		fprintf(stderr,"\n map and unmap vocabulary arrays created sucessfully");
		free(se);
		
	wcsa->map=map;
	wcsa->map_size = map_size;
	wcsa->unmap = unmap;
	wcsa->zeronode = zeronode;
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

		printf("\n\t**** [iCSA built on %zu integers. Size = %zu bytes... RAM",(size_t) wcsa->ssize, total);
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

	loadStructs(wcsa,filename);   									  

	return wcsa;
}

/** ------------------------------------------------------------------
 * LoadStructs.
 *	Reads files and loads all the data needed for searcherFacade
 ----------------------------------------------------------------- */ 
 void loadStructs(twcsa *wcsa, char *basename) {
	uint i,j;
	char *filename;
	int file;
	uint sizeFile;
	char c;
	uint n;
		
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
		wcsa->gaps = (uint *) my_malloc(sizeof(uint) * (wcsa->nEntries+1));
		read_err= read(file, wcsa->gaps, sizeof(uint) * (wcsa->nEntries+1));			
		read_err= read(file, &(wcsa->nodes), sizeof(uint));			
		read_err= read(file, &(wcsa->maxtime), sizeof(uint));
		read_err= read(file, &(wcsa->n), sizeof(size_t));			
		read_err= read(file, &(wcsa->ssize), sizeof(size_t));
				
		close(file);
	}		
	
	/** File with the words from the vocabulary (sorted alphabetically) */
	{			
		strcpy(filename, basename);
		strcat(filename, ".");
		strcat(filename, VOCABULARY_WORDS_FILE_EXT);
		//sizeFile= fileSize(filename)-sizeof(uint);

		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}
		
		//the number of canonical words
		int read_err=0;
		read_err = read(file, &wcsa->map_size, sizeof(uint));
		read_err = read(file, &wcsa->zeronode, sizeof(uint));
		
		wcsa->map = (uint *) my_malloc(sizeof(uint) * wcsa->map_size);
		wcsa->unmap = (uint *) my_malloc(sizeof(uint) * wcsa->zeronode);		
		read_err = read(file, wcsa->map, sizeof(uint) * wcsa->map_size);
		read_err = read(file, wcsa->unmap, sizeof(uint) * wcsa->zeronode);

		close(file);
	}		
		
	wcsa->s= NULL;	
	my_free_array(filename);
}

	


/** ****************************************************************
  * Querying the index WCSA
  * ***************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//					   FUNCTIONS NEEDED FOR SEARCHING A PATTERN    					 //
///////////////////////////////////////////////////////////////////////////////////////



/*------------------------------------------------------------------
 * Given a text pattern translates it into a list of integers (corresponding to the
 * canonical words associated to the valid words in the text pattern) 
 ------------------------------------------------------------------*/
void parseTextIntoIntegers(twcsa *wcsa, byte *textPattern, uint patLen, uint *integerPattern, uint *sizeIntegers) {
	return;
}



/** Querying the index =============================================================*/
	
	/* Writes in numocc the number of occurrences of the substring 
	   pattern[0..length-1] found in the text indexed by index. */
int count (void *index, uchar *pattern, ulong length, ulong *numocc){
	uint integerPatterns[MAX_INTEGER_PATTERN_SIZE];
	uint integerPatternSize;
	ulong l,r;	

	twcsa *wcsa=(twcsa *) index;
	parseTextIntoIntegers(wcsa, pattern, length, integerPatterns, &integerPatternSize);	
	if (!integerPatternSize) {*numocc=0; return 0;} //not found
	
	//*numocc = countCSA(wcsa->myicsa, integerPatterns, integerPatternSize, &l, &r);
	int err = countIntIndex((void *) wcsa->myicsa, integerPatterns, integerPatternSize, numocc,  &l, &r);
	return 0;
}

#define NODE     (0)
#define NODE_REV (1)
#define TIME_INI (2)
#define TIME_END (3)

//private function to use in "mapID"
uint getmap(twcsa *g, uint value) {
	return g->map[value];
}

//private function to use in "unmapID"
uint getunmap(twcsa *g, uint value) {
	return g->unmap[value];
}

uint mapID (twcsa *g, uint value, uint type) {
	return getmap(g, value +g->gaps[type]);
}


uint unmapID (twcsa *g, uint value, uint type) {

	return getHuffmanPsiValue(&g->cunmap,value) - g->gaps[type];
	
	//return getunmap(g, value) - g->gaps[type] ;
}



/******** TEMPORAL GRAPHS QUERIES FROM DIEGO-CARO ***********************/

/*size_t start(BitSequence *b, uint i) ;
inline uint belong(BitSequence *b, size_t i);
*/
/*
//Returns the set of active neighbors of node "node" in time "time".
// ti<= time <tf
void get_neighbors_point_v0(uint *res, void *gindex, uint node, uint time) {	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_neighbors_point node=%u, time =%u\n",cont, node,time);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint ti = mapID(g,time,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,time,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;

	pattern[0] = u;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		if (!numocc) rti --;

		numocc=0;
		pattern[0] = tf+1;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc)   
			ltf ++;
		
		ulong i;
		for (i=lu; i<=ru;i++) {
			uint p= getPsiicsa(g->myicsa, i);
			uint q= getPsiicsa(g->myicsa, p);
			if (q <=rti) {
				uint r= getPsiicsa(g->myicsa, q);
				if (r >=ltf) {
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE_REV);	

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n nodo i %lu= %u , reverso= %u=%u, t_i= %u, tf=%u ", i,U,V,res[n], TI,TF);
				//@@ Just for debugging
				#endif
				}				
			}
		}		
	}		 
}
*/

//Returns the set of active neighbors of node "node" in time "time".
// ti<= time <tf
void get_neighbors_point(uint *res, void *gindex, uint node, uint time) {	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_neighbors_point node=%u, time =%u\n",cont, node,time);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint ti = mapID(g,time,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,time,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;

	pattern[0] = u;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --;

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc)  rtf ++;
		
		ulong i;
		for (i=lu; i<=ru;i++) {
			uint p= getPsiicsa(g->myicsa, i);
			uint q= getPsiicsa(g->myicsa, p);
			if (q <=rti) {
				uint r= getPsiicsa(g->myicsa, q);
				if (r >rtf) {
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE_REV);	

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n nodo i %lu= %u , reverso= %u=%u, t_i= %u, tf=%u ", i,U,V,res[n], TI,TF);
				//@@ Just for debugging
				#endif
				}				
			}
		}		
	}		 
}

void get_neighbors_interval(uint *res, void *gindex, uint node, uint timestart, uint timeend, uint semantic) {
	if (!semantic)
		get_neighbors_weak(res, gindex, node, timestart, timeend);
	else	
		get_neighbors_strong(res, gindex, node, timestart, timeend);	
}


//Returns the set of active neighbors of node "node" in time  [timestart, timeend)
// (u,v,t1,t2) s.t. [t1,t2) intersect [timestart,timeend) != emptyset
void get_neighbors_weak(uint *res, void *gindex, uint node, uint timestart, uint timeend) {
	//return get_neighbors_interval(res, gindex, node, timestart, timeend, 0);
	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_neighbors_interval_weak:: node=%u, time =[%u,%u)\n",cont, node,timestart, timeend);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint ti1 = mapID(g,timestart,TIME_INI);  // uint x=unmapID(g,ti1,TIME_INI);
	uint ti2 = mapID(g,timeend,TIME_INI);  // uint x=unmapID(g,ti2,TIME_INI);
	uint tf = mapID(g,timestart,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;
   
	pattern[0] = u;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti1;
		ulong lti1,rti1;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti1, &rti1);
		//lti1
		//if (!numocc) rti1 --; 

		numocc=0;
		pattern[0] = ti2;   // time < ti2  (strict range)
		ulong lti2,rti2;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti2, &rti2);
		//lti2
		//if (!numocc) lti2 ++; 

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		//rtf
		if (!numocc) rtf ++; //

		
		ulong i;
		for (i=lu; i<=ru;i++) {
			uint p= getPsiicsa(g->myicsa, i);
			uint q= getPsiicsa(g->myicsa, p);
			
/*			if (timestart == 998) {
						uint r= getPsiicsa(g->myicsa, q);
						uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n nodo i %lu= %u , reverso= %u, t_i= %u, tf=%u ", i,U,V, TI,TF);
			}
*/

			if (q <lti1) {
				uint r= getPsiicsa(g->myicsa, q);
				if (r > rtf) {  //>=ltf
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE_REV);	

					//@@ Just for debugging
					#ifdef DEBUG_FACADE_ON
						uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n nodo i %lu= %u , reverso= %u=%u, t_i= %u, tf=%u ", i,U,V,res[n], TI,TF);
					//@@ Just for debugging
					#endif
				}				
			}
			else { 
				if (q <lti2) {
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE_REV);	

					//@@ Just for debugging
					#ifdef DEBUG_FACADE_ON
						uint r= getPsiicsa(g->myicsa, q);
						uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n nodo i %lu= %u , reverso= %u=%u, t_i= %u, tf=%u ", i,U,V,res[n], TI,TF);
					//@@ Just for debugging
					#endif
				}				
			}
		}		
	}		 	
} 



/*
void get_neighbors_strong2(uint *res, void *gindex, uint node, uint timestart, uint timeend) {
	//return get_neighbors_interval(res, gindex, node, timestart, timeend, 1);
	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_neighbors_interval_strong:: node=%u, time =[%u,%u)\n",cont, node,timestart, timeend);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint ti = mapID(g,timestart,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,timeend,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;

	pattern[0] = u;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --; //

		numocc=0;
		pattern[0] = tf+1;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc)   
			ltf ++; //
		
		ulong i;
		for (i=lu; i<=ru;i++) {
			uint p= getPsiicsa(g->myicsa, i);
			uint q= getPsiicsa(g->myicsa, p);
			if (q <=rti) {
				uint r= getPsiicsa(g->myicsa, q);
				if (r >=ltf) {
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE_REV);	

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n nodo i %lu= %u , reverso= %u=%u, t_i= %u, tf=%u ", i,U,V,res[n], TI,TF);
				//@@ Just for debugging
				#endif
				}				
			}
		}		
	}		 
	
}

*/

//Returns the set of active neighbors of node "node" in time  [timestart, timeend)
// (u,v,t1,t2) s.t. [t1,t2) contains [timestart,timeend) 

void get_neighbors_strong(uint *res, void *gindex, uint node, uint timestart, uint timeend) {
	//return get_neighbors_interval(res, gindex, node, timestart, timeend, 1);
	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_neighbors_interval_strong:: node=%u, time =[%u,%u)\n",cont, node,timestart, timeend);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint ti = mapID(g,timestart,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,timeend,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;

	pattern[0] = u;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --; //

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		//if (!numocc)   rtf ++; //
		
		ulong i;
		for (i=lu; i<=ru;i++) {
			uint p= getPsiicsa(g->myicsa, i);
			uint q= getPsiicsa(g->myicsa, p);
			if (q <=rti) {
				uint r= getPsiicsa(g->myicsa, q);
				if (r >rtf) {
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE_REV);	

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, r) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n nodo i %lu= %u , reverso= %u=%u, t_i= %u, tf=%u ", i,U,V,res[n], TI,TF);
				//@@ Just for debugging
				#endif
				}				
			}
		}		
	}		 
	
}



//Returns the set of active reverse neighbors of node "node" in time  "time"
// (u, node,t1,t2) s.t. t1 <=t <t2
void get_reverse_point(uint *res, void *gindex, uint node, uint time) {

	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_reverse_point node=%u, time =%u\n",cont, node,time);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint v  = mapID(g,node,NODE_REV);
	uint ti = mapID(g,time,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,time,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;

	pattern[0] = v;
	ulong numocc,lv,rv;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lv, &rv);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		if (!numocc) rti ++;

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc) rtf ++;
		
		ulong i;
		for (i=lv; i<=rv;i++) {
			uint q= getPsiicsa(g->myicsa, i);
			if (q <=rti) {
				uint x= getPsiicsa(g->myicsa, q);
				if (x >rtf) {
					uint p= getPsiicsa(g->myicsa, x);
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE);

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, p) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, i) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n nodo %u=%u , reverso= %lu=%u, t_i= %u, tf=%u ", U,res[n], i,V, TI,TF);
				//@@ Just for debugging
				#endif					
				}
			}
		}
	}
}

void get_reverse_interval(uint *res, void *gindex, uint node, uint ts, uint te, uint semantic) {
	if (!semantic)
		get_reverse_weak(res, gindex, node, ts, te);
	else	
		get_reverse_strong(res, gindex, node, ts, te);	
}

//Returns the set of active reverse neighbors of node "node" in time  [timestart, timeend)
// (u, node ,t1,t2) s.t. [t1,t2) intersect [timestart,timeend) != emptyset
void get_reverse_weak(uint *res, void *gindex, uint node, uint ts, uint te) {
	//return get_reverse_interval	(res, gindex, node, timestart, timeend, 0);
	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_reverse_weak:: node=%u, time =[%u,%u)\n",cont, node,ts, te);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint v  = mapID(g,node,NODE_REV);
	uint ti1 = mapID(g,ts,TIME_INI);  // uint x=unmapID(g,ti1,TIME_INI);
	uint ti2 = mapID(g,te,TIME_INI);  // uint x=unmapID(g,ti2,TIME_INI);
	uint tf = mapID(g,ts,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
		
	*res =0;

	pattern[0] = v;
	ulong numocc,lv,rv;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lv, &rv);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti1;
		ulong lti1,rti1;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti1, &rti1);
		//lti1
		//if (!numocc) rti1 --; 

		numocc=0;
		pattern[0] = ti2;   // time < ti2  (strict range)
		ulong lti2,rti2;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti2, &rti2);
		//lti2
		//if (!numocc) lti2 ++; 

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		//rtf
		if (!numocc) rtf ++; //

		
		
		ulong i;
		for (i=lv; i<=rv;i++) {
			uint q= getPsiicsa(g->myicsa, i);
			/*
			if (ts == 4820) {
				uint x= getPsiicsa(g->myicsa, q);
					uint p= getPsiicsa(g->myicsa, x);
						uint U = getRankicsa(g->myicsa, p) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, i) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n nodo %u , reverso= %lu=%u, t_i= %u, tf=%u ", U, i,V, TI,TF);
			}
			*/

			if (q <lti1) {
				uint x= getPsiicsa(g->myicsa, q);
				
				if (x >rtf) {
					uint p= getPsiicsa(g->myicsa, x);
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE);

					//@@ Just for debugging
					#ifdef DEBUG_FACADE_ON
						uint U = getRankicsa(g->myicsa, p) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, i) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n nodo %u=%u , reverso= %lu=%u, t_i= %u, tf=%u ", U,res[n], i,V, TI,TF);
					//@@ Just for debugging
					#endif					
				}
			}
			else {
				if (q < lti2) {
					uint x= getPsiicsa(g->myicsa, q);
					uint p= getPsiicsa(g->myicsa, x);
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE);

					//@@ Just for debugging
					#ifdef DEBUG_FACADE_ON
						uint U = getRankicsa(g->myicsa, p) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, i) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n nodo %u=%u , reverso= %lu=%u, t_i= %u, tf=%u ", U,res[n], i,V, TI,TF);
					//@@ Just for debugging
					#endif					
					
				}
			}
		}
	}	
}


//Returns the set of active reverse neighbors of node "node" in time  [timestart, timeend)
// (u, node, t1,t2) s.t. [t1,t2) contains [timestart,timeend) 
void get_reverse_strong(uint *res, void *gindex, uint node, uint ts, uint te) {	
	//return get_reverse_interval	(res, gindex, node, timestart, timeend, 1);
	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_reverse_strong:: node=%u, time =[%u,%u)\n",cont, node,ts, te);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint v  = mapID(g,node,NODE_REV);
	uint ti = mapID(g,ts,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,te,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	*res =0;

	pattern[0] = v;
	ulong numocc,lv,rv;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lv, &rv);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --;

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc) rtf ++;
		
		ulong i;
		for (i=lv; i<=rv;i++) {
			
			uint q= getPsiicsa(g->myicsa, i);
			if (q <=rti) {
				uint x= getPsiicsa(g->myicsa, q);
				if (x >rtf) {
					uint p= getPsiicsa(g->myicsa, x);
					res[0]++;
					uint n= res[0];
					res[n] = getRankicsa(g->myicsa,p) -1;
					res[n] = unmapID(g,res[n],NODE);

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, p) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, i) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n nodo %u=%u , reverso= %lu=%u, t_i= %u, tf=%u ", U,res[n], i,V, TI,TF);
				//@@ Just for debugging
				#endif					
				}
			}
		}
	}
}

/*  ------------*/

// returns true if edge <u,v> existed in time T 
//         false otherwise
// That is, true if it exists contact <u,v,t1,t2> s.t. t1<=T<t2
int get_edge_point(void *gindex, uint node, uint noderev, uint T) { 
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_edge_point node=%u, node_rev %u time =%u\n",
	                                                       cont, node,noderev,T);fflush (stdout);
	#endif
	
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint v  = mapID(g,noderev,NODE_REV);
	uint ti = mapID(g,T,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,T,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	pattern[0] = u; pattern[1] = v;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 2, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --;

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc) rtf ++;
		
		ulong i;
		for (i=lu; i<=ru;i++) {

			uint p= getPsiicsa(g->myicsa, i);   //node_rev
			uint q= getPsiicsa(g->myicsa, p);   //ti
			if (q <=rti) {
				uint x= getPsiicsa(g->myicsa, q);  //tf
				if (x >rtf) {

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n EDGE found for t= %u: nodo =%u , reverso= %u, t_i= %u, tf=%u ", T, U,V, TI,TF);
					fflush(stdout);fflush(stderr);
				//@@ Just for debugging
				fprintf(stderr,"\n");
				#endif
				return 1;
				}				
			}
		}
	}
	
	return 0;
}

// returns true if edge <u,v> existed in the whole period [t,t') 
//         false otherwise
// That is, true if it exists contact <u,v,t1,t2> s.t. [t1,t2) includes [t,t')

int get_edge_strong(void *gindex, uint node, uint noderev, uint tstart, uint tend) {
//        return get_edge_interval(gindex, u, v, tstart, tend, 1);   
#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_edge_interval_strong node=%u, node_rev %u ts =%u,te=%u\n",
	                                                       cont, node,noderev,tstart,tend);fflush (stdout);
#endif
	
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint v  = mapID(g,noderev,NODE_REV);
	uint ti = mapID(g,tstart,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,tend,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	pattern[0] = u; pattern[1] = v;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 2, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --;

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc) rtf ++;
		
		ulong i;
		for (i=lu; i<=ru;i++) {

			uint p= getPsiicsa(g->myicsa, i);   //node_rev
			uint q= getPsiicsa(g->myicsa, p);   //ti
			if (q <=rti) {
				uint x= getPsiicsa(g->myicsa, q);  //tf
				if (x >rtf) {

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n EDGE found for t= [%u,%u): nodo =%u , reverso= %u, t_i= %u, tf=%u ", tstart,tend, U,V, TI,TF);
					fflush(stdout);fflush(stderr);
				//@@ Just for debugging
				fprintf(stderr,"\n");				
				#endif
				
				return 1;
				}				
			}
		}
	}
	
	return 0;
}


// returns true if edge <u,v> existed during period [t,t') 
//         false otherwise
// That is, true if it exists contact <u,v,t1,t2> s.t. [t1,t2) intersect [t,t') != emptyset
int get_edge_weak(void *gindex, uint node, uint noderev, uint tstart, uint tend) {
//        return get_edge_interval(gindex, u, v, tstart, tend, 0);
#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_edge_interval_weak node=%u, node_rev %u ts =%u,te=%u\n",
	                                                       cont, node,noderev,tstart,tend);fflush (stdout);fflush(stderr);
#endif
	
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];

	uint u  = mapID(g,node,NODE);
	uint v  = mapID(g,noderev,NODE_REV);
	uint ti1 = mapID(g,tstart,TIME_INI);  // uint x=unmapID(g,ti1,TIME_INI);
	uint ti2 = mapID(g,tend,TIME_INI);  // uint x=unmapID(g,ti2,TIME_INI);
	uint tf = mapID(g,tstart,TIME_END);  // uint y=unmapID(g,tf,TIME_END);	
	
	pattern[0] = u; pattern[1] = v;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 2, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti1;
		ulong lti1,rti1;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti1, &rti1);
		//lti1
		//if (!numocc) rti1 --; 

		numocc=0;
		pattern[0] = ti2;   // time < ti2  (strict range)
		ulong lti2,rti2;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti2, &rti2);
		//lti2
		//if (!numocc) lti2 ++; 

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc) rtf ++; //
		
		
		ulong i;
		for (i=lu; i<=ru;i++) {

			uint p= getPsiicsa(g->myicsa, i);   //node_rev
			uint q= getPsiicsa(g->myicsa, p);   //ti
			
			/*
			if (    ((node == 1591) && (noderev==331)) 
			     || ((node==6074) && (noderev == 6730))
			   ){
				uint x= getPsiicsa(g->myicsa, q);
						uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
						uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
						uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
						uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
						printf("\n EDGE-WEAK? nodo %u , reverso= %lu=%u, t_i= %u, tf=%u \n", U, i,V, TI,TF);
						return 1;
			}
			*/		
				
			if (q < lti1) {
			//if (q <= rti1) {
				uint x= getPsiicsa(g->myicsa, q);  //tf
				if (x >rtf) {

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n EDGE found for t= [%u,%u): nodo =%u , reverso= %u, t_i= %u, tf=%u ", tstart,tend, U,V, TI,TF);
					fflush(stdout);fflush(stderr);
				//@@ Just for debugging
				fprintf(stderr,"\n");				
				#endif
				return 1;
				}				
			}
			else {
				if (q<lti2) {

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint x= getPsiicsa(g->myicsa, q);  //tf
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n EDGE found for t= [%u,%u): nodo =%u , reverso= %u, t_i= %u, tf=%u ", tstart,tend, U,V, TI,TF);
					fflush(stdout);fflush(stderr);
				//@@ Just for debugging
				fprintf(stderr,"\n");				
				#endif
				return 1;
				}
			}
		}
	}
	
	return 0;
}

int get_edge_interval(void *gindex, uint u, uint v, uint tstart, uint tend, uint semantic) { 	
	if (!semantic) 
		return get_edge_weak(gindex, u, v, tstart, tend);
	else
		return get_edge_strong(gindex, u, v, tstart, tend);
}


int get_edge_next(void *gindex, uint node, uint noderev, uint T) { 
	
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to get_edge_next node=%u, node_rev %u time =%u\n",
	                                                       cont, node,noderev,T);fflush (stdout);fflush(stderr);
	#endif
	
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint INFTIMEPOS = g->ssize;

	uint u  = mapID(g,node,NODE);
	uint v  = mapID(g,noderev,NODE_REV);
	uint ti = mapID(g,T,TIME_INI);  // uint x=unmapID(g,ti,TIME_INI);
	uint tf = mapID(g,T,TIME_END);  // uint y=unmapID(g,tf,TIME_END);
	
	pattern[0] = u; pattern[1] = v;
	ulong numocc,lu,ru;
	countIntIndex(g->myicsa, pattern , 2, &numocc, &lu, &ru);
	
	if (numocc) {
		numocc=0;
		pattern[0] = ti;
		ulong lti,rti;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
		//if (!numocc) rti --;

		numocc=0;
		pattern[0] = tf;   // time < tf  (strict range)
		ulong ltf,rtf;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
		if (!numocc) rtf ++;
		
		ulong i;
		ulong next_min = INFTIMEPOS;
		for (i=lu; i<=ru;i++) {

			uint p= getPsiicsa(g->myicsa, i);   //node_rev
			uint q= getPsiicsa(g->myicsa, p);   //ti
			if (q <=rti) {
				uint x= getPsiicsa(g->myicsa, q);  //tf
				if (x >rtf) {

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n EDGE_NEXT_TI** = %u, for t= %u: nodo =%u , reverso= %u, t_i= %u, tf=%u ", T, T, U,V, TI,TF);
					fflush(stdout);fflush(stderr);
				//@@ Just for debugging
				fprintf(stderr,"\n");
				#endif
				
				return T;
				}				
			}
			else {
				if (q < next_min) next_min = q;
			}
		}
		
		if (next_min != INFTIMEPOS)  {  //existing conctact (u,v,tx,-) s.t. tx > t, and tx=min of possible tx
			uint tinext= getRankicsa(g->myicsa, next_min) -1; tinext = unmapID(g,tinext,TIME_INI);

				//@@ Just for debugging
				#ifdef DEBUG_FACADE_ON
				{	pattern[0]=u;
					pattern[1]=v;
					pattern[2]=ti;
					countIntIndex(g->myicsa, pattern , 3, &numocc, &lu, &ru);
					ulong i;
					i=lu;
					uint p= getPsiicsa(g->myicsa, i);   //node_rev
					uint q= getPsiicsa(g->myicsa, p);   //ti
					uint x= getPsiicsa(g->myicsa, q);  //tf
					
					uint U = getRankicsa(g->myicsa, i) -1; U  = unmapID(g,U,NODE);
					uint V = getRankicsa(g->myicsa, p) -1; V  = unmapID(g,V,NODE_REV);
					uint TI= getRankicsa(g->myicsa, q) -1; TI = unmapID(g,TI,TIME_INI);
					uint TF= getRankicsa(g->myicsa, x) -1; TF = unmapID(g,TF,TIME_END);
					printf("\n EDGE_NEXT_TI**** = %u, for t= %u: nodo =%u , reverso= %u, t_i= %u, tf=%u ", tinext, T, U,V, TI,TF);
					fflush(stdout);fflush(stderr);
				//@@ Just for debugging
				fprintf(stderr,"\n");
				}
				#endif
			
			return tinext;
		}
		
	}
	
	#ifdef DEBUG_FACADE_ON		
	printf("\n EDGE_NEXT_TI*@#* = -1, for t= %u: nodo =%u , reverso= %u",  T, node, noderev);
	fflush(stdout);fflush(stderr);
	//exit(0);
	#endif

	return -1;
}

//counts the number of contacts in a given time t
// |(u,v,t1,t2)| s.t. t1<=t<t2 
uint get_snapshot_slow(void *gindex, uint t) { 	

	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to getsnapshot_slow  time =%u\n",cont, t);fflush (stdout);
	#endif
	
	//#define BUFFER 1024*1024*10
	uint *buffer1 = (uint *) my_malloc (1024*1024*20 *sizeof(uint));
	uint count = 0;
	
	twcsa *g = (twcsa *)gindex;

	uint step=g->nodes/100;
	ulong i;
	for (i=0; i<g->nodes; i++) {	
		get_neighbors_point(buffer1, gindex, i, t);
		count += buffer1[0];
		if(i%step==0) fprintf(stderr, "\tProcessing %.1f%% of %u nodes\r", (float)i/g->nodes*100, g->nodes);
	}	
	my_free_array(buffer1);
	return count; 	
}




//counts the number of contacts in a given time t
// |(u,v,t1,t2)| s.t. t1<=t<t2 
uint get_snapshot(void *gindex, uint time) {
	#ifdef DEBUG_FACADE_ON
	static int cont=0;
	cont ++;
	printf("\n\n @@@_%d_@@@@@@@@@@@@@\n\ncall to getsnapshot  time =%u\n",cont, time);fflush (stdout);
	#endif
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	
	uint count = 0;

	uint ti = mapID(g,time,TIME_INI);  // uint y=unmapID(g,tf,TIME_END);
	uint tf = mapID(g,time,TIME_END);  // uint y=unmapID(g,tf,TIME_END);

	ulong numocc=0;
	pattern[0] = ti;  // time <= ti (strict range)
	ulong lti,rti;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &lti, &rti);
	//if (!numocc) rti --;	

	numocc=0;
	pattern[0] = tf;   // time > tf  (opened range)
	ulong ltf,rtf;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &ltf, &rtf);
	if (!numocc)  rtf ++;
		
	ulong i = 2* g->n; //initial position of the t_i values on the suffix array.
	for ( ; i<=rti ; i++) {
		
		uint q= getPsiicsa(g->myicsa, i);
		
		if (q > rtf)
			count++;
	}
	
	return count;
}


/**************************/
//** debugging only

uint get_num_nodes(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->nodes;
}

/*  ------------*/
/******** TEMPORAL GRAPHS QUERIES FROM DIEGO-CARO ***********************/






















	/** ------------------------------------------------------------------
	 * recovers the source text by calling display(0,fileSize);
	 * ------------------------------------------------------------------ */


//-----------------------------------------------------------------------------------//
// needed for qsort of entries un dumpSourceDataSorted function
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
	


void dumpSourceData_slow(twcsa *wcsa, uint **data, size_t *len) {
	uint * buffer;
	dumpICSASequence(wcsa->myicsa, &buffer, len);

	size_t i,j,z=0;
	for (i=0; i<wcsa->n; i++){
		for (j=0;j<wcsa->nEntries;j++) {
			buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			z++;
		}
	}		
	*data = buffer;
	*len = *len -1;
}

	//recovers the source data
void dumpSourceData(twcsa *wcsa, uint **data, size_t *len) {	
	
	uint nEntries = wcsa->nEntries;
	size_t n = wcsa->n;
	size_t ssize = wcsa->ssize;
	
	uint * buffer = (uint *) my_malloc (sizeof(uint) * ssize);
	
	
	size_t i,j;
	size_t z=0;
	uint p;
	for (i=0;i<n;i++) {
		p=i;
		for (j=0;j<nEntries;j++) {
			uint value = getRankicsa(wcsa->myicsa,p);
			buffer[z] = value -1;
			buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			z++;
			p=getPsiicsa(wcsa->myicsa,p);
		}
	}
	
	*data = buffer;
	*len = z;
	return ;	
}

		//recovers the source data
void dumpSourceDataSorted(twcsa *wcsa, uint **data, size_t *len) {	
	
	uint nEntries = wcsa->nEntries;
	size_t n = wcsa->n;
	size_t ssize = wcsa->ssize;
	
	uint * buffer = (uint *) my_malloc (sizeof(uint) * ssize);
	
	
	size_t i,j;
	size_t z=0;
	uint p;
	for (i=0;i<n;i++) {
		p=i;
		for (j=0;j<nEntries;j++) {
			uint value = getRankicsa(wcsa->myicsa,p);
			buffer[z] = value -1;
			buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			z++;
			p=getPsiicsa(wcsa->myicsa,p);
		}
	}
	
	nentries_buildFacade = wcsa->nEntries;  //needed for qsort of entries
	sortRecords_buildFacade(wcsa, buffer);  //so that the records are returned "sorted"
	
	*data = buffer;
	*len = z;
	return ;	
}





double getTime2 (void)
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




	
