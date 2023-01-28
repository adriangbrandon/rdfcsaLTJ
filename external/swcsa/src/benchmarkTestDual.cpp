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
#include "interfaceDual.h"

#include "buildFacade.h"   // include only needed to have access to tdualrdfcsa here
                             

#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

#include "debug.h"
#include "arraysort.c"  //@@


// FROM ORIGINAL RDFCSA.
#define Q_SPO 1
#define Q_SP  2
#define Q_PO  3
#define Q_SO  4
#define Q_S   5
#define Q_P   6
#define Q_O   7

#define Q_DUAL_SPO_OPS 11
#define Q_DUAL_SO_OS   12
#define Q_DUAL_SP_PS   13
#define Q_DUAL_PO_OP   14
#define Q_DUAL_S_S     15
#define Q_DUAL_P_P     16
#define Q_DUAL_O_O     17

#define Q_INIT_TEST_DUAL_SPO_OPS 21


#define Q_INIT_DUAL_SPO 31
#define Q_INIT_DUAL_SO  32
#define Q_INIT_DUAL_SP  33
#define Q_INIT_DUAL_PO  34
#define Q_INIT_DUAL_S   35
#define Q_INIT_DUAL_P   36
#define Q_INIT_DUAL_O   37


#define Q_TEST_LEAP_VALID_VAL 40
#define Q_TEST_LEAP_DOWN_UP_1 41


/* defined below: We create iterators and play with them.*/
void testinitSPO(void * index);

void testinitSO (void * index);
void testinitSP (void * index);
void testinitPO (void * index);

void testinitS (void * index);
void testinitP (void * index);
void testinitO (void * index);

void testLeapValidValue (void * index) ;
void testLeapDownUp1 (void * index) ;




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
	int error= load_index_dual (filename, &index);
	IFERROR (error);
	error = index_size_dual(index, &Index_size);
	IFERROR (error);
	error = get_length_dual(index, &Text_length);
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






	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	my_free_array (dualrdf->spo->s);  dualrdf->spo->s = NULL;
	my_free_array (dualrdf->ops->s);  dualrdf->ops->s = NULL;
					
		
	startClockTime();
	long results;

			switch(queryType) {
				case Q_SPO: {  results =test_spo(dualrdf->spo, &res); break;}
				case Q_SP : {  results =test_sp (dualrdf->spo, &res, &res2); break;}
				case Q_PO : {  results =test_po (dualrdf->spo, &res, &res2); break;}
				case Q_SO : {  results =test_so (dualrdf->spo, &res, &res2); break;}
				case Q_S  : {  results =test_s  (dualrdf->spo, &res, &res2); break;}
				case Q_P  : {  results =test_p  (dualrdf->spo, &res, &res2); break;}
				case Q_O  : {  results =test_o  (dualrdf->spo, &res, &res2); break;}

				case Q_DUAL_SPO_OPS  : {  results =dual_test_spo_ops  (dualrdf, &res); break;}
				case Q_DUAL_SO_OS    : {  results =dual_test_so_os    (dualrdf, &res, &res2); break;}
				case Q_DUAL_SP_PS    : {  results =dual_test_sp_ps    (dualrdf, &res, &res2); break;}
				case Q_DUAL_PO_OP    : {  results =dual_test_po_op    (dualrdf, &res, &res2); break;}
				case Q_DUAL_S_S      : {  results =dual_test_s_s      (dualrdf, &res, &res2); break;}
				case Q_DUAL_P_P      : {  results =dual_test_p_p      (dualrdf, &res, &res2); break;}
				case Q_DUAL_O_O      : {  results =dual_test_o_o      (dualrdf, &res, &res2); break;}


				//NOt implemented
				case Q_INIT_TEST_DUAL_SPO_OPS   : {  results =dual_test_init_spo_ops  (dualrdf); break;}
				

				case Q_INIT_DUAL_SPO  			: {  testinitSPO (dualrdf); break;}
				case Q_INIT_DUAL_SO   			: {  testinitSO  (dualrdf); break;}
				case Q_INIT_DUAL_SP   			: {  testinitSP  (dualrdf); break;}
				case Q_INIT_DUAL_PO   			: {  testinitPO  (dualrdf); break;}
				case Q_INIT_DUAL_S    			: {  testinitS   (dualrdf); break;}
				case Q_INIT_DUAL_P    			: {  testinitP   (dualrdf); break;}
				case Q_INIT_DUAL_O    			: {  testinitO   (dualrdf); break;}

				case Q_TEST_LEAP_VALID_VAL		: {  testLeapValidValue (dualrdf); break;}
				case Q_TEST_LEAP_DOWN_UP_1		: {  testLeapDownUp1 (dualrdf); break;}

				
			}
    	
        
        ulong microsecs = endClockTime()/1000; //to microsecs

        
		printf("\n <index>\t<microsec>");
        printf("\n %s\t%ld\\n", argv[1], microsecs);

					
		free_index_dual(index);
        free(res);
        free(res2);

        exit(0);
}





