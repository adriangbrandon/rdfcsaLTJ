#include <stdlib.h>


/*************************************************************************/	
int po_0 (void *gindex, int P, int O, uint **buffer) { 
//int po_orig (void *gindex, int S, int P, uint **buffer){ 
// ../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
//  indexes/dbpedia16	dbpedia.queries/Vpo.txt	4110	496	10952	8.286290	0.375274
//		printf("\n@@@@@@@@@@@@@@@@ call to po (%d,%d)\n",P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = pp; pattern[1] = oo;
	ulong numocc,l,r;
	countIntIndex(g->myicsa, pattern , 2, &numocc, &l, &r);
	
	res[0]=0;

	if (numocc) {
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;
		ulong i;

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);	
		#endif		
		

		for (i=l; i<=r;i++) {
			uint n,s,o; 
			n = res[0];
			res[0]++;

			// predicates
			res_p[n] = P;			

			// objects
			#ifdef BUFFER_PSI_ON
				o=bufferpsi[i-l]; //o= getPsiicsa(g->myicsa, i);				
			#else
				o= getPsiicsa(g->myicsa, i);
			#endif		

			res_o[n] = O;		

			// subjects
			s= getPsiicsa(g->myicsa, o);
			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);	
			
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}	



/*************************************************************************/

int po_1 (void *gindex, int P, int O, uint **buffer) { 
//int po (void *gindex, int P, int O, uint **buffer) { 
// ../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
//  indexes/dbpedia16	dbpedia.queries/Vpo.txt	4370	496	10952	8.810484	0.399014
//		printf("\n@@@@@@@@@@@@@@@@ call to po (%d,%d)\n",P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = pp; pattern[1] = oo;
	ulong numocc,l,r;
	countIntIndex_Buffer (g->myicsa, pattern , 2, &numocc, &l, &r);
//		countIntIndex(g->myicsa, pattern , 2, &numocc, &l, &r);
	
	res[0]=0;

	if (numocc) {
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;
		ulong i;

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);	
		#endif		
		

		for (i=l; i<=r;i++) {
			uint n,s,o; 
			n = res[0];
			res[0]++;

			// predicates
			res_p[n] = P;			

			// objects
			#ifdef BUFFER_PSI_ON
				o=bufferpsi[i-l]; //o= getPsiicsa(g->myicsa, i);				
			#else
				o= getPsiicsa(g->myicsa, i);
			#endif		

			res_o[n] = O;		

			// subjects
			s= getPsiicsa(g->myicsa, o);
			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);	
			
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}		






/******************************************************************************/
// 2 selects for the intervals + checking upon the P-interval
// it is very slow because the source interval of P's contains many many entries.
/******************************************************************************/

