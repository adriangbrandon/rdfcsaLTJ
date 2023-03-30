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

#ifndef NO_TYPE_SET_OPS
		#define NO_TYPE_SET_OPS (99)
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
		
		//used in dual_rdf index.
		uint gapobjects;        //given an ID of object in the source data, we substract "gapsobjects" to 
                                 //map into an ID in the range [1..no]
                                 //or, the 1st object of the source data has the ID gapsopbjects+1.
                                 //we need it only for "LEAP()" function.
		
		uint is_in_spo_order;     //1 for RDFCSA IN SPO ORDER  -default
		                          //O for RDFCSA IN OPS ORDER  -BEWARE !! set externally after build and load. NOT SAVED TO DISK !!
		                          //                                      i.e. within build_index_dual() and load_index_dual(); 	                          
		                          
	} twcsa;


#endif




/** ******************************************************************************
    * Interface dual RDFCSA spo & ops.
*********************************************************************************/
typedef struct {
	twcsa *spo;
	twcsa *ops;
	//ulong RANGEVALID[3][2];  
	ulong **RANGEVALID;  
							 //valid ranges for input SUBJECTS [1,ns], PREDICATES [1,np], AND OBJECTS [1+gapobjects, gapopbjects +no]
	                         //initilized in dual_build() and dual_load()
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
// int printInfo_dual(void *index);
// 
// int testRecoverAndCompareSPO_OPS_dump(void *index);   //
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

int save_index (void *index, const char *filename);

        /*  Loads index from one or more file(s) named filename, possibly 
          adding the proper extensions. */

int load_index (const char *filename, void **index);

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
    void loadStructs(twcsa *wcsa, const char *basename);	
	twcsa *loadWCSA(const char *filename);	

// Definitions of PRIVATE functions
	//Auxiliary functions
	int saveSEfile (const char *basename, uint *v, ulong n);
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



// dual @ 2023. 

// for a given position "pos" within 0..3n-1, returns its type of component 
// allows g to be either a twcsa in spo- or ops-order
uint dual_typeFromPos(twcsa *g, size_t pos);

// for a given position "pos" within 0..3n-1, returns its actual value 
// allows g to be either a twcsa in spo- or ops-order
uint dual_valueFromPos(twcsa *g, size_t pos);

// prints and returns a triple, whose components include position pos (0 <= pos <3n)
// allows g to be either a twcsa in spo- or ops-order
uint *dual_printTriple(twcsa *g, size_t pos);



//**************** FOR LEAP AND DOWN ****************************************************
//Computes the 1st position i in [left,right] such that \Psi[i] \in in [tl,tr].
//Returns: 
//   0 if no i \in [left,right] maps into [tl,tr]
//   Otherwise: 
//		- Computes x=Psi[i]. 
//		- Updates *left =i;
//		- Returns dual_valueFromPos(g,x);
//

//NOTE :: left, right, tl and tr are defined as uint32 values, but could have been ulong

uint dual_searchPsiTarget_to_leap(twcsa *g, ulong *left, ulong right, ulong tl, ulong tr);
uint dual_searchPsiPsiTarget_to_leap(twcsa *g, ulong *left, ulong right, ulong tl, ulong tr) ;

//Computes the positions i in [left,right] such that forall i, \Psi[i] \in in [tl,tr].
//		- Updates *left  and *right is updated
//		- Then updates *left = getPsiValue( *left), and *right = getPsiValue (*right).
int dual_searchPsiTarget_to_down(twcsa *g, ulong *left, ulong *right, ulong tl, ulong tr);

// returns [0,n-1] || [n,2n-1] || [2n, 3n-1], that corresponds to "type"
// depending on the spo|ops order of g.
//int dual_getRangeLR_uint32_for_type(twcsa *g, uint type, uint *left, uint *right);
int dual_getRangeLR_for_type(twcsa *g, uint type, ulong *left, ulong *right);

// returns the range [lp,rp] that corresponds to a value of type "type"
// depending on the spo|ops order of g.
//int dual_getRangeLR_uint32_for_type_and_value(twcsa *g, uint type, uint value,  uint *left, uint *right);
int dual_getRangeLR_for_type_and_value(twcsa *g, uint type, uint value, ulong *left, ulong *right);


// returns the range [lp,rp] that corresponds for values x of type "type", such that x>=value 
// depending on the spo|ops order of g.
int dual_getRangeLR_for_type_and_GEQvalue(twcsa *g, uint type, uint value, ulong *left, ulong *right);


// checks if a given value of a given type is a valid value in the source alphabets
// eg p in [1,npi] (npi=number of predicates in source dataset => npi = np -1;
// eg s in [1,nsi] (nsi=number of subjects   in source dataset => nsi = ns -1;
// eg o in [gapobjects,gapobjects+noi-1] 
//                 (noi=number of objects    in source dataset => noi = no -1;
int dual_isValidValue_for_Type_in_Input(twcsa *g, int type, uint value);
inline int dual_isValidValue_for_Type_in_Input_Matrix(ulong** V, int type, uint value);
int dual_initRangeValid_for_Type_in_Input(tdualrdfcsa *dual);





