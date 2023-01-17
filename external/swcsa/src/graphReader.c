#include "graphReader.h"




/* @2023: loads the input data and adds both dummie node and sets gaps[nentries] to the value of the terminator  */

int parseFileADRIANformat_IntoGraph(char *infilebase, struct graphDB *graph) {
	//not yet defined ... parses Adrian's type graph DB and returns the proper struct graphDB 
		
	struct stat statbuf;
	if (stat(infilebase,&statbuf) ==-1) {
		perror("stat source file error:");
		return -1;
	}

	graph->nEntries = 3; // 3=RDF (number of entries per triple...)
	
	ulong infilesize=statbuf.st_size;
	uint ns, nso, no, gapobjects;
	char headerextension[100]=".header";
	if (-1== readNS_NO_NSO_gapobjects(infilebase,headerextension,&ns,&nso,&no,&gapobjects)) return -1;
	
	printf(" HEADER read: ns=%u - nso=%u - no=%u -gapobjects=%u \n",ns,nso,no,gapobjects);

	FILE *f = fopen(infilebase, "r"); 
	if(f == NULL) {perror(infilebase);return -1;}
	
	ulong i=0;
	uint s,p,o;
	
	ulong n=1000000000L; 
	uint *data = (uint *) my_malloc (n*sizeof(uint));

	uint mins=1000000000,minp=1000000000,mino=1000000000; 
	uint maxs=0,maxp=0,maxo=0; 
	
	
	char tmp[1024];
	uint len;	
	ulong totalbytes=0;
	ulong step = infilesize/10000;
	ulong stepi=0;

    //infilesize
	
	printf("\n");
	printf("\n -------------------------------");
	printf("\n Processing file: %s ", infilebase);
	printf("\n -------------------------------\n");
	
	while (3== fscanf(f, "%u %u %u\n", &s, &p, &o)) {
		//o-=gapobjects;   //do not substract this here, because it will be done in buildWcsa() funcion.
				//printf("%u - %u - %u \n",s,p,o);
		data[i++] =s;		
		data[i++] =p;
		data[i++] =o;
			sprintf(tmp,"%u %u %u\n", s,p,o);
			len = strlen(tmp);
			totalbytes +=len;
					
					
				o-=gapobjects; //OJO: no restado en data[i-1] !!!  --> sólo para computar bien "mino y maxo"
				
		if (s<mins) mins=s;
		if (s>maxs) maxs=s;
		if (p<minp) minp=p;
		if (p>maxp) maxp=p;
		if (o<mino) mino=o;
		if (o>maxo) maxo=o;
		
		if (i>n-5) {n*=2; data = (uint *) realloc(data, n*sizeof(uint) );}
		
		stepi+=len;
		if(stepi>step) {
			stepi=0;
			printf("\r\t processed %3.2f%% (%1.1f of %1.1f MiBytes); :: ntriples = %lu, Memusage data (%1.1f MiBytes)", (double)totalbytes / infilesize*100   , totalbytes*1.0/1024/1024, infilesize*1.0/1024/1024, i/3, 1.0*i*sizeof(uint)/1024/1024);
		}		
	}
	printf("\r\t processed %3.2f%% (%1.1f of %1.1f MiBytes); :: ntriples = %lu, Memusage data (%1.1f MiBytes)", (double)totalbytes / infilesize*100   , totalbytes*1.0/1024/1024, infilesize*1.0/1024/1024, i/3, 1.0*i*sizeof(uint)/1024/1024);
	printf("\n");
	



	// **
		//ADDS a dummy node to the graph (the 1st node must be node 0).	
	//	graph->n++;
	
	n=i+graph->nEntries +1;  // (3 uints for dummie + 1 extra for terminator)
	data = (uint *) realloc(data, n*sizeof(uint) );

		{ 			
			data[i++]=0;                   //s component
			data[i++]=0;                   //p component
			data[i++]=0 + gapobjects;      //o component  (we add gapobjects, because buildWcsa will substract "gapobjects" 
			                                            // from all objects.  (as gaps[2] will be equal to ns + np -gapobjects).
		}

		//ADD a terminator value
		//s[graph->nEntries * graph->n]=2>>30;  //avoid valgrind warnings
							//2014.04.29. Added on buildFacade.c::buildWCSA()
	// **


	
	n=i;
	ulong ntriples=n/3;
	
//	data = realloc(data, n*sizeof(uint) );
//
	printf("\n -------------------------------");
	printf("\n summary: ");
	printf("\n num triples = %lu",ntriples);
	printf("\n %lu uints read ==> first triple = <%u %u %u>  -- last triple -1  = <%u %u %u> -- last triple (dummie)  = <%u %u %u>\n", n, data[0],data[1], data[2] , data[n-6], data[n-5], data[n-4]   ,     data[n-3], data[n-2], data[n-1]);
	printf("\n mins = %9u, maxs= %9u", mins, maxs);
	printf("\n minp = %9u, maxp= %9u", minp, maxp);
	printf("\n mino = %9u, maxo= %9u", mino, maxo);
	printf("\n -------------------------------");
	printf("\n");
	
//	array_sortRecordsSPO(data, ntriples, 3);
	


	// END CHECK NO HAY HUECOS 

	graph->s = data;   //includes <s1,p1,o1> <s2,p2,o2>  ... <sk,pk,ok> <0,0,gapobjects>   ... where k=n-1
	                   //we are not substracting gapobjects from the objects,
	                   //but gaps[2] is set to g->ns + g->np -gapobjects.
	                   //so that buildWCSA will substract "gapobjects" to all the values accordingly.
	                   //and gaps[] will be set so that mapID and unmapID work.
		
	graph->n = ntriples;
	graph->ns = ns + nso;
	graph->np = maxp;
	graph->no = no + nso;
	graph->nso = nso;

	graph->ns++;  //due to dummie node
	graph->np++;  //due to dummie node
	graph->no++;  //due to dummie node	

	
	int *gaps = (int *) my_malloc (sizeof(int) * (graph->nEntries+1));
	gaps[0] = 0;
	gaps[1] = graph->ns;
	gaps[2] = graph->ns + graph->np - gapobjects;
	gaps[3] = graph->ns + graph->np + graph->no;  //terminator for suffix sort!

	//setGaps(&graph, gaps);
	graph->gaps = gaps;
	graph->gapobjects = gapobjects;


	
	printf("\n ----- after parseAdrian-------------------");
	printf("\n graph->n = %9lu", graph->n);
	printf("\n graph->ns = %9u, graph->np= %9u, graph->no= %9u, graph->nso= %9u", graph->ns, graph->np, graph->no, graph->nso);
	printf("\n graph->gaps =[ %d, %d, %d, %d]", gaps[0], gaps[1], gaps[2], gaps[3]);
	printf("\n graph->gapobjects =  %u]", graph->gapobjects);
	printf("\n -------------------------------");


	//free(data);
	fclose(f);	



	fprintf(stderr, "\n\n MAIN VALUES: ns(+1dummy)=%u, np(+1dummy)= %u, no(+1dummy)=%u, nso=%u, n=%zu (n-1dummy=%zu)\n", 
					 graph->ns, graph->np, graph->no, graph->nso, graph->n, graph->n-1);


	gr_printRecords(graph);

	
	if (gr_areSortedRecords(graph)) {
		fprintf(stderr,"\n INPUT RECORDS ARE SORTED INCREASINGLY");
	} else {
		fprintf(stderr,"\n INPUT RECORDS ARE #not# SORTED INCREASINGLY");
		gr_sortRecords(graph);	
	}

	if (gr_areSortedRecords(graph)) {
		fprintf(stderr,"\n SORTED RECORDS ARE SORTED INCREASINGLY");
	} else {
		fprintf(stderr,"\n SORTED RECORDS ARE #not# SORTED INCREASINGLY");
	}

	gr_printRecords(graph);
	
	
		
		
	return 0;
}