void testinitSPO(void * index) {
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_spo_dual(it,6991606, 758, 24226252);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_spo_dual(it2,15962932, 54, 27431207);
	printIterator_dual(it2);
	
	free(it);
	free(it2);	
}




void testinitSO(void * index) {
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_so_dual(it,6991606,  24226252);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_so_dual(it2,15962932, 27431207);
	printIterator_dual(it2);
	
	free(it);
	free(it2);	
}


void testinitSP (void * index){
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_sp_dual(it,6991606, 758);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_sp_dual(it2, 15962932, 54);
	printIterator_dual(it2);
	

	init_sp_dual(it2, 16949722 ,726);
	printIterator_dual(it2);

	
	free(it);
	free(it2);		
}

void testinitPO (void * index){
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_po_dual(it,758, 24226252);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_po_dual(it2, 54, 27431207);
	printIterator_dual(it2);

//** borra 2 liñas seguintes
//	init_po_dual(it2, 0,14357810);
//	printIterator_dual(it2);
//
//	t_iterator *iter = (t_iterator*) it;
//	tdualrdfcsa *dualrdf = (tdualrdfcsa *) iter->dualrdfcsa;
//	printf("\n UNMAP --> %u", mapID(dualrdf->spo,1+dualrdf->spo->gapobjects,OBJECT));
//	
//*
	free(it);
	free(it2);		
}

void testinitS (void * index) {
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_s_dual(it,6991606);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_s_dual(it2, 15962932);
	printIterator_dual(it2);


	
	free(it);
	free(it2);		
}

void testinitP (void * index) {
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_p_dual(it,758);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_p_dual(it2, 54);
	printIterator_dual(it2);
	
	free(it);
	free(it2);		
} 

void testinitO (void * index) {	
	void *it = createIterator_dual(index);
	printIterator_dual(it);
	init_o_dual(it,24226252);
	printIterator_dual(it);
	
	printf("\n------\n");
	void *it2 = createIterator_dual(index);	
	printIterator_dual(it2);
	init_o_dual(it2, 27431207);
	printIterator_dual(it2);
	
	void *it3 = cloneIterador_dual(it);

	free(it);
	free(it2);		
	printf("\n--- cloned from 1st iterator---\n");
	printIterator_dual(it3);


//** borra 2 liñas seguintes
//** borra 2 liñas seguintes
//
//	t_iterator *iter = (t_iterator*) it;
//	tdualrdfcsa *dualrdf = (tdualrdfcsa *) iter->dualrdfcsa;
//	uint firstobject = 1+dualrdf->spo->gapobjects;
//	//init_po_dual(it3, 0,14357810);
//	init_po_dual(it3, 2093,firstobject);
//	printIterator_dual(it3);
//	
//*


//	init_o_dual(it2, 14357810);
//	printIterator_dual(it2);
	
	
	free(it3);

} 



 
void testLeapValidValue (void * index) {	
	printf("\n\t\t** ---------------------testLeapValidValue: on wiki1 -------------------");
	// LOAD : ns  = 19227373, np =2102, no =37641487, gapoffsets=14357810
	// INPUT: nsi = 19227372, npi=2101, noi=37641486, gapoffsets=14357810
	
	void *it = createIterator_dual(index);
	printIterator_dual(it);


	
	uint value;
	printf("\n test RANGES leap on PREDICATES");
		 value = leap_dual(it,PREDICATE,1);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,PREDICATE,2);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,PREDICATE,2101);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,PREDICATE,2102);  //should be 0
	printf("\n\t\t** value = %u",value);

	printf("\n");

//gapoffsets=14357810

	printf("\n test RANGES leap on SUBJECTS");
		 value = leap_dual(it,SUBJECT,1);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,SUBJECT,19227371);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,SUBJECT,19227372);   
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,SUBJECT,19227373);   //should be 0
	printf("\n\t\t** value = %u",value);
	printf("\n");
	
