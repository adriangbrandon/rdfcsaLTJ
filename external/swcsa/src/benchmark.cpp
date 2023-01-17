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
#define IFERROR(error) {{if (error) { fprintf(stderr, "%s\n", error_index(error)); exit(1); }}}

#include "debug.h"
#include "arraysort.c"  //@@


#define Q_SPO  1 
#define Q_SPO0 10
#define Q_SPO1 11
#define Q_SPO2 12
#define Q_SPO9 19

#define Q_SP 2
#define Q_SP0 20
#define Q_SP1 21
#define Q_SP2 22
#define Q_SP9 29

#define Q_PO 3
#define Q_PO0 30
#define Q_PO1 31
#define Q_PO2 32
#define Q_PO9 39

#define Q_SO 4
#define Q_SO0 40
#define Q_SO1 41
#define Q_SO2 42
#define Q_SO9 49

#define Q_S  5
#define Q_S0  50
#define Q_S9  59

#define Q_P  6
#define Q_P0  60
#define Q_P9  69

#define Q_O  7
#define Q_O0  70
#define Q_O9  79

#define Q_VVV  8

typedef struct squery {
        int s;
        int p;
        int o;
        int expectednres;
} tquery;

tquery * readQueries  (char *filename, int nQueries);
tquery * readQueries2(char *filename,  int *nQueries);
void alighnnum(char *str, int len);

#define LOGTIMES
/**/ FILE *gnuFile;
/**/ char operation[10];
/**/ char *indexbasename;