int readNS_NO_NSO_gapobjects (char * filename, char *extension, uint *ns, uint *nso, uint *no, uint *gapobjects) {
	char path[10000];
	sprintf(path,"%s%s",filename,extension);
	FILE *f = fopen(path, "r"); 
	if(f == NULL) return -1;
	
	if (3!= fscanf(f, "%u %u %u\n", ns, nso, no)) {
		printf("\n unable to read file: %s\n",filename);
		return -1;
	}
	//*ns += *nso;
	//*no += *nso;
	*gapobjects = *ns ;
	//*gapobjects = 0;
	
	fclose (f);
	return 0;
};
	



//loads the input data and adds both dummie node and sets gaps[nentries] to the value of the terminator 
int parseFileHDTformat_IntoGraph(char *infilebase, struct graphDB *graph) {
	// ---------- Set Graph properties and read input -----------------------
	int error;
	char infileHDR[4096], infileTRIPLES[4096];
	sprintf(infileHDR,"%s.header",infilebase);
	sprintf(infileTRIPLES,"%s.triples",infilebase);

	graph->nEntries = 3; // 3=RDF (number of entries per record/contact...)
	 	
	FILE *f = fopen(infileHDR, "r"); 
	if(f == NULL) return 1;
	error = gr_readHeader(graph, f);
	IFERROR(error);
	fclose (f);

	f = fopen(infileTRIPLES, "r"); 
	if(f == NULL) return 1;
	error = gr_readRecords(graph,f);
	IFERROR(error);
	fclose(f);
	
	fprintf(stderr, "\n\n MAIN VALUES: ns(+1dummy)=%u, np(+1dummy)= %u, no(+1dummy)=%u, nso=%u, n=%zu (n-1dummy=%zu)\n", 
					 graph->ns, graph->np, graph->no, graph->nso, graph->n, graph->n-1);


	gr_printRecords(graph);

	
	if (gr_areSortedRecords(graph)) {
		fprintf(stderr,"\n INPUT RECORDS ARE SORTED INCREASINGLY");
	} else {
		fprintf(stderr,"\n INPUT RECORDS ARE #not# SORTED INCREASINGLY");
		gr_sortRecords(graph);	
	}

	if (gr_areSortedRecords(graph)) {
		fprintf(stderr,"\n SORTED RECORDS ARE SORTED INCREASINGLY");
	} else {
		fprintf(stderr,"\n SORTED RECORDS ARE #not# SORTED INCREASINGLY");
	}

	gr_printRecords(graph);

	// START CHECK NO HAY HUECOS 

		int *contar = (int *) malloc ((graph->ns + graph->np + graph->no+10 )*sizeof(uint));
		int j;
		//test no huecos en los sujetos
		for (j=0;j<graph->ns;j++) contar[j]=0;	
		for (j=0;j<graph->n;j++)
			contar[graph->s[j*3]]++;

		for (j=0;j<graph->ns;j++) 
			if (contar[j]==0){
				printf("\n\n hay huecos en los sujetos\n");
				break;
			}
		if (j==graph->ns)
			printf("\n\n **sin huecos en sujetos**");
	

		//test no huecos en los predicados
		for (j=0;j<graph->np;j++) contar[j]=0;	
		for (j=0;j<graph->n;j++)
			contar[graph->s[j*3+1]]++;

		for (j=0;j<graph->np;j++) 
			if (contar[j]==0){
				printf("\n hay huecos en los predicados\n");
				break;
			}
		if (j==graph->np)
			printf("\n **sin huecos en predicados**");



		//test no huecos en los objetos
		for (j=0;j<graph->no;j++) contar[j]=0;	
		for (j=0;j<graph->n;j++)
			contar[graph->s[j*3+2]]++;

		for (j=0;j<graph->no;j++) 
			if (contar[j]==0){
				printf("\n hay huecos en los objetos\n");
				break;
			}
		if (j==graph->no)
			printf("\n **sin huecos en objetos**");


		free(contar);
		

	// END CHECK NO HAY HUECOS 
	
	int *gaps = (int *) my_malloc (sizeof(int) * (graph->nEntries+1));
	gaps[0] = 0;
	gaps[1] = graph->ns;
	gaps[2] = graph->ns + graph->np;
	gaps[3] = graph->ns + graph->np + graph->no;  //terminator for suffix sort!

	//setGaps(&graph, gaps);
	graph->gaps = gaps;
	graph->gapobjects = 0;
	
	// ---------- End Set Graph properties and read input ---------------	
	return 0;	
}


