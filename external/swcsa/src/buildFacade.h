/* buildFacade.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * BuildFacade.h: Definition of the functions that the word-based self-index
 *   must implement following the "../pizza/interface.h" guidelines. We also 
 *   define here the data types required by the WCSA self-index, as well as the
 *   twcsa type (that will be referred to as an opaque type by "../pizza" programs.
 *
 * See more details in:
 * Antonio Fariña, Nieves Brisaboa, Gonzalo Navarro, Francisco Claude, Ángeles Places, 
 * and Eduardo Rodríguez. Word-based Self-Indexes for Natural Language Text. ACM 
 * Transactions on Information Systems (TOIS), 2012. 
 * http://vios.dc.fi.udc.es/indexing/wsi/publications.html
 * http://www.dcc.uchile.cl/~gnavarro/ps/tois11.pdf	  

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



/* only for getTime() */
#include <sys/time.h>
#include <sys/resource.h>
#include <assert.h>


#include "utils/defValues.h"
#include "utils/fileInfo.h"


#include "utils/huff.h"
#include "utils/parameters.h"

#include "utils/bitmap.h"


#include "intIndex_qsort/interfaceIntIndex.h"
#include "intIndexUtils/psiHuffmanRLE.h"

#include "libcds/libcdsBasics.h"
#include "libcds/BitSequence.h"
#include "libcds/BitSequenceBuilder.h"



#include "interface.h"
#include "interfaceDual.h"
#include "graphReader.h"



using namespace std;
using namespace cds_static;

/*
#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint  unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif
*/

#ifndef my_malloc
	#define my_malloc(u) malloc(u)
	//#define my_malloc(u) new uchar[u]
#endif

#ifndef my_free
	#define my_free(u) free(u)
#endif

#ifndef my_free_array
	#define my_free_array(u) free(u)
#endif


//Permits to show the results of the queries.
//#define DEBUG_FACADE_ON 

//Allows using getPsiBuffericsa to sequentially uncompress consecutive 
//psi values instead of obtaining (not sequentially) 1 value at a time

#define BUFFER_PSI_ON 


//TYPE for use in mapID(id,type) and unmapID(id,type)
#ifndef SUBJECT 
		#define SUBJECT (0) 
#endif
#ifndef PREDICATE
		#define PREDICATE (1)
#endif
#ifndef OBJECT 
		#define OBJECT (2)
#endif


#ifndef BUILD_FACADE_TYPES
#define BUILD_FACADE_TYPES


#define MAX_ENTRIES 10


	/**the WCSA index structures... */
	typedef struct {
		uint nEntries;  // 3=RDF
		int *gaps;     //value to add to any entry in a RDF triplet -> gaps = [0, ns, ns+np, ns+np+no] by default in S->p->O order
		
		uint ns;        //number of subjects from [0 ... ns -1]
		uint np;        //number of predicates from [0 ... np -1]
		uint no;        //number of objects from [0 ... no -1]
		uint nso;       //number of subject-object ids (subjects that become objects in some cases)

		size_t n;       // number of contacts, or RDF triplets + 1 dummy node, we are going to process
		uint *s;        // S[0 .. nEntries*n) (including dummy)
		size_t ssize;   //number of integers in S

		void *myicsa;   //the CSA built on S				
	} twcsa;


#endif




/** ******************************************************************************
    * Interface dual RDFCSA spo & ops.
*********************************************************************************/
typedef struct {
	twcsa *spo;
	twcsa *ops;
} tdualrdfcsa;

#define DUAL_FILE_EXT "dualOPS"

#include "interfaceDual.h"