int main(int argc, char ** argv) {        

	void *index;

	long totalres;
	if (argc < 5) {
			printf("Usage: %s <graphIdx> <queryFile> <tipoQuery> <iters> [<numQueries> [<numPred> <numSO> ]]\n", argv[0]);
			exit(1);
	}

	char * filename = argv[1]; //file: associated to the graph-index.
/**/indexbasename = argv[1];

	ulong Index_size, Text_length;
	int error= load_index (filename, &index);
	IFERROR (error);
	error = index_size(index, &Index_size);
	IFERROR (error);
	error = get_length(index, &Text_length);
	Text_length *= sizeof(uint);
	IFERROR (error);
	fprintf(stderr, "Index size = %lu Kb\n", Index_size/1024);
	//@@

	filename = argv[2];   //file: now for the queries.
	int queryType = atoi(argv[3]);
	int iters = atoi(argv[4]);
	
	int numPred,numSO, nqueries=-1; 
	if (argc ==6)
		nqueries = atoi(argv[5]);

	if (argc >6) {
		nqueries = atoi(argv[5]);
		numPred = atoi(argv[6]);
		numSO = atoi(argv[7]);
	}
	
	
	#ifdef LOGTIMES  
	/**/	char outFilenameGNU[256];	
	/**/	switch(queryType) {
	/**/	case Q_SPO:  { sprintf(operation,"spo"); break;}
	/**/	case Q_SPO0: { sprintf(operation,"spo0"); break;}
	/**/	case Q_SPO1: { sprintf(operation,"spo1"); break;}
	/**/	case Q_SPO2: { sprintf(operation,"spo2"); break;}
	/**/	case Q_SPO9: { sprintf(operation,"spo9"); break;}

	/**/	case Q_SP : { sprintf(operation,"spV"); break;}
	/**/	case Q_SP0 : { sprintf(operation,"spV0"); break;}
	/**/	case Q_SP1 : { sprintf(operation,"spV1"); break;}
	/**/	case Q_SP2 : { sprintf(operation,"spV2"); break;}
	/**/	case Q_SP9 : { sprintf(operation,"spV9"); break;}

	/**/	case Q_PO : { sprintf(operation,"Vpo"); break;}
	/**/	case Q_PO0 : { sprintf(operation,"Vpo0"); break;}
	/**/	case Q_PO1 : { sprintf(operation,"Vpo1"); break;}
	/**/	case Q_PO2 : { sprintf(operation,"Vpo2"); break;}
	/**/	case Q_PO9 : { sprintf(operation,"Vpo9"); break;}

	/**/	case Q_SO : { sprintf(operation,"sVo"); break;}
	/**/	case Q_SO0 : { sprintf(operation,"sVo0"); break;}
	/**/	case Q_SO1 : { sprintf(operation,"sVo1"); break;}
	/**/	case Q_SO2 : { sprintf(operation,"sVo2"); break;}
	/**/	case Q_SO9 : { sprintf(operation,"sVo9"); break;}
	
	/**/	case Q_S  : { sprintf(operation,"sVV"); break;}
	/**/	case Q_S0  : { sprintf(operation,"sVV0"); break;}
	/**/	case Q_S9  : { sprintf(operation,"sVV9"); break;}

	/**/	case Q_P  : { sprintf(operation,"VpV"); break;}
	/**/	case Q_P0  : { sprintf(operation,"VpV0"); break;}
	/**/	case Q_P9  : { sprintf(operation,"VpV9"); break;}

	/**/	case Q_O  : { sprintf(operation,"VVo"); break;}
	/**/	case Q_O0  : { sprintf(operation,"VVo0"); break;}
	/**/	case Q_O9  : { sprintf(operation,"VVo9"); break;}

	/**/	case Q_VVV  : { sprintf(operation,"VVV"); break;}
	
	
	
	/**/}
	/**/	sprintf(outFilenameGNU,"%s.dat",operation);
	/**/	struct stat bufgnu;
	/**/	if (-1 == stat(outFilenameGNU, &bufgnu)){
	/**/		gnuFile =fopen(outFilenameGNU,"w");
	/**/		if (!gnuFile) {printf("\n could not open GNUPLOT data file (%s)... exitting\n",outFilenameGNU); exit(0);}
	/**/		fprintf(gnuFile,"#%15s\t%15s\t%15s\t%15s\t%15s\t%15s\t%4s\t%8s\t%s\n","IdxSize(byte)","usec/pat","usec/occ","occs","numpats", "sourcesize","#oper", "#repeats","#indexBaseName");
	/**/	}
	/**/	else {
	/**/		gnuFile = fopen(outFilenameGNU,"a");
	/**/		if (!gnuFile) {printf("\n could not open GNUPLOT data file (%s)... exitting\n",outFilenameGNU); exit(0);}
	/**/	}
	#endif		
	
	
		
	tquery * queries;
	if (nqueries == -1)
		queries = readQueries2(filename,&nqueries);
	else
		queries = readQueries(filename,nqueries);
		
	if (!queries) {
		fprintf(stderr,"\n uname to read %d queries. Aborting!\n",nqueries); 
		exit(0);
	}
	else 
		fprintf(stderr,"\nI have read %d queries. \n",nqueries); 

	long results=0;
	int i;
	uint * res = (uint *) malloc (sizeof(uint) * (MAX_RESULTS * 3 +1));    //max_results constant defined in "interface.h" file
	if (!res) {
		perror("ERROR MALLOC");
		exit(0);
	}
	// res[0] = noccs  = number of <triplets to return>
	// res[(1..MAX_RESULTS) + (MAX_RESULTS)*0] = Subjects
	// res[(1..MAX_RESULTS) + (MAX_RESULTS)*1] = Predicates
	// res[(1..MAX_RESULTS) + (MAX_RESULTS)*2] = Objects

	#ifdef SHOW_RESULTS
		char msg[5];
		switch(queryType) {
			case Q_SPO: 
			case Q_SPO0: 
			case Q_SPO1: 
			case Q_SPO2: 
			case Q_SPO9: 
						{ sprintf(msg,"SPO"); break;}
			case Q_SP: 
			case Q_SP0: 
			case Q_SP1: 
			case Q_SP2 : 
			case Q_SP9 : 
						{ sprintf(msg,"SP\?"); break;}
			
			case Q_PO :
			case Q_PO0 :
			case Q_PO1 :
			case Q_PO2 : 
			case Q_PO9 : 
						{ sprintf(msg,"\?PO"); break;}

			case Q_SO :
			case Q_SO0 :
			case Q_SO1 :
			case Q_SO2 : 
			case Q_SO9 : 
						{ sprintf(msg,"S\?O"); break;}
			
			case Q_S   :
			case Q_S0  : 
			case Q_S9  : 
						{ sprintf(msg,"S\?\?"); break;}

			case Q_P   :			
			case Q_P0  : 
			case Q_P9  : 
						{ sprintf(msg,"\?P\?"); break;}

			case Q_O   : 			
			case Q_O0  : 
			case Q_O9  : 
						{ sprintf(msg,"\?\?O"); break;}
			
			case Q_VVV  : { sprintf(msg,"\?\?\?"); break;}
		}
	#endif	



	startClockTime();
    if (queryType ==Q_VVV) {    //ONLY FOR ??? query
		int z;
		for (z=0; z<iters;z++) {
			uint *buffer=NULL;
			totalres = vvv(index, &buffer);
			if(buffer) free(buffer);
        }	
	}
	else {		
	//	startClockTime();
		int z;
		for (z=0; z<iters;z++) {
			totalres = 0;
			for (i = 0; i < nqueries; i++) {
				tquery query = queries[i];
				//res = NULL;

				switch(queryType) {
					case Q_SPO:  { results =spo  (index, query.s, query.p, query.o, &res); break;}
					case Q_SPO0: { results =spo_0(index, query.s, query.p, query.o, &res); break;}
					case Q_SPO1: { results =spo_1(index, query.s, query.p, query.o, &res); break;}
					case Q_SPO2: { results =spo_2(index, query.s, query.p, query.o, &res); break;}
					case Q_SPO9: { results =spo_9(index, query.s, query.p, query.o, &res); break;}

					case Q_SP :  { results =sp   (index, query.s, query.p,          &res); break;}
					case Q_SP0 : { results =sp_0 (index, query.s, query.p,          &res); break;}
					case Q_SP1 : { results =sp_1 (index, query.s, query.p,          &res); break;}
					case Q_SP2 : { results =sp_2 (index, query.s, query.p,          &res); break;}
					case Q_SP9 : { results =sp_9 (index, query.s, query.p,          &res); break;}

					case Q_PO :  { results =po   (index,          query.p, query.o, &res); break;}
					case Q_PO0 : { results =po_0 (index,          query.p, query.o, &res); break;}
					case Q_PO1 : { results =po_1 (index,          query.p, query.o, &res); break;}
					case Q_PO2 : { results =po_2 (index,          query.p, query.o, &res); break;}
					case Q_PO9 : { results =po_9 (index,          query.p, query.o, &res); break;}

					case Q_SO  : { results =so   (index, query.s,          query.o, &res); break;}
					case Q_SO0 : { results =so_0 (index, query.s,          query.o, &res); break;}
					case Q_SO1 : { results =so_1 (index, query.s,          query.o, &res); break;}
					case Q_SO2 : { results =so_2 (index, query.s,          query.o, &res); break;}
					case Q_SO9 : { results =so_9 (index, query.s,          query.o, &res); break;}
					
					case Q_S   : { results =s    (index, query.s,                   &res); break;}
					case Q_S0  : { results =s_0  (index, query.s,                   &res); break;}
					case Q_S9  : { results =s_9  (index, query.s,                   &res); break;}

					case Q_P   : { results =p    (index,          query.p         , &res); break;}
					case Q_P0  : { results =p_0  (index,          query.p         , &res); break;}
					case Q_P9  : { results =p_9  (index,          query.p         , &res); break;}

					case Q_O   : { results =o    (index,                   query.o, &res); break;}
					case Q_O0  : { results =o_0  (index,                   query.o, &res); break;}
					case Q_O9  : { results =o_9  (index,                   query.o, &res); break;}
														
				}

				#ifndef EXPERIMENTS
					//Comentar para medir tiempos:
					if (results != query.expectednres) {
						fprintf(stderr,"\n Operations failed: results = %d  != expectedRes = %d, (s=%d, p=%d, o=%d, querytype=%d) \n", 
																results, query.expectednres, query.s,query.p,query.o, queryType);
						//exit(0);
					}
					
					#ifdef SHOW_RESULTS
					{ 	int j;
						uint *res_s = res+1;
						uint *res_p = res+1+ (MAX_RESULTS)*1;
						uint *res_o = res+1+ (MAX_RESULTS)*2;
						  printf("\n [%4d]Querying %s: %ld results for <%d,%d,%d> -->",i,msg, results,query.s,query.p,query.o);
						  for (j=0;j<res[0];j++) {
							  printf("<%u,%u,%u> ", res_s[j], res_p[j], res_o[j]);
						  }
						fflush(stdout);
					}					
					#endif
					
				#endif

				totalres += results;
				//if(res) free(res);  //only needed if "a malloc per query is done"
		//		printf("\n [%4d]Querying %s: %10ld results for <%d,%d,%9d> --> acum=%12lu",i,operation, results,query.s,query.p,query.o,totalres);

			}
		}
	}
			
        ulong microsecs = endClockTime()/1000/iters; //to microsecs

        // datasets.structura query_input num_queries totaloutput timeperquery timeperoutput
        
		printf("\n <index>\t<query-set>\t<microsec>\t<nqueries>\t<totalres>\t<microsec/query>\t<microsec/totalres>");
        printf("\n %s\t%s\t%ld\t%d\t%lu\t%lf\t%lf\n", argv[1], argv[2],
                       microsecs, nqueries, (ulong)totalres, (double)microsecs/nqueries, (double)microsecs/totalres);


		#ifdef LOGTIMES
		/**/	//fprintf (stdout, "\t** Operation = %s ** numpatts = %u\n", operation, nqueries);
		/**/	//fprintf (stdout, "\t**size_Index= %lu bytes   \n", Index_size);
		/**/	//fprintf (stdout, "\tTotal Num occs found = %lu\n", (ulong) totalres);
		/**/	//fprintf (stdout, "\tSearch time = %.6f s\n", microsecs/1000000.0);
		/**/	//fprintf (stdout, "\tSearch time/Num_occs = %.6f microsec/occss\n", (double)microsecs/totalres);
		/**/	//fprintf (stdout, "\tSearch_time/pat = %.8f microsec/pat\n", (double)microsecs/nqueries);

		/**/    char timeperpat[50]=""; sprintf(timeperpat,"%5.9f", (double)microsecs/nqueries );
		/**/    char timeperocc[50]=""; sprintf(timeperocc,"%5.9f", (double)microsecs/totalres );
		/**/	alighnnum(timeperpat,15);alighnnum(timeperocc,15);
		/**/	fprintf(gnuFile," %15ld\t",Index_size);
		/**/	fprintf(gnuFile,"%15s\t",timeperpat);
		/**/	fprintf(gnuFile,"%15s\t",timeperocc);
		/**/	fprintf(gnuFile,"%15lu\t",(ulong)totalres);
		/**/	fprintf(gnuFile,"%15u\t",nqueries);
		/**/	fprintf(gnuFile,"%15lu\t",Text_length -12);
		/**/	fprintf(gnuFile,"#%4s\t",operation);
		/**/	fprintf(gnuFile,"#%7d\t",iters);
		/**/	fprintf(gnuFile,"#%s\n",indexbasename);
		
		fclose(gnuFile);
		#endif
		
				
		free_index(index);
        free(queries);
        free(res);

        exit(0);
}














