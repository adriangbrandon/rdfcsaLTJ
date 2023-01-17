



/*************************************************************************/

int so_0 (void *gindex, int S, int O, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to so (%d,%d)\n",S,O);fflush (stdout);
 //../benchmark indexes/dbpedia16 dbpedia.queries/sVo.txt 40 100
 // <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 // indexes/dbpedia16	dbpedia.queries/sVo.txt	1504	500	556	3.008000	2.705036
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = oo; pattern[1] = ss;
	ulong numocc,l,r;
	countIntIndex(g->myicsa, pattern , 2, &numocc, &l, &r);
	
	res[0]=0;

	if (numocc) {
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;


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

			// subjects
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			res_s[n] = S;

			// predicates
			p= getPsiicsa(g->myicsa, s);
			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);		
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}	

int so_1 (void *gindex, int S, int O, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to so (%d,%d)\n",S,O);fflush (stdout);
//../benchmark indexes/dbpedia16 dbpedia.queries/sVo.txt 41 100
 // <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 // indexes/dbpedia16	dbpedia.queries/sVo.txt	1715	500	556	3.430000	3.084532

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = oo; pattern[1] = ss;
	ulong numocc,l,r;
	//countIntIndex(g->myicsa, pattern , 2, &numocc, &l, &r);
	countIntIndex_Buffer (g->myicsa, pattern , 2, &numocc, &l, &r);
	
	res[0]=0;

	if (numocc) {
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;


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

			// subjects
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			res_s[n] = S;

			// predicates
			p= getPsiicsa(g->myicsa, s);
			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);		
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}	