/***************************************************************************/



// *********************************************************************** 

int gr_readHeader (struct graphDB *graph, FILE *f) {
	uint np,ns,no,nso,n;
	uint values[9];

	if (9!=fread(values,sizeof(uint),9,f)) {
		perror("unable to read RDF.header");
		exit(0);
	}
	
	// ** NOTE: input values must be given in that order: ns, np, no, nso, n
	graph->ns= values[4];
	graph->np= values[5];
	graph->no= values[6];
	graph->nso=values[7];
	graph->n=  values[8];

	graph->ns++;  //due to dummie node
	graph->np++;  //due to dummie node
	graph->no++;  //due to dummie node

	return 0;
}



int gr_readRecords(struct graphDB *graph, FILE *f ) {

	uint nentries = graph->nEntries;
	uint *s = (uint *) my_malloc ((graph->nEntries * (graph->n+1) +1) * sizeof(uint)) ;  
    //EXTRA uint for terminator and 4 uints for dummy contact	
	//s[graph-	>nEntries * graph->n]=2>>30;  //avoid valgrind warnings
	//2014.04.29. Added on buildFacade.c::buildWCSA()
	
	size_t to_read = graph->n * nentries;
	size_t i=0;
	uint j;


	{
		unsigned int data[nentries];
	
		while (i<to_read) {
			ssize_t err_r= fread(data, sizeof(uint), nentries, f);
		//	s[i++]=data[0]; //p			
		//	s[i++]=data[1];	//s		
		//	s[i++]=data[2];	//o		

			s[i++]=data[1]; //s		
			s[i++]=data[0];	//p	
			s[i++]=data[2];	//o	

			// printf("\n[%u][%u][%u]",data[0],data[1],data[2]);
			if(i%600000==0) fprintf(stderr, "Processing %.1f%%\r", (float)i/to_read*100);
			if (i==to_read) break;
		}
		fprintf(stderr, "Processing %.1f%%", (float)i/to_read*100);
	}

	
	if (i!=to_read) {
		fprintf(stderr,"\n ERROR reading graph: read %lu, to_read %lu", (ulong) i, (ulong) to_read);
	}	
	assert(i == to_read);

// **
	//ADDS a dummy node to the graph (the 1st node must be node 0).	

	graph->n++;

	{ 	uint z; 
		for (z = 0; z < nentries; z++) {
	 	// If we want to put it as the last record
		   s[i++]=0; 
		// If we want to put it as the first record
		// s[z] = 0;
		}
	 	
		//s[i++]=0;
		//s[i++]=0;
		//s[i++]=0;
	}

	//ADD a terminator value
	//s[graph->nEntries * graph->n]=2>>30;  //avoid valgrind warnings
						//2014.04.29. Added on buildFacade.c::buildWCSA()
// **


	graph->s = s;	
	return 0;	
}

