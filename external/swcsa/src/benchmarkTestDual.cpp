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
#define Q_DUAL_TEST_BIN_EXP 18

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
#define Q_TEST_LEAP_DOWN_UP_2 42
#define Q_TEST_LEAP_DOWN_UP_3 43


#define Q_TEST_GETALL_GETONE_PO 50
#define Q_TEST_GETALL_GETONE_SP 51



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
void testLeapDownUp2 (void * index) ;  //adrian fallo integración sábado 28/01
void testLeapDownUp3 (void * index) ;  //adrian fallo integración domingo 29/01

int dual_test_po_op_getall_getOne_next(void *index);
int dual_test_sp_ps_getall_getOne_next(void *index);


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

				case Q_DUAL_TEST_BIN_EXP : {  results =dual_test_sp_ps_TARGET_BIN_VS_FIRSTLEFT (dualrdf, &res, &res2); break;}

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
				case Q_TEST_LEAP_DOWN_UP_2		: {  testLeapDownUp2 (dualrdf); break;}
				case Q_TEST_LEAP_DOWN_UP_3		: {  testLeapDownUp3 (dualrdf); break;}

				case Q_TEST_GETALL_GETONE_PO		: {  dual_test_po_op_getall_getOne_next (dualrdf); break;}
				case Q_TEST_GETALL_GETONE_SP		: {  dual_test_sp_ps_getall_getOne_next (dualrdf); break;}

				
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
	

	init_p_dual(it2, 377);
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




void testLeapDownUp2 (void * index) {	
	printf("\n\t\t** ---------------------testLeapDownUp1: on wiki1 -------------------");
	// LOAD : ns  = 19227373, np =2102, no =37641487, gapoffsets=14357810
	// INPUT: nsi = 19227372, npi=2101, noi=37641486, gapoffsets=14357810
	
	void *it1 = createIterator_dual(index);
	void *it2 = createIterator_dual(index);
	uint value;

	setColorRojo(); printf("\n\t\t init_p_dual(it1,353)");  init_p_dual(it1,353); setColorNormal();
	printIterator_dual(it1);
	
	setColorRojo(); printf("\n\t\t init_p_dual(it2,280)");  init_p_dual(it2,280); setColorNormal();
	printIterator_dual(it2);


		setColorVerde();;printf("\n\t\t leap_dual(it1,SUBJECT,1) = %u" , leap_dual(it1,SUBJECT,1) ); setColorNormal();               //8319740 353
	printIterator_dual(it1);

		setColorVerde();;printf("\n\t\t leap_dual(it2,SUBJECT,8319740) = %u" , leap_dual(it2,SUBJECT, 8319740) ); setColorNormal();  //8319772 280
	printIterator_dual(it2);

		setColorVerde();;printf("\n\t\t leap_dual(it1,SUBJECT,8319772) = %u" , leap_dual(it1,SUBJECT,8319772) ); setColorNormal();   //8330616 353
	printIterator_dual(it1);

		setColorVerde();;printf("\n\t\t leap_dual(it2,SUBJECT,8330616) = %u" , leap_dual(it2,SUBJECT, 8330616) ); setColorNormal();  //8330624 280
	printIterator_dual(it2);

		setColorVerde();;printf("\n\t\t leap_dual(it1,SUBJECT,8330624) = %u" , leap_dual(it1,SUBJECT,8330624) ); setColorNormal();   //8331656 353
	printIterator_dual(it1);

		setColorVerde();;printf("\n\t\t leap_dual(it2,SUBJECT,8331656) = %u" , leap_dual(it2,SUBJECT, 8331656) ); setColorNormal();  //8333944 280
	printIterator_dual(it2);

		setColorVerde();;printf("\n\t\t leap_dual(it1,SUBJECT,8333944) = %u" , leap_dual(it1,SUBJECT,8333944) ); setColorNormal();   //8334516 353
	printIterator_dual(it1);

	uint value1 ,value2;
	value1=1;
	value2=1;
	
	 uint i=0;
	 while (value1 !=0 && value2 !=0) {
			value1 = leap_dual(it1,SUBJECT,value2);			
			value2 = leap_dual(it2,SUBJECT,value1);
			if (value1== value2) {
				printf("\n\t match #%u found triples  < %u 353 ?o > < %u 280 ?o >", i, value1,value2);
				i++;
				// printf("\n ** ITERADOR 1 **");
				// printIterator_dual(it1);
				// printf("\n ** ITERADOR 2 **");
				// printIterator_dual(it2);
				// break;
				value2 = leap_dual(it2,SUBJECT,value1+1);
			}
	 }
	
	printf("\n\n");

	
	
}