/******************************************************************************/
// 2 selects for the intervals + checking upon the O-interval
// fastests than the original version
/******************************************************************************/
int so (void *gindex, int S, int O, uint **buffer) {
//	printf("\n@@@@@@@@@@@@@@@@ call to so (%d,%d)\n",S,O);fflush (stdout);
	
 //../benchmark indexes/dbpedia16 dbpedia.queries/sVo.txt 4 100
 // <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 //indexes/dbpedia16	dbpedia.queries/sVo.txt	922	500	556	1.844000	1.658273
 
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	ulong numoccs,ls,rs;
	
	#ifndef USE_SELECT
		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = ss;		
		countIntIndex(g->myicsa, pattern , 1, &numoccs, &ls, &rs);
		
	#else
		//l= getSelecticsa(g->myicsa, oo+1);
		//r = getSelecticsa(g->myicsa, oo+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &l, &r);	r--;
		numocc = r-l+1;

		
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		//ls= getSelecticsa(g->myicsa, ss+1);
		//rs = getSelecticsa(g->myicsa, ss+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		
		numoccs = rs-ls+1;

	#endif
	
	res[0]=0;

	if (numocc && numoccs) {
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		
		
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		ulong i;
		for (i=l; i<=r;i++) {
			uint s,p; 
			// subjects
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			if ((s>=ls) && (s <=rs)) {
				uint n = res[0];
				res[0]++;
				// objects
				res_o[n] = O;
				// subjects
				res_s[n] = S;
				// predicates
				//res_p[n] = p;		
				p= getPsiicsa(g->myicsa, s);
				res_p[n] = getRankicsa(g->myicsa,p) -1;
				res_p[n] = unmapID(g,res_p[n],PREDICATE);				
			}
		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	return res[0];	
}




/******************************************************************************/
// 2 selects for the intervals + checking upon the O-interval
// fastests than the original version
/******************************************************************************/
int so_9 (void *gindex, int S, int O, uint **buffer) {
//	printf("\n@@@@@@@@@@@@@@@@ call to so (%d,%d)\n",S,O);fflush (stdout);
	
 //../benchmark indexes/dbpedia16 dbpedia.queries/sVo.txt 4 100
 // <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 //indexes/dbpedia16	dbpedia.queries/sVo.txt	922	500	556	1.844000	1.658273
 
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	ulong numoccs,ls,rs;
	
	#ifndef USE_SELECT
		pattern[0] = oo;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = ss;		
		countIntIndex(g->myicsa, pattern , 1, &numoccs, &ls, &rs);
		
	#else
		l= getSelecticsa(g->myicsa, oo+1);
		r = getSelecticsa(g->myicsa, oo+2)-1;
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ls= getSelecticsa(g->myicsa, ss+1);
		rs = getSelecticsa(g->myicsa, ss+2)-1;
		numoccs = rs-ls+1;

	#endif
	
	res[0]=0;

	if (numocc && numoccs) {
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		
		
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		ulong i;
		for (i=l; i<=r;i++) {
			uint s,p; 
			// subjects
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			if ((s>=ls) && (s <=rs)) {
				uint n = res[0];
				res[0]++;
				// objects
				res_o[n] = O;
				// subjects
				res_s[n] = S;
				// predicates
				//res_p[n] = p;		
				p= getPsiicsa(g->myicsa, s);
				res_p[n] = getRankicsa(g->myicsa,p) -1;
				res_p[n] = unmapID(g,res_p[n],PREDICATE);				
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
// (a)Os into Ss interval
// Created 2015/01/15 ... slower than the previous one on DBPEDIA	
/******************************************************************************/

//int so_2select_1binSearchPSI_tocheck_psi(void *gindex, int S, int O, uint **buffer) {
int  so_2 (void *gindex, int S, int O, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to so (%d,%d)\n",S,O);fflush (stdout);
//../benchmark indexes/dbpedia16 dbpedia.queries/sVo.txt 42 100
 // <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 // indexes/dbpedia16	dbpedia.queries/sVo.txt	989	500	556	1.978000	1.778777
 
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lo,ro, numocc_o;
	
	#ifndef USE_SELECT
		pattern[0] = oo;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);

		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &ls, &rs);		
	#else
	
		lo= getSelecticsa(g->myicsa, oo+1);
		ro = getSelecticsa(g->myicsa, oo+2)-1; 
		numocc_o = ro-lo+1;

		ls= getSelecticsa(g->myicsa, ss+1);
		rs = getSelecticsa(g->myicsa, ss+2)-1;
		numocc_s = rs-ls+1;
	#endif
		
	res[0]=0;
	
	if (numocc_s && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=lo;r=ro;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, ls, rs);		
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, ls, rs);

		if (!numocc) return res[0];
	//	printf("\n BSearchS-P= ls,rs = [%lu, %lu]",l,r);

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;   //Os that map into the Ss' interval
		for (i=l; i<=r;i++) {
			uint n,s,p; 
			n = res[0];
			res[0]++;

			// objects
			res_o[n] = O;

			// subjects
			res_s[n] = S;

			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			// predicates
			//res_p[n] = P;
			p= getPsiicsa(g->myicsa, s);
			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);
						
		}
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}








































/************************************************************************/
//  TESTING *************************************************************

int gr_graphsuffixCmp_SOP(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	
	if ( *(a) < *(b)) return -1;
	else if ( *(a) > *(b)) return  +1;


	if ( *(a+2) < *(b+2)) return -1;
	else if ( *(a+2) > *(b+2)) return +1;

	if ( *(a+1) < *(b+1)) return -1;
	else if ( *(a+1) > *(b+1)) return +1;

	
	//for (i=0; i<gr_nentriesGicsa;i++) {
	//	if (*a <*b) return -1;
	//	if (*a >*b) return +1;
	//	a++;b++;
	//}
	
	return 0;
}

void gr_sortRecords_SPO_into_SOP(uint *s, size_t n, uint nEntries) {
	//graph->nEntries;
	//graph->n;
	qsort(s , n , sizeof(uint)*nEntries, gr_graphsuffixCmp_SOP);
}

void gr_shiftRecords_SPO_into_SOP(uint *buff, size_t n, uint nEntries) {
	size_t i,j,z=0;
	uint s,p,o;
	for (i=0; i<n; i++){
		s=buff[i*nEntries  ];
		p=buff[i*nEntries+1];
		o=buff[i*nEntries+2];
		
		buff[i*nEntries  ] = s;
		buff[i*nEntries+1] = o;
		buff[i*nEntries+2] = p;
		
	}					

}



/*************  TEST PO*****************************************************************/
int test_so(void *gindex, uint **res, uint **res2) {
	printf("\n call to test_so\n");
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
	
	gr_sortRecords_SPO_into_SOP(buffer , size_buffer/g->nEntries, g->nEntries);
	printf("\n sort records passed");fflush(stdout);
	//now triples are sorted by S, O, P
	gr_shiftRecords_SPO_into_SOP(buffer, size_buffer/g->nEntries, g->nEntries);
	printf("\n shift records passed");fflush(stdout);	
	//now reordered internally accordingly
	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets so* and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) ) {
			j+=3;
			
			if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[i],B[j+2],B[i+1]);
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
	printf("\n source triplets = %zu, different triplets <s,\?,o> = %lu\n",size_buffer/3,n);

	/***************/{
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,\?,o> queries over the existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = so(gindex, B[i*3],B[i*3+1], res);
//		results1 = so_0(gindex, B[i*3],B[i*3+1], res2);
		results1 = so_2(gindex, B[i*3],B[i*3+1], res2);
		
		
		if ((results != f[i]) || (results != results1)) { 
			printf("\n so operation failed (i=%lu)\n", i);
			printf("\n<s,\?,o>= <%u, %u, %u>",B[i*3], B[i*3+2], B[i*3+1]);
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
			if ( (res_s[x] != B[i*3])    ||  (res_o[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,\?,o> = <%u,\?, %u> and should be  <%u,\?, %u>\n", 
				res_s[x], res_o[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <s\?o> passed **ok** (total results = %lu) \n", total_results);
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
		

		results = so(gindex, B[i*3],B[i*3+1], res);
		//results2 = so_0(gindex, B[i*3],B[i*3+1], res2);
		results2 = so_2(gindex, B[i*3],B[i*3+1], res2);
				
		if (results != results2) { 
			printf("\n so operation failed (i=%lu)\n", i);
			printf("\n<s,\?,o>= <%u, %u, %u>",B[i*3], B[i*3+2], B[i*3+1]);
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
			if ( (res_s[x] != B[i*3])    ||  (res_o[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,\?,o> = <%u,\?, %u> and should be  <%u,\?, %u>\n", 
				res_s[x], res_o[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <s\?o> (rand) passed **ok** (total results = %lu) \n", total_results);

	}
	/***************/	
	
	return 0;
}