//gapoffsets=14357810

	printf("\n test RANGES leap on OBJECTS");
		 value = leap_dual(it,OBJECT,14357810);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,OBJECT,14357810+1);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,OBJECT,51999295);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,OBJECT,51999296);   
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,OBJECT,51999297);   //should be 0
	printf("\n\t\t** value = %u",value);
	printf("\n");


	
//	init_o_dual(it,24226252);
//	printIterator_dual(it);
}


void testLeapDownUp1 (void * index) {	
	printf("\n\t\t** ---------------------testLeapDownUp1: on wiki1 -------------------");
	// LOAD : ns  = 19227373, np =2102, no =37641487, gapoffsets=14357810
	// INPUT: nsi = 19227372, npi=2101, noi=37641486, gapoffsets=14357810
	
	void *it = createIterator_dual(index);
	uint value;
	
	printf("\n test RANGES leap on SUBJECTS");
	
	
	printf("\n\t\t** --------------");
		 value = leap_dual(it,SUBJECT,16949721);
	printf("\n\t\t** value = %u",value);
		 value = leap_dual(it,SUBJECT,16949722);
	printf("\n\t\t** value = %u",value);
	printf("\n");
	printIterator_dual(it);

	setColorRojo(); down_dual(it,SUBJECT, 16949722);  printf("\n\t\t down_dual(it,SUBJECT, 16949722)"); setColorNormal();
	printIterator_dual(it);

		printf("\n\t\t** --------------");
		printf("\n\t\t");

		setColorVerde(); setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,1) = %u" , leap_dual(it,PREDICATE,1)); setColorNormal();
	printIterator_dual(it);
		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,3) = %u" , leap_dual(it,PREDICATE,3)); setColorNormal();
	printIterator_dual(it);		
		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,4) = %u" , leap_dual(it,PREDICATE,4)); setColorNormal();
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,8) = %u" , leap_dual(it,PREDICATE,8)); setColorNormal();
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,13) = %u" , leap_dual(it,PREDICATE,13)); setColorNormal();
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,406) = %u" , leap_dual(it,PREDICATE,406)); setColorNormal();
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,420) = %u" , leap_dual(it,PREDICATE,420)); setColorNormal();
	printIterator_dual(it);

	printf("\n\n--");
	printf("\n\n--");
	
	setColorRojo(); down_dual(it,PREDICATE,420);  printf("\n\t\t down_dual(it,PREDICATE,420)"); setColorNormal();
	
	printIterator_dual(it);
		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16682550) = %u" , leap_dual(it,OBJECT,16682550));  setColorNormal(); //SI está
		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16983700) = %u" , leap_dual(it,OBJECT,16983700));  setColorNormal(); //SI está
		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16983800) = %u" , leap_dual(it,OBJECT,16983800));  setColorNormal(); //SI está
	printIterator_dual(it);
	printf("\n\n--");
	printf("\n\n--");
	
	
	setColorRojo();    up_dual(it);   	setColorNormal();
	
	printIterator_dual(it);

	printf("\n @@@@@@@@@@@@@@@@@@@");


		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,421) = %u" , leap_dual(it,PREDICATE,421)); setColorNormal();
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,421) = %u" , leap_dual(it,PREDICATE,421));  setColorNormal(); //no está
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,726) = %u" , leap_dual(it,PREDICATE,726));  setColorNormal(); //SI está Y ES EL ÚLTIMO (hai 3 tripletas con p=726
	printIterator_dual(it);

//		setColorVerde();printf("\n\t\t leap_dual(it,PREDICATE,727) = %u" , leap_dual(it,PREDICATE,727));  setColorNormal(); //no está y devuelve 0 (comprobado
//	printIterator_dual(it);


	printf("\n\n--");
	setColorRojo();  down_dual(it,PREDICATE,726);  printf("\n\t\t down_dual(it,PREDICATE,726)");  setColorNormal(); 
	printIterator_dual(it);

		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16870252) = %u" , leap_dual(it,OBJECT,16870252));  setColorNormal(); //SI está
		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16975765) = %u" , leap_dual(it,OBJECT,16975765));  setColorNormal(); //SI está
		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16994259) = %u" , leap_dual(it,OBJECT,16994259));  setColorNormal(); //SI está
		setColorVerde();printf("\n\t\t leap_dual(it,OBJECT,16994300) = %u" , leap_dual(it,OBJECT,16994300));  setColorNormal(); //SI está
		
	printIterator_dual(it);


		printf("\n");

	
	
}
