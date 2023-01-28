#ifndef _INTERFACE_GRAPH
#define _INTERFACE_GRAPH


/* interface.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * interface.h: Interface including all the functions temporal graph needs.
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
 
/* General interface for using the compressed temporal graph */

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif

#ifndef SUBJECT                    //used in BuildFacade.h --> for mapID and unmapID functions
		#define SUBJECT (0) 
#endif
#ifndef PREDICATE
		#define PREDICATE (1)
#endif
#ifndef OBJECT 
		#define OBJECT (2)
#endif


 struct graphDB {
	uint nEntries ;  // 3=RDF
				     // (number of entries per record/contact...)	
	int *gaps;      // value to add to any entry in a RDF triple
		             // gaps=[0, ns, ns+np] for RDF-Triples
	
	//uint nodes;      //number of nodes from [0 ... nodes -1]
	//uint maxtime;    //maximum time in the dataset from [0 ... maxtime-1]
	//uint edges;      //unused.

	uint ns,np,no,nso;
	uint gapobjects; //for adrian's type format;  0 for HDT format
	
	size_t n;    // number of triples in the input, that will be processed (+1 when dummy node is added)
	uint *s;     // (S[0 .. nEntries*n)
};


// queries in the original k*-trees were using a fixed buffer for the results.
// we will do the same here for a fair direct comparison.
#define MAX_RESULTS (1024*1024*170)  // not more than 170xtimes... otherwise linking error.



/* Error management */

        /* Returns a string describing the error associated with error number
          e. The string must not be freed, and it will be overwritten with
          subsequent calls. */

char *error_index (int e);

/* Building the index */

        /* Creates index from text[0..length-1]. Note that the index is an 
          opaque data type. Any build option must be passed in string 
          build_options, whose syntax depends on the index. The index must 
          always work with some default parameters if build_options is NULL. 
          The returned index is ready to be queried. */

//int build_index (struct graphDB *graph, char *build_options, void **index) ;
int build_index (const char *filename, char *build_options, void **index) ;

        /*  Saves index on disk by using single or multiple files, having 
          proper extensions. */
int save_index (void *index, const char *filename);
	//@@void tgs_save(struct tgs *a, ofstream & f);

        /*  Loads index from one or more file(s) named filename, possibly 
          adding the proper extensions. */
int load_index (const char *filename, void **index);
	//@@void tgs_load(struct tgs *a, ifstream & f);

        /* Frees the memory occupied by index. */
int free_index (void *index);
	//@@void tgs_free(struct tgs *a);

        /* Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size);
	//@@size_t tgs_size(struct tgs *a);


        /* Gives the length of the text indexed */

int get_length(void *index, ulong *length);

        /*  Obtains the length of the text indexed by index. */
int length (void *index, ulong *length);


/******** RDF GRAPHS ****************************************************/

int spo(void *gindex, int s, int p, int o, uint **res);
	int spo_0(void *gindex, int s, int p, int o, uint **res);
	int spo_1(void *gindex, int s, int p, int o, uint **res);
	int spo_2(void *gindex, int s, int p, int o, uint **res);
	int spo_9(void *gindex, int s, int p, int o, uint **res);

int sp (void *gindex, int s, int p, uint **res);
	int sp_0 (void *gindex, int s, int p, uint **res);
	int sp_1 (void *gindex, int s, int p, uint **res);
	int sp_2 (void *gindex, int s, int p, uint **res);
	int sp_9 (void *gindex, int s, int p, uint **res);

int po (void *gindex, int p, int o, uint **res);
	int po_0 (void *gindex, int p, int o, uint **res);
	int po_1 (void *gindex, int p, int o, uint **res);
	int po_2 (void *gindex, int p, int o, uint **res);
	int po_9 (void *gindex, int p, int o, uint **res);

int so (void *gindex, int s, int o, uint **res);
	int so_0 (void *gindex, int s, int o, uint **res);
	int so_1 (void *gindex, int s, int o, uint **res);
	int so_2 (void *gindex, int s, int o, uint **res);
	int so_9 (void *gindex, int s, int o, uint **res);

int s  (void *gindex, int s, uint **res);
	int s_0  (void *gindex, int s, uint **res);
	int s_9  (void *gindex, int s, uint **res);

int p  (void *gindex, int p, uint **res);
	int p_0  (void *gindex, int p, uint **res);
	int p_9  (void *gindex, int p, uint **res);
	
int o  (void *gindex, int o, uint **res);
	int o_0  (void *gindex, int o, uint **res);
	int o_9  (void *gindex, int o, uint **res);

long vvv  (void *gindex,  uint **res);  //2019-fari





/******** RDF GRAPHS ****************************************************/

//** debugging only
int test_spo(void *gindex, uint **res);
int test_sp (void *gindex, uint **res,  uint **res2);
int test_po (void *gindex, uint **res,  uint **res2);
int test_so (void *gindex, uint **res,  uint **res2);
int test_s  (void *gindex, uint **res,  uint **res2);
int test_p  (void *gindex, uint **res,  uint **res2);
int test_o  (void *gindex, uint **res,  uint **res2);

uint get_num_nodes(void *gindex) ;

#endif