/********************************************************************************************************************/
/************** AUXILIAR FUNCTIONS **********************************************************************************/
/********************************************************************************************************************/

tquery * readQueries(char *filename, int nQueries) {
	unsigned int i;
	
	FILE * fqueries;
	if ((fqueries = fopen(filename, "r")) == NULL) {
			fprintf(stderr, "error: no se pudo abrir el fichero %s\n", filename);
		return NULL;
	}
	
	tquery *queries = (tquery *) malloc (nQueries * sizeof(tquery));
	for (i=0; i<nQueries;i++) {
		if (-1==fscanf(fqueries,"%d;%d;%d;%d\n",  &queries[i].s,&queries[i].p,&queries[i].o,&queries[i].expectednres)){
			fprintf(stderr,"error al leer queries\n");exit(1);
		}
	}
	fclose(fqueries);
	if (i==nQueries)
		return queries;
	
	free(queries);
	fclose(fqueries);
	return NULL;	
}



tquery * readQueries2(char *filename, int *nQueries) {
	unsigned int i=0;
	
	FILE * fqueries;
	if ((fqueries = fopen(filename, "r")) == NULL) {
			fprintf(stderr, "error: no se pudo abrir el fichero %s\n", filename);
		return NULL;
	}

	int maxQ=2; *nQueries=0;
	tquery *queries = (tquery *) malloc (maxQ * sizeof(tquery));
	
	while  (EOF!=fscanf(fqueries,"%d;%d;%d;%d\n",  &queries[i].s,&queries[i].p,&queries[i].o,&queries[i].expectednres)){
		(*nQueries)++;
		if (*nQueries >= maxQ) {
			maxQ *=2;
			queries = (tquery *) realloc(queries,maxQ * sizeof(tquery));
		}
		i++;
	}
	fclose(fqueries);
	
	return queries;	
}



/* used for gnuplot formatting file */
void alighnnum(char *str, int len){
	int n=strlen(str);
	int i;
	if (n<len) {
	  int shift = len-n;
	  for (i=len-1; i>=shift;i--)
		 str[i] = str[i-shift];
	  for (i=0;i<shift;i++) str[i]=' ';
	  
//for (i=0;i< len-n;i++)
//		str[i+n]=' ';
//
//	  for (i=0;i< len-n;i++)
//		str[i+n]=' ';
//	  str[len]='\0';
	}
}