void gr_printRecords(struct graphDB *graph) {
	
	fprintf(stderr,"\n\n %zu records read (%u entries per each one), ns=%u, np=%u, no=%u, nso=%u \n", graph->n, graph->nEntries, graph->ns, graph->np, graph->no, graph->nso);

	uint i,j,z;
	z=0;
	for (j=0;j<10;j++) {
		fprintf(stderr,"\n\t");
		for (i=0; i<graph->nEntries;i++){
			fprintf(stderr,"[%u]",graph->s[z++]);
		}
	}
	fprintf(stderr,"\n...\n...\n");		

	z= (graph->n/2-5) * (graph->nEntries);
	for (j=0;j<10;j++) {
		fprintf(stderr,"\n\t");
		for (i=0; i<graph->nEntries;i++){
			fprintf(stderr,"[%u]",graph->s[z++]);
		}
	}
	fprintf(stderr,"\n...\n...\n");		


	z= (graph->n-10) * (graph->nEntries);
	for (j=0;j<10;j++) {
		fprintf(stderr,"\n\t");
		for (i=0; i<graph->nEntries;i++){
			fprintf(stderr,"[%u]",graph->s[z++]);
		}
		fprintf(stderr,"\n");		
	}
	
}

int gr_areSortedRecords(struct graphDB *graph) {
	uint i,j,z;
	z=graph->nEntries;
	for (j=0; j<graph->n-1;j++) {
		uint nextz= z+graph->nEntries;
		for (i=0; i<graph->nEntries;i++){
			if (graph->s[z]  > graph->s[z - graph->nEntries]) {z=nextz;break;}
			if (graph->s[z]  < graph->s[z - graph->nEntries]) {fprintf(stderr,"\n UNSORTED: %u > %u  (record j=%u)\n", graph->s[z] , graph->s[z - graph->nEntries] ,j   ); return 0; }
			if (graph->s[z] == graph->s[z - graph->nEntries]) {z++;}			
		}
	}
	return 1;			
}


uint gr_nentriesGicsa; // Global variable used in quicksort graphsuffixCmp function
int gr_graphsuffixCmp(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	for (i=0; i<gr_nentriesGicsa;i++) {
		if (*a <*b) return -1;
		if (*a >*b) return +1;
		a++;b++;
	}
	return 0;

}

int gr_graphsuffixCmp_PSO(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	
	if ( *(a+1) < *(a+1)) return -1;
	else if ( *(a+1) > *(a+1)) return +1;

	if ( *(a) < *(a)) return -1;
	else if ( *(a) > *(a)) return  +1;

	if ( *(a+2) < *(a+2)) return -1;
	else if ( *(a+2) > *(a+2)) return +1;


	
	//for (i=0; i<gr_nentriesGicsa;i++) {
	//	if (*a <*b) return -1;
	//	if (*a >*b) return +1;
	//	a++;b++;
	//}
	
	return 0;

}


void gr_sortRecords(struct graphDB *graph) {
	gr_nentriesGicsa= graph->nEntries;
	qsort(graph->s, graph->n, sizeof(uint)*graph->nEntries, gr_graphsuffixCmp);
}

void gr_setGaps(struct graphDB *graph, int *gaps) {
	uint i;
	graph->gaps = (int *) my_malloc(sizeof(int) * (graph->nEntries+1));
	for (i=0; i<=graph->nEntries;i++) graph->gaps[i] = gaps[i];	
}

// -------------------------------------------------------------------