void testLeapDownUp3 (void * index) {	
	printf("\n\t\t** ---------------------testLeapDownUp1: on wiki1 -------------------");
	// LOAD : ns  = 19227373, np =2102, no =37641487, gapoffsets=14357810
	// INPUT: nsi = 19227372, npi=2101, noi=37641486, gapoffsets=14357810
	
	void *it1 = createIterator_dual(index);
	void *it2 = createIterator_dual(index);
	uint value;

	
//	setColorRojo(); printf("\n\t\t init_po_dual(it2,377,16960404)");  init_po_dual(it2,377,16960404); setColorNormal();  //
//	printIterator_dual(it2);
//
//		setColorVerde();;printf("\n\t\t leap_dual(it2,SUBJECT,1) = %u" , leap_dual(it2,SUBJECT,1) ); setColorNormal();   //16815794            
//	printIterator_dual(it2);
//
//		setColorVerde();;printf("\n\t\t leap_dual(it2,SUBJECT,16815794+1) = %u" , leap_dual(it2,SUBJECT,16815794+1) ); setColorNormal();   //16917576             
//	printIterator_dual(it2);
//
//		setColorVerde();;printf("\n\t\t leap_dual(it2,SUBJECT,16917576+1) = %u" , leap_dual(it2,SUBJECT,16917576+1) ); setColorNormal();   0//             
//	printIterator_dual(it2);
//


	setColorRojo(); printf("\n\t\t init_p_dual(it1,377)");  init_p_dual(it1,377); setColorNormal();
	printIterator_dual(it1);

		setColorVerde();;printf("\n\t\t leap_dual(it1,OBJECT,1) = %u" , leap_dual(it1,OBJECT,1) ); setColorNormal();               //
	printIterator_dual(it1);

		setColorVerde();;printf("\n\t\t leap_dual(it1,OBJECT,16960404) = %u" , leap_dual(it1,OBJECT,16960404) ); setColorNormal(); //
	printIterator_dual(it1);


exit(0);
	uint value1 ,value2;
	value1=1;
	value2=1;
	
	 uint i=0;
	 while (value1 !=0 && value2 !=0) {
			value1 = leap_dual(it1,SUBJECT,value2);			
			value2 = leap_dual(it2,SUBJECT,value1);
			if (value1== value2) {
				printf("\n\t match #%u found triples  < %u 353 ?o > < %u 280 ?o >", i, value1,value2);
				i++;
				// printf("\n ** ITERADOR 1 **");
				// printIterator_dual(it1);
				// printf("\n ** ITERADOR 2 **");
				// printIterator_dual(it2);
				// break;
				value2 = leap_dual(it2,SUBJECT,value1+1);
			}
	 }
	
	printf("\n\n");

	
	
}












/*************************************************************************************************************/

/*************  TEST PO*****************************************************/
//1st parameter is a pointer to a tdualrdfcsa
//typedef struct {
//	twcsa *spo;
//	twcsa *ops;
//} tdualrdfcsa;

//  TESTING *************************************************************

int bis_gr_graphsuffixCmp_POS(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	
	if ( *(a+1) < *(b+1)) return -1;
	else if ( *(a+1) > *(b+1)) return +1;

	if ( *(a+2) < *(b+2)) return -1;
	else if ( *(a+2) > *(b+2)) return +1;

	if ( *(a) < *(b)) return -1;
	else if ( *(a) > *(b)) return  +1;

	
	//for (i=0; i<gr_nentriesGicsa;i++) {
	//	if (*a <*b) return -1;
	//	if (*a >*b) return +1;
	//	a++;b++;
	//}
	
	return 0;
}