// /* creates first the spo permutation and the reutilizes s to create the ops permutation index */
// int build_index_dual (struct graphDB *graph, char *build_options, void **index) ;
// 
// int save_index_dual (void *index, char *filename);
// int load_index_dual (char *filename, void **index);
// int free_index_dual (void *index);
// int index_size_dual(void *index, ulong *size);
// int get_length_dual(void *index, ulong *length);
// 
// int testRecoverAndCompareSPO_OPS(void *index);   //
// 
// 
// //titerator * createIterator(void *index);
// void * createIterator_dual(void *index);
// int init_so_dual(void *iterator, uint s, uint o);
// int init_sp_dual(void *iterator, uint s, uint o);
// //...
// int leap_dual(void *iterator, int type, uint value);
// int down_dual(void *iterator, int type, uint value);
// int up_dual(void *iterator);
// 
// // para un iterador, devolver o tamaño mínimo dos rangos que ten restrinxidos. --- ou algo así ... a definir con Adrián.
// // se están os 2 activos --> o menor
// // se só hai un activo --> ese.
// // ** sácase a partir dos rangos e facendo ifs ... ** isto está sen definir aínda.
// uint get_range_length_dual (void *iterator);


/** ******************************************************************************
    * Interface (from pizza chili) for using the WCSA index
*********************************************************************************/

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
int build_index (char *filename, char *build_options, void **index) ;

	//private function
		int build_index_from_graph (struct graphDB *graph, char *build_options, void **index);

        /*  Saves index on disk by using single or multiple files, having 
          proper extensions. */

int save_index (void *index, char *filename);

        /*  Loads index from one or more file(s) named filename, possibly 
          adding the proper extensions. */

int load_index (char *filename, void **index);

        /* Frees the memory occupied by index. */

int free_index (void *index);

        /* Gives the memory occupied by index in bytes. */


int index_size(void *index, ulong *size);


        /* Gives the length of the text indexed */

int get_length(void *index, ulong *length);

        /*  Obtains the length of the text indexed by index. */
int length (void *index, ulong *length);


/*size_t start(BitSequence *b, uint i) ;
inline uint belong(BitSequence *b, size_t i);
*/



/** *******************************************************************************************/
/** Building part of the index ****************************************************************/
/** *******************************************************************************************/

int build_WCSA (struct graphDB *graph,char *build_options, void **index);
int build_iCSA (char  *build_options, void *index); 



/** *******************************************************************************
  * Showing some statistics and info of the index
  * *******************************************************************************/

		/* Shows summary info of the index */
int printInfo(void *index);


// Definitions of some PUBLIC function prototipes.

		//loading/freeing the data structures into memory.	
    void loadStructs(twcsa *wcsa, char *basename);	
	twcsa *loadWCSA(char *filename);	

// Definitions of PRIVATE functions
	//Auxiliary functions
	int saveSEfile (char *basename, uint *v, ulong n);
	double getTime2 (void); 



/** *******************************************************************************************/
/** ********* RECOVERING THE SOURCE GRAPH DATABASE ********************************************/
/** *******************************************************************************************/
	//recovers the source data
void dumpSourceData(twcsa *wcsa, uint **data, size_t *len);
void dumpSourceDataOPS_to_SPO(twcsa *wcsa, uint **data, size_t *len);
/*
void dumpSourceData_slow(twcsa *wcsa, uint **data, size_t *len);

	//recovers the source data sorted 
void dumpSourceDataSorted(twcsa *wcsa, uint **data, size_t *len);
*/

/** *******************************************************************************************/
/** Search part of the index ******************************************************************/
/** *******************************************************************************************/

/******** RDF GRAPHS ****************************************************/

int spo(void *gindex, int s, int p, int o, uint **res);
int sp (void *gindex, int s, int p, uint **res);
int po (void *gindex, int p, int o, uint **res);
int so (void *gindex, int s, int o, uint **res);
int s  (void *gindex, int s, uint **res);
int p  (void *gindex, int p, uint **res);
int o  (void *gindex, int o, uint **res);

/**************************/
//** debugging only
int test_spo(void *gindex, uint **res);
int test_sp (void *gindex, uint **res, uint **res2);
int test_po (void *gindex, uint **res, uint **res2);


uint get_num_subjects(void *gindex);
uint get_num_predicates(void *gindex);
uint get_num_objects(void *gindex);
uint get_num_subjects_objects(void *gindex);
uint get_num_nodes(void *gindex);

uint mapID (twcsa *g, uint value, uint type);
uint unmapID (twcsa *g, uint value, uint type);