//int po_fast_2s_and_check(void *gindex, int P, int O, unsigned int **buffer) {
int po_2 (void *gindex, int P, int O, uint **buffer) { 	
//int po (void *gindex, int P, int O, uint **buffer) { 
	
//
// ../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/Vpo.txt	1316861	496	10952	2654.961694	120.239317
//		printf("\n@@@@@@@@@@@@@@@@ call to po (%d,%d)\n",P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = oo;
		ulong numocco,lo,ro;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lo, &ro);
		
	#else
		//l= getSelecticsa(g->myicsa, pp+1);
		//r = getSelecticsa(g->myicsa, pp+2)-1;
		
		l= getSelectTablePredicates(g->myicsa,pp+1);
		r= getSelectTablePredicates(g->myicsa,pp+2)-1;

		
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lo,ro;
		lo= getSelecticsa(g->myicsa, oo+1);
		ro = getSelecticsa(g->myicsa, oo+2)-1;

	#endif
	
	res[0]=0;

	if (numocc) {
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		
		
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		ulong i;
		for (i=l; i<=r;i++) {
			uint o,s; 
			// objects
			#ifdef BUFFER_PSI_ON
				o=bufferpsi[i-l];
			#else
				o= getPsiicsa(g->myicsa, i);
			#endif		

			if ((o>=lo) && (o <=ro)) {
				// subjects
				s= getPsiicsa(g->myicsa, o);
				uint n = res[0];
				res[0]++;
				// predicates
				res_p[n] = P;
				// objects
				res_o[n] = O;
				// subjects
				//res_s[n] = s;
				res_s[n] = getRankicsa(g->myicsa,s) -1;
				res_s[n] = unmapID(g,res_s[n],SUBJECT);	
			}
		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	return res[0];
}



/******************************************************************************/
// 2 selects for the intervals + checking upon the P-interval
// the previous is very slow because the source interval of P's contains many many entries.
// This is why here we start traversing from O's interval... Yet, it is a bad choice!
/******************************************************************************/
int po_22 (void *gindex, int P, int O, uint **buffer) { 

// quizas valga a pena facer un PO() que busque binariamente O, P, (igual que o de abaixo), pero que en lugar de recorrer os P's e checkear (son moitos)
// comece polos O's e aplicando PsiPsi() mire se chegamos aos P's que interesan.
//
// ../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// // indexes/dbpedia16	dbpedia.queries/Vpo.txt	2894789	496	10952	5836.268145	264.316015
//		printf("\n@@@@@@@@@@@@@@@@ call to po (%d,%d)\n",P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong lp,rp, numocc_p;	
	ulong lo,ro, numocc_o;
	
	#ifndef USE_SELECT
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);

		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		
	#else
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp= getSelecticsa(g->myicsa, pp+2)-1;
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;

		numocc_p = rp-lp+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		lo= getSelecticsa(g->myicsa, oo+1);
		ro= getSelecticsa(g->myicsa, oo+2)-1;
		numocc_o = ro-lo+1;

	#endif
		
	res[0]=0;
	
	if (numocc_p && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=lo;r=ro;

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;
		for (i=l; i<=r;i++) {
			uint s,p; 
			// subjects
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			p= getPsiicsa(g->myicsa, s);

			if ((p>=lp) && (p <=rp)) {
				uint n = res[0];
				res[0]++;
				
				// subjects
				//res_s[n] = s;
				res_s[n] = getRankicsa(g->myicsa,s) -1;
				res_s[n] = unmapID(g,res_s[n],SUBJECT);	
				// predicates
				res_p[n] = P;
				// objects
				res_o[n] = O;
			}
		}
		
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}
	


/******************************************************************************/
// 2 selects for the intervals + 1 binary search to check:
// (a)Ps into Os interval
// Created 2015/01/15 ... slower than the previous one?? on DBPEDIA	
/******************************************************************************/

//int po2(void *gindex, int P, int O, uint **buffer) { 
int po (void *gindex, int P, int O, uint **buffer) {
//
// ../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
//
//		printf("\n@@@@@@@@@@@@@@@@ call to po (%d,%d)\n",P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong lp,rp, numocc_p;	
	ulong lo,ro, numocc_o;
	
	#ifndef USE_SELECT
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);

		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		
	#else
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp= getSelecticsa(g->myicsa, pp+2)-1;
		
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;

		
		numocc_p = rp-lp+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

			//			pattern[0] = oo; countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		//lo= getSelecticsa(g->myicsa, oo+1);
		//ro= getSelecticsa(g->myicsa, oo+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;
		numocc_o = ro-lo+1;

	#endif
	
		
	res[0]=0;
	
	if (numocc_p && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=lp;r=rp;
			//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);	
			
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);
		//expSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);


		if (!numocc) return res[0];
	//	printf("\n BSearchP-O= ls,rs = [%lu, %lu]",l,r);

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;
		for (i=l; i<=r;i++) {
			uint n,o,s; 
			n = res[0];
			res[0]++;

			// predicates
			res_p[n] = P;
			
			// objects
			#ifdef BUFFER_PSI_ON
				o=bufferpsi[i-l];
			#else
				o= getPsiicsa(g->myicsa, i);
			#endif		
			res_o[n] = O;

			// subjects
			s= getPsiicsa(g->myicsa, o);
			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);
			//res_s[n] = S;
						
		}
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}