void bis_gr_sortRecords_SPO_into_POS(uint *s, size_t n, uint nEntries) {
	//graph->nEntries;
	//graph->n;
	qsort(s , n , sizeof(uint)*nEntries, bis_gr_graphsuffixCmp_POS);
}

void bis_gr_shiftRecords_SPO_into_POS(uint *buff, size_t n, uint nEntries) {
	size_t i,j,z=0;
	uint s,p,o;
	for (i=0; i<n; i++){
		s=buff[i*nEntries  ];
		p=buff[i*nEntries+1];
		o=buff[i*nEntries+2];
		
		buff[i*nEntries  ] = p;
		buff[i*nEntries+1] = o;
		buff[i*nEntries+2] = s;
		
	}					

}




void * creaIterador_doble_down_FALLA(void * index, int type1, uint value1, int type2, uint value2) {
	
	void *it = createIterator_dual(index);
	//printIterator_dual(it);
	//init_o_dual(it,24226252);
	down_dual(it, type1, value1); 
	down_dual(it, type2, value2); 
	return it;
}

void * creaIterador_doble_down_OK(void * index, int type1, uint value1, int type2, uint value2) {
	
	void *it = createIterator_dual(index);
	//printIterator_dual(it);
	init_po_dual(it,value1,value2);
	//down_dual(it, type1, value1); 
	//down_dual(it, type2, value2); 
	return it;
}


void * creaIterador_doble_down(void * index, int type1, uint value1, int type2, uint value2) {
	
	void *it = createIterator_dual(index);
	//printIterator_dual(it);
	init_p_dual(it, value1);
	//down_dual(it, type1, value1); 
	leap_dual(it, type2, 0);
	down_dual(it, type2, value2); 
	return it;
}




int dual_test_po_op_getall_getOne_next(void *index) {
	printf("\n call to dual_test_po_op_getall_getOne_next\n");

double start ,end;

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	twcsa *g = dualrdf->spo;
	twcsa *g2 = dualrdf->ops;
	

	uint *buffer;
	size_t size_buffer;
	printf("\n dumping source data \n");fflush(stdout);
		dumpSourceData(g, &buffer, &size_buffer);
	printf("... %zu triplets recovered (%zu expected)\n",size_buffer / 3, g->n);fflush(stdout);

	/*
				{
					size_t i,j,z=0;
					z=0;
					z+=g->nEntries;
					for (i=1; i<g->n; i++){
						printf("\n <");
						for (j=0;j<g->nEntries;j++) {
							fprintf(stdout,"%u ",buffer[z]);							
							z++;
						}		
						printf(">");										
					}					
				}
	*/
	
	bis_gr_sortRecords_SPO_into_POS(buffer , size_buffer/g->nEntries, g->nEntries);
	printf("\n sort records passed");fflush(stdout);
	//now triples are sorted by P,O,S
	bis_gr_shiftRecords_SPO_into_POS(buffer, size_buffer/g->nEntries, g->nEntries);
	printf("\n shift records passed");fflush(stdout);	
	//now reordered internally accordingly
	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets po* and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) ) {
			j+=3;
			
			//if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[j+2],B[i],B[i+1]);
			if (!(j%1000000)    && (i<=50000000)) printf("\n\t<s,p,o>= %u, %u, %u", B[j+2],B[i],B[i+1]);
			
		}
		f[z/3]=(j-i)/3;
		B[z] = B[i];
		B[z+1] = B[i+1];
		B[z+2] = B[i+2];
		z+=3;
		
		i=j;
	}

	n= z/3;
	printf("\n n=%lu, z=%lu, i=%lu, j= %lu\n", n,z,i,j);fflush(stdout);
	
	// now  B[0.. 3n-1] has the "n" different triplets.
	//freq[0..n-1] has the "n" frequencies of such triplets.
	printf("\n source triplets = %zu, different triplets <\?,p,o> = %lu\n",size_buffer/3,n);




