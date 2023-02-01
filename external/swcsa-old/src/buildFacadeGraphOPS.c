
//--------- SORTING a sequence of SPO entries into an OPS sequence

//used to shift tid[ s1, p1, o1, s2,...] into tid[o1, p1, s1, o2,...] and sort entries

int shiftIntoOPS_order(uint *tid, size_t n, uint nEntries) {
	uint i;
	for (i=0; i<n; i++) {
		uint s,o; //p;
		s= tid[i*nEntries];
		//p= tid[i*nEntries+1];
		o= tid[i*nEntries+2];
		tid[i*nEntries] = o;
		tid[i*nEntries+2] = s;		
	}
	return 0;
}


//--------- SORTING tid[o1, p1, s1, o2, p2, s2,... ] into OPS order

uint gr_nentriesGicsaOPS; // Global variable used in quicksort graphsuffixCmp function INTO OPS order
int gr_graphsuffixCmpOPS(const void *arg1, const void *arg2) {
	uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	for (i=0; i<gr_nentriesGicsaOPS;i++) {
		if (*a <*b) return -1;
		if (*a >*b) return +1;
		a++;b++;
	}
	return 0;
}

void gr_sortRecordsOPS(struct graphDB *graph) {
	gr_nentriesGicsaOPS= graph->nEntries;
	qsort(graph->s, graph->n, sizeof(uint)*graph->nEntries, gr_graphsuffixCmpOPS);
}


int gr_areSortedRecordsOPS(struct graphDB *graph) {
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
//---------



//--------------------------------------------
//-- FUNCTIONS USED TO SORT (in SPO order) an array s containing an unordered set of S1,P1,O1,S2,P2,O2,... triples
//-- USED AT LEAST FOR dumpSourceDataSPO_fromOPS function (see buildFacade.c).
//--------------------------------------------

int array_areSortedRecordsSPO(uint *s, uint n, uint nEntries) {
	uint i,j,z;
	z=nEntries;
	for (j=0; j<n-1;j++) {
		uint nextz= z+nEntries;
		for (i=0; i<nEntries;i++){
			if (s[z]  > s[z - nEntries]) {z=nextz;break;}
			if (s[z]  < s[z - nEntries]) {fprintf(stderr,"\n UNSORTED: %u > %u  (record j=%u)\n", s[z] , s[z - nEntries] ,j   ); return 0; }
			if (s[z] == s[z - nEntries]) {z++;}			
		}
	}
	return 1;			
}


uint array_nentriesGicsaSPO; // Global variable used in quicksort graphsuffixCmp function
int array_graphsuffixCmpSPO(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	for (i=0; i<array_nentriesGicsaSPO;i++) {
		if (*a <*b) return -1;
		if (*a >*b) return +1;
		a++;b++;
	}
	return 0;
}



void array_sortRecordsSPO(uint *s, uint n, uint nEntries) {
	array_nentriesGicsaSPO= nEntries;
	qsort(s, n, sizeof(uint)* nEntries, array_graphsuffixCmpSPO);
}