//int po2(void *gindex, int P, int O, uint **buffer) { 
int po_9 (void *gindex, int P, int O, uint **buffer) {
//
// ../benchmark indexes/dbpedia16 dbpedia.queries/Vpo.txt 3 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
//
//		printf("\n@@@@@@@@@@@@@@@@ call to po (%d,%d)\n",P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong lp,rp, numocc_p;	
	ulong lo,ro, numocc_o;
	
	#ifndef USE_SELECT
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);

		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		
	#else
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp= getSelecticsa(g->myicsa, pp+2)-1;
		
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;

		
		numocc_p = rp-lp+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

//			pattern[0] = oo; countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		lo= getSelecticsa(g->myicsa, oo+1);
		ro= getSelecticsa(g->myicsa, oo+2)-1;
		numocc_o = ro-lo+1;

	#endif
	
		
	res[0]=0;
	
	if (numocc_p && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=lp;r=rp;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);		
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);

		if (!numocc) return res[0];
	//	printf("\n BSearchP-O= ls,rs = [%lu, %lu]",l,r);

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;
		for (i=l; i<=r;i++) {
			uint n,o,s; 
			n = res[0];
			res[0]++;

			// predicates
			res_p[n] = P;
			
			// objects
			#ifdef BUFFER_PSI_ON
				o=bufferpsi[i-l];
			#else
				o= getPsiicsa(g->myicsa, i);
			#endif		
			res_o[n] = O;

			// subjects
			s= getPsiicsa(g->myicsa, o);
			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);
			//res_s[n] = S;
						
		}
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}










/************************************************************************/
//  TESTING *************************************************************

int gr_graphsuffixCmp_POS(const void *arg1, const void *arg2) {

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


void gr_sortRecords_SPO_into_POS(uint *s, size_t n, uint nEntries) {
	//graph->nEntries;
	//graph->n;
	qsort(s , n , sizeof(uint)*nEntries, gr_graphsuffixCmp_POS);
}

void gr_shiftRecords_SPO_into_POS(uint *buff, size_t n, uint nEntries) {
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



//Computes the range [left,right] where triple <P,O> is found.
//Returns the size of the range (right-left+1), or 0 if <P,O> is not found.

uint dual_rdfcsaSPO_init_po (void *gindex, int P, int O, uint *left, uint *right) {
	
	twcsa *g = (twcsa *)gindex;

	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong lp,rp, numocc_p;	
	ulong lo,ro, numocc_o;
	
	#ifndef USE_SELECT
		uint pattern[MAX_ENTRIES];	
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);

		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		
	#else
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp= getSelecticsa(g->myicsa, pp+2)-1;
		
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
		numocc_p = rp-lp+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

			//			pattern[0] = oo; countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		//lo= getSelecticsa(g->myicsa, oo+1);
		//ro= getSelecticsa(g->myicsa, oo+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;
		numocc_o = ro-lo+1;

	#endif
	
		
	if (numocc_p && numocc_o) {	
		
		l=lp;r=rp;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);

		if (!numocc) return 0;

		*left = l;     //within subjects
		*right= r;
		return r-l+1;  /* number of matching triples */
	}
	
	return 0;
}




