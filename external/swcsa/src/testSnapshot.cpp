/*
 * This code was originally made by Guillermo de Bernardo <guillermo.debernardo@udc.es>
 *
 * It's evaluate the performance of the datastructure.
 *
 * Has been modified by Diego Caro <diegocaro@gmail.com>
 */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "timing.h"
//#include "tgs.h"
#include "interface.h"
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

#include "debug.h"

 #include "arraysort.c"  //@@

/*
FIXME: Non soporta un numero de nodos maior que MAX_INT.

As consultas son nodos aleatorios (non e unha permutacion,
pode haber repetidos)
*/


#define SNAPSHOT 9


int compareRes(unsigned int * l1, unsigned int * l2) {
        uint i;
        if (l1[0]!=l2[0]) return 1;
        for (i = 1; i <= l1[0]; i++) {
                if (l1[i] != l2[i])  {
                        printf("got value %u at index %d, expected %u\n", l1[i], i, l2[i]);
                        return 1;
                }
        }
        return 0;
}



int main(int argc, char ** argv) {

        char * fileName;

        //@@ struct tgs index;
        void *index;
        int totalres = 0;

        //@@ifstream f;
        uint gotres = 0;
        
        int tiempo = -1;

        if (argc != 3) {
                printf("Usage: %s <graphfile> <time-instant> \n  set time-instant = -1 for auto-loop", argv[0]);
                exit(1);
        }
        
        fileName = argv[1];
        tiempo = atoi(argv[2]);
        


        //@@ f.open(fileName, ios::binary);
        //@@ tgs_load(&index, f);        
        //@@ f.close();
        
        ulong Index_size, Text_length;
        int error= load_index (fileName, &index);
        IFERROR (error);
		error = index_size(index, &Index_size);
		IFERROR (error);
		error = get_length(index, &Text_length);
		Text_length *= sizeof(uint);
		IFERROR (error);
		fprintf(stderr, "Index size = %lu Kb\n", Index_size/1024);
	    //@@


	uint nqueries = 0;
	ulong microsecs;
	
	
	//**** only test tiempo = given parameter 
	if (tiempo != -1)  {

		startClockTime();
		
			uint tiempo = atoi(argv[2]);
			gotres = get_snapshot(index, tiempo);
			totalres+=gotres;
			nqueries++;
			

			microsecs = endClockTime()/1000; //to microsecs
			printf("\n get_snapshot(t=%4u) fast -> count = %u , slapsed time = %2.3f sec.\n",tiempo, gotres, microsecs/1000000.0); 
			fflush(stdout);fflush(stderr);


		startClockTime();
				
			uint gotres2 = get_snapshot_slow(index, tiempo);

			microsecs = endClockTime()/1000; //to microsecs
			printf("\n get_snapshot(t=%4u) slow -> count = %u , slapsed time = %2.3f sec.\n",tiempo, gotres, microsecs/1000000.0); 
			fflush(stdout);fflush(stderr);

			if (gotres == gotres2) {
				printf("\n \t check get_snapshot(t=%u) passed ok",tiempo);
			}
			else {
				printf("\n get_snapshot(t=%u) failed !!",tiempo);
				exit(0);
			}		
		exit(0);
	}
	
	
	uint steps=0;
	for (steps =0; steps < get_num_nodes(index) ; steps++) {
		tiempo = steps;
	
		startClockTime();
		
			gotres = get_snapshot(index, tiempo);
			totalres+=gotres;
			nqueries++;
			

			microsecs = endClockTime()/1000; //to microsecs
			printf("\n get_snapshot(t=%4u) fast -> count = %u , slapsed time = %2.3f sec.\n",tiempo, gotres, microsecs/1000000.0); 
			fflush(stdout);fflush(stderr);


		startClockTime();
				
			uint gotres2 = get_snapshot_slow(index, tiempo);

			microsecs = endClockTime()/1000; //to microsecs
			printf("\n get_snapshot(t=%4u) slow -> count = %u , slapsed time = %2.3f sec.\n",tiempo, gotres, microsecs/1000000.0); 
			fflush(stdout);fflush(stderr);

			if (gotres == gotres2) {
				printf("\n \t check get_snapshot(t=%u) passed ok",tiempo);
			}
			else {
				printf("\n get_snapshot(t=%u) failed !!",tiempo);
				exit(0);
			}
	}



        exit(0);

}