//  std::vector <uint> get_all_dual (void *iterator, int type)
//  {   std::vector <uint> res;
//  //    añadir a vector res -->    res.push_back( 3333);
//  	res.push_back(444);
//      return res;
//  }

 
 	/***************/	
 	{
 	srand(time(NULL));
 start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o> ==> dual_test_po_op_getall_getOne_next() -- test using all existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <*,%u,%u> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;	
	for (i=1; i< n ; i++) {
		
 		//results  = dual_rdfcsaSPO_po(g , B[i*3],B[i*3+1], res);   
 		//results1 = dual_rdfcsaOPS_op(g2, B[i*3+1],B[i*3], res2);

		int type1 = PREDICATE;
		uint value1 = B[i*3];
		int type2 = OBJECT;
		uint value2= B[i*3+1];
		int TYPE3 = SUBJECT;		
		
		void *it = creaIterador_doble_down(index, type1, value1, type2, value2);

		std::vector <uint> res1 = get_all_dual(it, TYPE3);
		
		
		std::vector <uint> res2;		
		uint value;
		get_one_dual_init (it, TYPE3 );
		
		while ((value = get_one_dual_next (it)) != 0) {
			res2.push_back(value);
		}

		if (i==20345) {printf("\n i= 20345\n"); printIterator_dual(it);}
		
		it =  destroyIterator_dual(it);
 		
		if ( (f[i] != res1.size()) ||  (f[i] != res2.size()) ) {
			printf("\n po operation failed (i=%lu) ::-sizes-differ- res1.size()=%u and res2.size() = %u  (expected-size= %u)!!\n", i, (uint)res1.size(), (uint)res2.size(),f[i]);
			printf("\n<\?,p,o>= <%u, %u, %u>",B[i*3+2], value1,value2 );

			printf("\n\n Retrieved triplet failed:   <\?,p,o> ==> <%u,%u,%u>!= <%u,%u,%u>\n", 
			res1[0], value1,value2, res2[0],value1,value2 );fflush (stdout);			
			exit(0);
		}
//		else {
//			printf("\n po operation succeeded (i=%lu) ::-sizes-match- res1.size()=%u and res2.size() = %u  (expected-size= %u)!!\n", i, (uint)res1.size(), (uint)res2.size(),f[i]);
//		}
		
		uint results = res1.size();
		if (results > 0) {
			
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++) {
				if ( res1[x] != res2[x]) {
	
					printf("\n\n Retrieved triplet failed:   <\?,p,o> ==> <%u,%u,%u>!= <%u,%u,%u>\n", 
					res1[x], value1,value2, res2[x],value1,value2 );fflush (stdout);

					exit(0);
				}		
			}		
		}	
		total_results +=results;
		
		

		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <\?po> passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
 	}
 	/***************/	
 



 	/***************/	
 	{
 	srand(time(NULL));
	start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o> ==> measuring_getall() only:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <*,%u,%u> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;	
	for (i=1; i< n ; i++) {
		

		int type1 = PREDICATE;
		uint value1 = B[i*3];
		int type2 = OBJECT;
		uint value2= B[i*3+1];
		int TYPE3 = SUBJECT;		
		
		void *it = creaIterador_doble_down(index, type1, value1, type2, value2);

		std::vector <uint> res1 = get_all_dual(it, TYPE3);
		
		
//		std::vector <uint> res2;		
//		uint value;
//		get_one_dual_init (it, TYPE3 );
		
//		while ((value = get_one_dual_next (it)) != 0) {
//			res2.push_back(value);
//		}

		
		it =  destroyIterator_dual(it);
		
		uint results = res1.size();
		total_results +=results;
		
		

		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <\?po> getall() passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
 	}
 	/***************/	
 



 	/***************/	
 	{
 	srand(time(NULL));
	start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o> ==> measuring_getOne_next() only:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <*,%u,%u> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;	
	for (i=1; i< n ; i++) {
		

		int type1 = PREDICATE;
		uint value1 = B[i*3];
		int type2 = OBJECT;
		uint value2= B[i*3+1];
		int TYPE3 = SUBJECT;		
		
		void *it = creaIterador_doble_down(index, type1, value1, type2, value2);

//		std::vector <uint> res1 = get_all_dual(it, TYPE3);
		
		
		std::vector <uint> res2;		
		uint value;
		get_one_dual_init (it, TYPE3 );
		
		while ((value = get_one_dual_next (it)) != 0) {
			res2.push_back(value);
		}

		
		it =  destroyIterator_dual(it);
		
		uint results = res2.size();
		total_results +=results;
		
				
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <\?po> geta_one_next() passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
 	}
 	/***************/	
 

	free(buffer);
	free(f);
	
	return 0;
}












/************************************************************************************************************/
void * creaIterador_doble_down_sp(void * index, int type1, uint value1, int type2, uint value2) {
	
	void *it = createIterator_dual(index);
	//printIterator_dual(it);
	init_s_dual(it, value1);
	//down_dual(it, type1, value1); 
	leap_dual(it, type2, value2);
	down_dual(it, type2, value2); 
	return it;
}


void * creaIterador_doble_down_sp_ok(void * index, int type1, uint value1, int type2, uint value2) {
	
	void *it = createIterator_dual(index);
	//printIterator_dual(it);
	init_sp_dual(it,value1,value2);
	//down_dual(it, type1, value1); 
	//down_dual(it, type2, value2); 
	return it;
}


void * creaIterador_doble_down_ps(void * index, int type1, uint value1, int type2, uint value2) {
	
	void *it = createIterator_dual(index);
	//printIterator_dual(it);
	init_p_dual(it, value2);	
	leap_dual(it, type1, value1);
	down_dual(it, type1, value1); 
	return it;
}
//void * creaIterador_doble_down_ps_ok(void * index, int type1, uint value1, int type2, uint value2) {
//	
//	void *it = createIterator_dual(index);
//	//printIterator_dual(it);
//	init_ps_dual(it, value2,value1);	 //NO EXISTE ESTA FUNCION
//	//leap_dual(it, type1, value1);
//	//down_dual(it, type1, value1); 
//	return it;
//}


int dual_test_sp_ps_getall_getOne_next(void *index) {
	printf("\n call to dual_test_sp_ps_getall_getOne_next\n");

double start ,end;

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	twcsa *g = dualrdf->spo;
	twcsa *g2 = dualrdf->ops;
	

	uint *buffer;
	size_t size_buffer;
	printf("\n dumping source data \n");fflush(stdout);
		dumpSourceData(g, &buffer, &size_buffer);

	//uint *buffer2;
	//size_t size_buffer2;
	//	dumpSourceData(g2, &buffer2, &size_buffer2);
	printf("... %zu triplets recovered (%zu expected)\n",size_buffer / 3, g->n);fflush(stdout);

	/*
				{
					size_t i,j,z=0;
					z=0;
					z+=g->nEntries;
					for (i=1; i<g->n; i++){
						printf("\n <");
						for (j=0;j<g->nEntries;j++) {
							fprintf(stdout,"%u ",buffer[z]);							
							z++;
						}		
						printf(">");										
					}					
				}
	*/
	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets sp* and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) ) {
			j+=3;
			
			if (!(j%1000000)   ) printf("\n\t<s,p,o>= %u, %u, %u", B[j],B[i+1],B[i+2]);
			
		}
		f[z/3]=(j-i)/3;
		B[z] = B[i];
		B[z+1] = B[i+1];
		B[z+2] = B[i+2];
		z+=3;
		
		i=j;
	}

	n= z/3;
	printf("\n n=%lu, z=%lu, i=%lu, j= %lu\n", n,z,i,j);fflush(stdout);
	
	// now  B[0.. 3n-1] has the "n" different triplets.
	//freq[0..n-1] has the "n" frequencies of such triplets.
	printf("\n source triplets = %zu, different triplets <s,p,\?> = %lu\n",size_buffer/3,n);

 
 	/***************/	
 	{
 	srand(time(NULL));
 start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o> ==> dual_test_sp_ps_getall_getOne_next() -- test using all existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <%u,%u,*> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;
	for (i=1; i< n ; i++) {
 		
 //		results  = dual_rdfcsaSPO_sp(g , B[i*3],B[i*3+1], res);   
 //		results1 = dual_rdfcsaOPS_ps(g2, B[i*3+1],B[i*3], res2);
		

		int type1 = SUBJECT;
		uint value1= B[i*3];
		int type2 = PREDICATE;
		uint value2 = B[i*3+1];
		int TYPE3 = OBJECT;		
		
		void *it = creaIterador_doble_down_sp_ok(index, type1, value1, type2, value2);
//		void *it = creaIterador_doble_down_sp(index, type1, value1, type2, value2);
//		void *it = creaIterador_doble_down_ps(index, type1, value1, type2, value2);

		std::vector <uint> res1 = get_all_dual(it, TYPE3);
				
		std::vector <uint> res2;		
		uint value;
		get_one_dual_init (it, TYPE3 );
		
		while ((value = get_one_dual_next (it)) != 0) {
			res2.push_back(value);
		}
		
		if (i==41) {printf("\n i= 41\n"); printIterator_dual(it);}
		it =  destroyIterator_dual(it);
 		
		if ( (f[i] != res1.size()) ||  (f[i] != res2.size()) ) {
			printf("\n sp operation failed (i=%lu) ::-sizes-differ- res1.size()=%u and res2.size() = %u  (expected-size= %u)!!\n", i, (uint)res1.size(), (uint)res2.size(),f[i]);
			printf("\n<s,p,\?>= <%u, %u, %u>",value1,value2,B[i*3+2] );

			printf("\n\n Retrieved triplet failed:   <s,p,\?> ==> <%u,%u,%u>!= <%u,%u,%u>\n", 
			value1,value2, res1[0],value1,value2,res2[0] );fflush (stdout);			
			exit(0);
		}
//		else {
//			printf("\n po operation succeeded (i=%lu) ::-sizes-match- res1.size()=%u and res2.size() = %u  (expected-size= %u)!!\n", i, (uint)res1.size(), (uint)res2.size(),f[i]);
//		}
		
		uint results = res1.size();
		if (results > 0) {
			
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++) {
				if ( res1[x] != res2[x]) {
	
					printf("\n\n Retrieved triplet failed:   <s,p,\?> ==> <%u,%u,%u>!= <%u,%u,%u>\n", 
					value1,value2,res1[x], value1,value2, res2[x] );fflush (stdout);

					exit(0);
				}		
			}		
		}	
		total_results +=results;
		
		

		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <sp\?> passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
 	}
 	/***************/	
 



 	/***************/	
 	{
 	srand(time(NULL));
	start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> ==> measuring_getall() only:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <*,%u,%u> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;	
	for (i=1; i< n ; i++) {
		

		int type1 = SUBJECT;
		uint value1= B[i*3];
		int type2 = PREDICATE;
		uint value2 = B[i*3+1];
		int TYPE3 = OBJECT;		
		
//		void *it = creaIterador_doble_down(index, type2, value2, type1, value1);
		void *it = creaIterador_doble_down_sp_ok(index, type1, value1, type2, value2);

		std::vector <uint> res1 = get_all_dual(it, TYPE3);
		
		
//		std::vector <uint> res2;		
//		uint value;
//		get_one_dual_init (it, TYPE3 );
		
//		while ((value = get_one_dual_next (it)) != 0) {
//			res2.push_back(value);
//		}

		
		it =  destroyIterator_dual(it);
		
		uint results = res1.size();
		total_results +=results;
		
		

		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <s,p,\?> getall() passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
 	}
 	/***************/	
 



 	/***************/	
 	{
 	srand(time(NULL));
	start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> ==> measuring_getOne_next() only:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <*,%u,%u> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;	
	for (i=1; i< n ; i++) {
		

		int type1 = SUBJECT;
		uint value1= B[i*3];
		int type2 = PREDICATE;
		uint value2 = B[i*3+1];
		int TYPE3 = OBJECT;		
		
//		void *it = creaIterador_doble_down(index, type2, value2, type1, value1);
		void *it = creaIterador_doble_down_sp_ok(index, type1, value1, type2, value2);

//		std::vector <uint> res1 = get_all_dual(it, TYPE3);
		
		
		std::vector <uint> res2;		
		uint value;
		get_one_dual_init (it, TYPE3 );
		
		while ((value = get_one_dual_next (it)) != 0) {
			res2.push_back(value);
		}

		
		it =  destroyIterator_dual(it);
		
		uint results = res2.size();
		total_results +=results;
		
				
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <s,p,\?> geta_one_next() passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
 	}
 	/***************/	
 


	free(buffer);	
	free(f);
	
	return 0;
}