/*************  TEST PO*****************************************************************/
int test_po(void *gindex, uint **res, uint **res2) {
	printf("\n call to test_po\n");
	twcsa *g = (twcsa *)gindex;

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
	
	gr_sortRecords_SPO_into_POS(buffer , size_buffer/g->nEntries, g->nEntries);
	printf("\n sort records passed");fflush(stdout);
	//now triples are sorted by P,O,S
	gr_shiftRecords_SPO_into_POS(buffer, size_buffer/g->nEntries, g->nEntries);
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
			
			if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[j+2],B[i],B[i+1]);
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

	/***************/{
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o> queries over the existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = po(gindex, B[i*3],B[i*3+1], res);
		results1 = po_0(gindex, B[i*3],B[i*3+1], res2);
		
		
		if ((results != f[i]) || (results != results1)) { 
			printf("\n po operation failed (i=%lu)\n", i);
			printf("\n<\?,p,o>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		

			uint *res2_s = (*res2)+1;
			uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
			uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		

			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_p[x] != B[i*3])    ||  (res_o[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,o> = <\?,%u,%u> and should be  <\?,%u,%u>\n", 
				res_p[x], res_o[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <\?po> passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/


	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o>  queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		B[i*3] += ((B[i*3]> (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		

		results = po(gindex, B[i*3],B[i*3+1], res);
		results2 = po_0(gindex, B[i*3],B[i*3+1], res2);
				
		if (results != results2) { 
			printf("\n po operation failed (i=%lu)\n", i);
			printf("\n<\?,p,o>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		

			uint *res2_s = (*res2)+1;
			uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
			uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		

			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_p[x] != B[i*3])    ||  (res_o[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,o> = <\?,%u,%u> and should be  <\?,%u,%u>\n", 
				res_p[x], res_o[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}		
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <\?po> (rand) passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	
	
	return 0;
}






















































///*************************************************************************************************/
///*   DUAL - RDFCSA - 																			 */
///*************************************************************************************************/

// uses backward search, given 2 ranges obtained with select
/* receives a rdfcsa in SPO order as 1st parameter */
int dual_rdfcsaSPO_po (void *gindex, int P,  int O, unsigned int **buffer) {

	return po(gindex, P,O,buffer);
	
}


// uses backward search, given 2 ranges obtained with select
/* receives a rdfcsa in OPS order as 1st parameter */
int dual_rdfcsaOPS_op (void *gindex, int O, int P, unsigned int **buffer) {

	twcsa *g = (twcsa *)gindex;
	
	uint *res= *buffer; //no malloc performed here!!

	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaOPS_op (O,P)= (%d,\?,%d)\n",O,P);fflush (stdout);


	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong lp,rp, numocc_p;	
	ulong lo,ro, numocc_o;
	
	#ifndef USE_SELECT
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);

		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
		
	#else
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp= getSelecticsa(g->myicsa, pp+2)-1;
		
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;	
		numocc_p = rp-lp+1;
		

		{	// PARA ELIMINAR TENER AQUÍ SÓLO PARA TESTEAR !!
			ulong lp2,rp2, numocc_p2;

				lp2 = getSelecticsa(g->myicsa, pp+1);
				rp2 = getSelecticsa(g->myicsa, pp+2)-1; 
				//geticsa_select_j_y_j_mas_1 (g->myicsa, pp+1, &lp2, &rp2);	rp2--;

				if ((lp2 != lp) || (rp2 !=rp)) {
					printf("\n error en getSelectTablePredicates for P=%d",P);
					printf("\n tabulated-select: [lp , rp ] = [%lu, %lu]",lp,rp);
					printf("\n computed-Bselect: [lp2, rp2] = [%lu, %lu]",lp2,rp2);
					exit(0);
				}
			//FIN PARA ELIMINAR TENER AQUI SÓLO PARA TESTEAR 
		}		

		//lo= getSelecticsa(g->myicsa, oo+1);
		//ro= getSelecticsa(g->myicsa, oo+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;
		numocc_o = ro-lo+1;

	#endif
	
	res[0]=0;
	
	if (numocc_p && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=lo;r=ro;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);		
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lp, rp);

		if (!numocc) return res[0];
	//	printf("\n BSearchP-O= ls,rs = [%lu, %lu]",l,r);

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;
		for (i=l; i<=r;i++) {
			uint n,p,s; 
			n = res[0];
			res[0]++;

			// objects
			res_o[n] = O;
			
			// predicates
			#ifdef BUFFER_PSI_ON
				p=bufferpsi[i-l];
			#else
				p= getPsiicsa(g->myicsa, i);
			#endif		
			res_p[n] = P;

			// subjects
			s= getPsiicsa(g->myicsa, p);
			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);
			//res_s[n] = S;
						
		}
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}






/*************  TEST PO*****************************************************/
//1st parameter is a pointer to a tdualrdfcsa
//typedef struct {
//	twcsa *spo;
//	twcsa *ops;
//} tdualrdfcsa;


int dual_test_po_op(void *index, uint **res, uint **res2) {
	printf("\n call to dual_test_po_op\n");

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
	
	gr_sortRecords_SPO_into_POS(buffer , size_buffer/g->nEntries, g->nEntries);
	printf("\n sort records passed");fflush(stdout);
	//now triples are sorted by P,O,S
	gr_shiftRecords_SPO_into_POS(buffer, size_buffer/g->nEntries, g->nEntries);
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






 
 	/***************/	
 	{
 	srand(time(NULL));
 start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,o> (dual_rdfcsaSPO_po() && dual_rdfcsaOPS_op()) queries over existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	i=0;
	printf("\n\t <*,%u,%u> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;	
	for (i=1; i< n ; i++) {
		//results = po(gindex, B[i*3],B[i*3+1], res);
		//results1 = po_0(gindex, B[i*3],B[i*3+1], res2); 		
 		
 		results  = dual_rdfcsaSPO_po(g , B[i*3],B[i*3+1], res);   
 		results1 = dual_rdfcsaOPS_op(g2, B[i*3+1],B[i*3], res2);
 		
 		//SORT RESULTS1 in so order here not needed, both objects and subjects are sorted within those ranges!!
 	
		if ((results != f[i]) || (results != results1)) { 
			printf("\n po operation failed (i=%lu)\n", i);
			printf("\n<\?,p,o>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		

			uint *res2_s = (*res2)+1;
			uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
			uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		

			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_p[x] != B[i*3])    ||  (res_o[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,o> = <\?,%u,%u> and should be  <\?,%u,%u>\n", 
				res_p[x], res_o[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
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
	//now perform spo queries.
	printf("\n now performing <\?,p,o> (dual_rdfcsaSPO_po() && dual_rdfcsaOPS_op()) queries over rather unexisting triplets:\n");
	int results, results1 ;	
	long total_results=0;

	//skips i=0;		
	for (i=1; i< n ; i++) {
		B[i*3]   += ((B[i*3]  > (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		if (B[i*3+2] >= g->gaps[3]) B[i*3+2] = B[i*3+2] >= g->gaps[3] -1;

		//results = po(gindex, B[i*3],B[i*3+1], res);
		//results1 = po_0(gindex, B[i*3],B[i*3+1], res2); 		
 		
 		results  = dual_rdfcsaSPO_po(g , B[i*3],B[i*3+1], res);   
 		results1 = dual_rdfcsaOPS_op(g2, B[i*3+1],B[i*3], res2);
 		
 		//SORT RESULTS1 in so order here not needed, both objects and subjects are sorted within those ranges!!


		if (results != results1) { 
			printf("\n po operation failed (i=%lu)\n", i);
			printf("\n<\?,p,o>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		

			uint *res2_s = (*res2)+1;
			uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
			uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		

			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_p[x] != B[i*3])    ||  (res_o[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,o> = <\?,%u,%u> and should be  <\?,%u,%u>\n", 
				res_p[x], res_o[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <\?po> (rand)passed **ok** (total results = %lu) \n", total_results);

end = getTime2();	
	printf("\n time: %.3f secs\n\n", end-start );
	
	}
	/***************/	

	
	return 0;
}






