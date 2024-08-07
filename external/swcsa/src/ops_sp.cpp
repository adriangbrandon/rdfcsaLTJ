



/*************************************************************************/

int sp_0 (void *gindex, int S, int P, uint **buffer){ 
//int sp_orig (void *gindex, int S, int P, uint **buffer){ 
// ../benchmark indexes/dbpedia16 dbpedia.queries/spV.txt 2 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spV.txt	1820	500	666	3.640000	2.732733
//
	//	printf("\n@@@@@@@@@@@@@@@@ call to sp (%d,%d)\n",S,P);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	pattern[0] = ss; pattern[1] = pp;
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
			uint n,p,o; 
			n = res[0];
			res[0]++;

			// subjects
			res_s[n] = S;

			// predicates
			#ifdef BUFFER_PSI_ON
				p=bufferpsi[i-l];
			#else
				p= getPsiicsa(g->myicsa, i);
			#endif		

			res_p[n] = P;

			// objects
			o= getPsiicsa(g->myicsa, p);
			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);
						
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}	


int sp_1 (void *gindex, int S, int P, uint **buffer){ 
//int sp_orig_buffer (void *gindex, int S, int P, uint **buffer){ 
// ../benchmark indexes/dbpedia16 dbpedia.queries/spV.txt 2 100	
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spV.txt	1667	500	666	3.334000	2.503003
//
	//	printf("\n@@@@@@@@@@@@@@@@ call to sp (%d,%d)\n",S,P);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	pattern[0] = ss; pattern[1] = pp;
	ulong numocc,l,r;
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
			uint n,p,o; 
			n = res[0];
			res[0]++;

			// subjects
			res_s[n] = S;

			// predicates
			#ifdef BUFFER_PSI_ON
				p=bufferpsi[i-l];
			#else
				p= getPsiicsa(g->myicsa, i);
			#endif		

			res_p[n] = P;

			// objects
			o= getPsiicsa(g->myicsa, p);
			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);
						
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}	

/******************************************************************************/
// 2 selects for the intervals + checking upon the S-interval
// faster choice up to 2015/01/13
/******************************************************************************/

//int sp_fast_2s_and_check(void *gindex, int S, int P, unsigned int **buffer) {
	int sp (void *gindex, int S, int P, uint **buffer){ 

//
// ../benchmark indexes/dbpedia16 dbpedia.queries/spV.txt 2 100
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spV.txt	889	500	666	1.778000	1.334835
//
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = pp;
		ulong numoccp,lp,rp;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
		
	#else
		//l= getSelecticsa(g->myicsa, ss+1);
		//r = getSelecticsa(g->myicsa, ss+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &l, &r);	r--;		
		numocc = r-l+1;
		
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1;
		//indexes/dbpedia8	dbpedia.queries/spV.txt	663	500	666	1.326000	0.995495

		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
        // indexes/dbpedia8	dbpedia.queries/spV.txt	547	500	666	1.094000	0.821321


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
			uint p,o; 
			// predicates
			#ifdef BUFFER_PSI_ON
				p=bufferpsi[i-l];
			#else
				p= getPsiicsa(g->myicsa, i);
			#endif		

			if ((p>=lp) && (p <=rp)) {
				// objects
				o= getPsiicsa(g->myicsa, p);
				uint n = res[0];
				res[0]++;
				// subjects
				res_s[n] = S;
				// predicates
				res_p[n] = P;
				// objects
				//res_o[n] = o;		
				res_o[n] = getRankicsa(g->myicsa,o) -1;
				res_o[n] = unmapID(g,res_o[n],OBJECT);				
			}
		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	return res[0];
}



int sp_9 (void *gindex, int S, int P, uint **buffer){ 

//
// ../benchmark indexes/dbpedia16 dbpedia.queries/spV.txt 2 100
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spV.txt	889	500	666	1.778000	1.334835
//
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = pp;
		ulong numoccp,lp,rp;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
		
	#else
		l= getSelecticsa(g->myicsa, ss+1);
		r = getSelecticsa(g->myicsa, ss+2)-1;
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1;
		//indexes/dbpedia8	dbpedia.queries/spV.txt	663	500	666	1.326000	0.995495

		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
        // indexes/dbpedia8	dbpedia.queries/spV.txt	547	500	666	1.094000	0.821321


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
			uint p,o; 
			// predicates
			#ifdef BUFFER_PSI_ON
				p=bufferpsi[i-l];
			#else
				p= getPsiicsa(g->myicsa, i);
			#endif		

			if ((p>=lp) && (p <=rp)) {
				// objects
				o= getPsiicsa(g->myicsa, p);
				uint n = res[0];
				res[0]++;
				// subjects
				res_s[n] = S;
				// predicates
				res_p[n] = P;
				// objects
				//res_o[n] = o;		
				res_o[n] = getRankicsa(g->myicsa,o) -1;
				res_o[n] = unmapID(g,res_o[n],OBJECT);				
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
// (a)Ss into POs interval
// Created 2015/01/15 ... slower than the previous one on DBPEDIA	
/******************************************************************************/

//int sp_2select_1binSearchPSI_tocheck_psi(void *gindex, int S, int P, unsigned int **buffer) {
int sp_2(void *gindex, int S, int P, unsigned int **buffer) {	
	//	printf("\n@@@@@@@@@@@@@@@@ call to sp (%d,%d)\n",S,P);fflush (stdout);
// ../benchmark indexes/dbpedia16 dbpedia.queries/spV.txt 2 100
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 //indexes/dbpedia16	dbpedia.queries/spV.txt	951	500	666	1.902000	1.427928

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lp,rp, numocc_p;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &ls, &rs);

		pattern[0] = pp;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
		
	#else
	
		ls= getSelecticsa(g->myicsa, ss+1);
		rs = getSelecticsa(g->myicsa, ss+2)-1;
		numocc_s = rs-ls+1;

		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1; 
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;

		
		numocc_p = rp-lp+1;

	/*	
		printf("\n ls,rs = [%lu, %lu]",ls,rs);
		printf("\n lp,rp = [%lu, %lu]",lp,rp);
		printf("\n lenS= %lu, lenP=%lu",rs-ls+1, rp-lp+1);
	*/
	#endif
		
	res[0]=0;
	
	if (numocc_s && numocc_p) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=ls;r=rs;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lp, rp);		
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lp, rp);

		if (!numocc) return res[0];
	//	printf("\n BSearchS-P= ls,rs = [%lu, %lu]",l,r);

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;
		for (i=l; i<=r;i++) {
			uint n,p,o; 
			n = res[0];
			res[0]++;

			// subjects
			res_s[n] = S;

			// predicates
			#ifdef BUFFER_PSI_ON
				p=bufferpsi[i-l];
			#else
				p= getPsiicsa(g->myicsa, i);
			#endif		
			res_p[n] = P;

			// objects
			o= getPsiicsa(g->myicsa, p);
			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);
						
		}
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}




















/*************  TEST SP*****************************************************************/
int test_sp_old(void *gindex, uint **res) {
	printf("\n call to test_sp\n");
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
	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets sp* and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) ) {
			j+=3;
			
			if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[i],B[i+1],B[i+2]);
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

	/***************/{
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> queries over the existing triplets:\n");
	int results ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = sp(gindex, B[i*3],B[i*3+1], res);
		
		if (results != f[i]) { 
			printf("\n sp operation failed (i=%lu)\n", i);
			printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ) {
				printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
				res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);
				exit(0);
			}				
		}	
		total_results +=results;
		
		if(i%100000==0) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <sp\?> passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/


	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		B[i*3] += ((B[i*3]> (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		
		results2 = sp_0(gindex, B[i*3],B[i*3+1], res);
		results = sp(gindex, B[i*3],B[i*3+1], res);
		if (results != results2) { 
			printf("\n sp operation failed (i=%lu)\n", i);
			printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ) {
				printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
				res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);
				exit(0);
			}				
		}	
		total_results +=results;
		
		if(i%100000==0) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <sp\?> (rand) passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	
	
	return 0;
}







/*************  TEST SP*****************************************************************/
int test_sp(void *gindex, uint **res, uint **res2) {
	printf("\n call to test_sp\n");
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
	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets sp* and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) ) {
			j+=3;
			
			if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[j],B[i+1],B[i+2]);
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

	/***************/{
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> queries over the existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = sp(gindex, B[i*3],B[i*3+1], res);
		results1 = sp_0(gindex, B[i*3],B[i*3+1], res2);
		
		
		if ((results != f[i]) || (results != results1)) { 
			printf("\n sp operation failed (i=%lu)\n", i);
			printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
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
			if ( (res_s[x] != B[i*3])    ||  (res_p[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
				res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <sp\?> passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/


	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		B[i*3] += ((B[i*3]> (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		
		results2 = sp_0(gindex, B[i*3],B[i*3+1], res2);
		results = sp(gindex, B[i*3],B[i*3+1], res);
		if (results != results2) { 
			printf("\n sp operation failed (i=%lu)\n", i);
			printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
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
			if ( (res_s[x] != B[i*3])    ||  (res_p[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
				res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}			
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <sp\?> (rand) passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	
	
	return 0;
}
















































///*************************************************************************************************/
///*   DUAL - RDFCSA - 																			 */
///*************************************************************************************************/

// uses backward search, given 2 ranges obtained with select
/* receives a rdfcsa in SPO order as 1st parameter */
int dual_rdfcsaSPO_sp (void *gindex, int S,  int P, unsigned int **buffer) {

	return sp_2(gindex, S,P,buffer);
	
}


// uses backward search, given 2 ranges obtained with select
/* receives a rdfcsa in OPS order as 1st parameter */
int dual_rdfcsaOPS_ps (void *gindex, int P, int S, unsigned int **buffer) {

	twcsa *g = (twcsa *)gindex;
	
	uint *res= *buffer; //no malloc performed here!!

	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaOPS_sp (S,P)= (%d,\?,%d)\n",S,O);fflush (stdout);


	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lp,rp, numocc_p;

	
	#ifndef USE_SELECT
		uint pattern[MAX_ENTRIES];
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &ls, &rs);

		pattern[0] = pp;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
	#else
		//ls= getSelecticsa(g->myicsa, ss+1);
		//rs = getSelecticsa(g->myicsa, ss+2)-1;
		//numocc_s = rs-ls+1;
	
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		
		numocc_s = rs-ls+1;
		
	
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
		numocc_p = rp-lp+1;
		

		{	// PARA ELIMINAR TENER AQUÍ SÓLO PARA TESTEAR !!
			ulong lp2,rp2, numocc_p2;

				lp2 = getSelecticsa(g->myicsa, pp+1);
				rp2 = getSelecticsa(g->myicsa, pp+2)-1; 
				if ((lp2 != lp) || (rp2 !=rp)) {
					printf("\n error en getSelectTablePredicates for P=%d",P);
					printf("\n tabulated-select: [lp , rp ] = [%lu, %lu]",lp,rp);
					printf("\n computed-Bselect: [lp2, rp2] = [%lu, %lu]",lp2,rp2);
					exit(0);
				}
			//FIN PARA ELIMINAR TENER AQUI SÓLO PARA TESTEAR 
		}		
		
		
		//		printf("\n*****\n \t l,r = [%lu, %lu]",l,r);
		//		printf("\n \t lp,rp = [%lu, %lu]",lp,rp);
		//		printf("\n \t ls,rs = [%lu, %lu]",ls,rs);
		//		printf("\n lenS= %lu, lenP=%lu, lenO=%lu",r-l+1, rp-lp+1,ro-lo+1);
		
	#endif
	
	res[0]=0;
	
	if (numocc_s && numocc_p) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		l=lp;r=rp;

		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, ls, rs);

		if (!numocc) return res[0];
	//	printf("\n BSearchS-P= ls,rs = [%lu, %lu]",l,r);

		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		

		ulong i;   //Ps that map into the Ss' interval
		for (i=l; i<=r;i++) {
			uint n,o,s; 
			n = res[0];
			res[0]++;

			// predicates
			res_p[n] = P;

			// subjects
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-l];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		
			res_s[n] = S;

			// objects
			o= getPsiicsa(g->myicsa, s);
			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);
	
						
		}
		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif		
	}
	
	return res[0];
}


//Computes the range [left,right] where triple <S,P> is found.
//Returns the size of the range (right-left+1), or 0 if <S,P> is not found.
uint dual_rdfcsaSPO_init_sp (void *gindex, int S, int P,  uint *left, uint *right) {
	
	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaSPO_init_sp (%d,%d,!?)\n",S,P);fflush (stdout);

	twcsa *g = (twcsa *)gindex;

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lp,rp, numocc_p;
	
	#ifndef USE_SELECT
		uint pattern[MAX_ENTRIES];
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &ls, &rs);

		pattern[0] = pp;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
		
	#else
	
		//ls= getSelecticsa(g->myicsa, ss+1);
		//rs = getSelecticsa(g->myicsa, ss+2)-1;
		//numocc_s = rs-ls+1;
	
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		
		numocc_s = rs-ls+1;		

		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1; 
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
		
		numocc_p = rp-lp+1;
	#endif
			
	if (numocc_s && numocc_p) {	

		l=ls;r=rs;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lp, rp);		
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lp, rp);

		if (!numocc) return 0;

		*left = l;     //within subjects
		*right= r;
		return r-l+1;  /* number of matching triples */
	}
	
	return 0;
}




/*************  TEST SP*****************************************************/
//1st parameter is a pointer to a tdualrdfcsa
//typedef struct {
//	twcsa *spo;
//	twcsa *ops;
//} tdualrdfcsa;


int dual_test_sp_ps(void *index, uint **res, uint **res2) {
	printf("\n call to dual_test_sp_ps\n");

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
	printf("\n now performing <s,p,\?> queries over the existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
//	i= 415000810;
//	for ( ; i< n ; i++) {

	i=0;
	printf("\n\t <%u,%u,*> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;
	for (i=1; i< n ; i++) {
		//results = sp(gindex, B[i*3],B[i*3+1], res);
		//results1 = sp_0(gindex, B[i*3],B[i*3+1], res2);
 		
 		results  = dual_rdfcsaSPO_sp(g , B[i*3],B[i*3+1], res);   
 		results1 = dual_rdfcsaOPS_ps(g2, B[i*3+1],B[i*3], res2);
 		
 		//SORT RESULTS1 in so order here not needed, both objects and subjects are sorted within those ranges!!
 	
		if ((results != f[i]) || (results != results1)) { 
			//printf("\n sp operation failed (i=%lu)\n", i);
			printf("\n sp operation failed (i=%lu) - results = %u, results1 = %u\n", i, results, results1);
			
			printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
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
			if ( (res_s[x] != B[i*3])    ||  (res_p[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
				res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
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
	//now perform spo queries.
	printf("\n now performing <s,p,\?> queries over rather unexisting triplets:\n");
	int results, results1 ;	
	long total_results=0;
	
		//skips i=0;
	for (i=1; i< n ; i++) {
		B[i*3]   += ((B[i*3]  > (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		if (B[i*3+2] >= g->gaps[3]) B[i*3+2] = B[i*3+2] >= g->gaps[3] -1;

		//results2 = sp_0(gindex, B[i*3],B[i*3+1], res2);
		//results = sp(gindex, B[i*3],B[i*3+1], res);
		
 		results  = dual_rdfcsaSPO_sp(g , B[i*3],B[i*3+1], res);   
 		results1 = dual_rdfcsaOPS_ps(g2, B[i*3+1],B[i*3], res2);
 		
 		//SORT RESULTS1 in so order here not needed, both objects and subjects are sorted within those ranges!!
 	
		if (results != results1) { 
			printf("\n sp operation failed (i=%lu) - results = %u, results1 = %u\n", i, results, results1);
			printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
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
			if ( (res_s[x] != B[i*3])    ||  (res_p[x] != B[i*3+1])  ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
				res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
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

	
	return 0;
}













int dual_test_sp_ps_TARGET_BIN_VS_FIRSTLEFT(void *index, uint **res, uint **res2) {
	printf("\n call to dual_test_sp_ps_TARGET_BIN_VS_FIRSTLEFT\n");


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
uint *V = NULL;
 	{
 	srand(time(NULL));
 start = getTime2();	
 
 
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,p,\?> queries over the existing triplets:\n");
	int results, results1 ;	

	i=0;
	printf("\n\t <%u,%u,*> skipped\n", B[i*3],B[i*3+1]); fflush(stdout);fflush(stderr);
	//skips i=0;
	
	V = (uint *) malloc (n * 4 * sizeof(uint));
		printf("\n now preparing ranges (using select) that will be searched for with bin/exp: \n");
		for (i=1; i< n ; i++) {
			
			uint S=B[i*3];
			uint P=B[i*3+1];

			uint ss = mapID(g,S,SUBJECT);
			uint pp = mapID(g,P,PREDICATE);
		
			ulong l,r, numocc;
			ulong ls,rs, numocc_s;
			ulong lp,rp, numocc_p;	
			
			geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		
			numocc_s = rs-ls+1;
			
		
			lp= getSelectTablePredicates(g->myicsa,pp+1);
			rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
			numocc_p = rp-lp+1;
			
			V[i*4+0]=ls;  V[i*4+1]=rs;
			V[i*4+2]=lp;  V[i*4+3]=rp;
		}	
	}
	
	{
 start = getTime2();	
	
		printf("\n now performing BOTH binSearchPsiTarget_samplesFirst() and \n\t expSearchPsiTarget_leftOnly_samplesFirst() over existing triplets:\n");
		for (i=1; i< n ; i++) {

			ulong l,r, numocc;
			ulong ls,rs, numocc_s;
			ulong lp,rp, numocc_p;	
					
			ls = V[i*4+0]; rs = V[i*4+1];
			lp = V[i*4+2]; rp = V[i*4+3];
			
	//		/////////// TEST binSearchPsiTarget_samplesFirst /////////////////////
			l=ls;r=rs;
			binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lp, rp);   //we only need l actually
					
	//		/////////// TEST expSearchPsiTarget_leftOnly_samplesFirst /////////////////////
			 ulong lll=ls, rrr=rs;
			  ulong numocc2;
				int xx= expSearchPsiTarget_leftOnly_samplesFirst(g->myicsa, &lll,&rrr, &numocc2, lp, rp);
	//		///////////////////
			
				if (numocc2>0) {
					if (numocc == 0 ) {
					//if (numocc2 > numocc ){   //now numocc2 is a position (a value of psi, so it could be larger than numocc)
						printf("\n expSearch_leftOnly fails: numocc= %lu, numocc2=%lu  (retval=%d)\n", numocc,numocc2,xx);
					}
					else {
						if (lll == l)
							{ //printf("\r expSearch_leftOnly OK: (occs = %lu) (retval=%d)",numocc,xx);
							}
						else {
							printf("\n expSearch_leftOnly fails: l= %lu, lll=%lu (retval=%d)\n", l, lll,xx); 
						}
					}
				}
				else {				
					if(numocc>0) {
						printf("\n expSearch_leftOnly fails: numocc= %lu, numocc2=%lu  (retval=%d)\n", numocc,numocc2,xx);
					}
					//else numocc=numocc2=0
				}
			////////////////
						
			if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
		}	
		
		fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
		printf("\n TEST <sp -bin-exp-search\?> passed **ok** \n");
	 
	 end = getTime2();	
		printf("\n total time: %.3f secs\n\n", end-start );
	}


 	{
start = getTime2();	
		printf("\n\n\n now performing binSearchPsiTarget_samplesFirst() ONLY over existing triplets:\n");
		for (i=1; i< n ; i++) {

			ulong l,r, numocc;
			ulong ls,rs, numocc_s;
			ulong lp,rp, numocc_p;	
					
			ls = V[i*4+0]; rs = V[i*4+1];
			lp = V[i*4+2]; rp = V[i*4+3];
			
	//		/////////// TEST binSearchPsiTarget_samplesFirst /////////////////////
			l=ls;r=rs;
			binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lp, rp);   //we only need l actually
					
	//		/////////// TEST expSearchPsiTarget_leftOnly_samplesFirst /////////////////////
	//		 ulong lll=ls, rrr=rs;
	//		  ulong numocc2;
	//			int xx= expSearchPsiTarget_leftOnly_samplesFirst(g->myicsa, &lll,&rrr, &numocc2, lp, rp);
	//		///////////////////
			
						
			if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
		}	
		
		fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	 
	 end = getTime2();	
		printf("\n total time (binSearchPsiTarget_samplesFirst()): %.3f secs\n\n", end-start );
	}


	{
 start = getTime2();	
	
		printf("\n\n\n now performing expSearchPsiTarget_leftOnly_samplesFirst() ONLY over existing triplets:\n");
		for (i=1; i< n ; i++) {

			ulong l,r, numocc;
			ulong ls,rs, numocc_s;
			ulong lp,rp, numocc_p;	
					
			ls = V[i*4+0]; rs = V[i*4+1];
			lp = V[i*4+2]; rp = V[i*4+3];
			
	//		/////////// TEST binSearchPsiTarget_samplesFirst /////////////////////
	//		l=ls;r=rs;
	//		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lp, rp);   //we only need l actually
					
	//		/////////// TEST expSearchPsiTarget_leftOnly_samplesFirst /////////////////////
			 ulong lll=ls, rrr=rs;
			  ulong numocc2;
				int xx= expSearchPsiTarget_leftOnly_samplesFirst(g->myicsa, &lll,&rrr, &numocc2, lp, rp);
	//		///////////////////
			
						
			if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
		}	
		
		fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	 
	 end = getTime2();	
		printf("\n total time (expSearchPsiTarget_leftOnly_samplesFirst()): %.3f secs\n\n", end-start );
	}
printf("\n done!!\n");

if (V) free(V);

 	/***************/		
 

//		/***************/
//		{
//		srand(time(NULL));
//	start = getTime2();	
//	
//		fflush(stdout);fflush(stderr);
//		//now perform spo queries.
//		printf("\n now performing <s,p,\?> queries over rather unexisting triplets:\n");
//		int results, results1 ;	
//		long total_results=0;
//		
//			//skips i=0;
//		for (i=1; i< n ; i++) {
//			B[i*3]   += ((B[i*3]  > (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
//			B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
//			B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
//			if (B[i*3+2] >= g->gaps[3]) B[i*3+2] = B[i*3+2] >= g->gaps[3] -1;
//	
//			//results2 = sp_0(gindex, B[i*3],B[i*3+1], res2);
//			//results = sp(gindex, B[i*3],B[i*3+1], res);
//			
//	 		results  = dual_rdfcsaSPO_sp(g , B[i*3],B[i*3+1], res);   
//	 		results1 = dual_rdfcsaOPS_ps(g2, B[i*3+1],B[i*3], res2);
//	 		
//	 		//SORT RESULTS1 in so order here not needed, both objects and subjects are sorted within those ranges!!
//	 	
//			if (results != results1) { 
//				printf("\n sp operation failed (i=%lu) - results = %u, results1 = %u\n", i, results, results1);
//				printf("\n<s,p,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
//				exit(0);
//			}
//			if (results > 0) {
//				uint *res_s = (*res)+1;
//				uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
//				uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
//	
//				uint *res2_s = (*res2)+1;
//				uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
//				uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		
//	
//				//checks results are "expected"
//				int x;
//				for (x=0; x<results;x++)
//				if ( (res_s[x] != B[i*3])    ||  (res_p[x] != B[i*3+1])  ||
//					 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
//				   ) {
//					printf("\n\n Retrieved triplet failed: <s,p,\?> = <%u,%u,\?> and should be  <%u,%u,\?>\n", 
//					res_s[x], res_p[x],  B[i*3],B[i*3+1] );fflush (stdout);
//	
//					printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
//					res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);
//	
//					exit(0);
//				}				
//			}	
//			total_results +=results;
//			
//			if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
//	 	}	
//	 	
//	 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
//		printf("\n TEST <sp\?> passed **ok** (total results = %lu) \n", total_results);
//	 
//	 end = getTime2();	
//	 	printf("\n time: %.3f secs\n\n", end-start );
//	 	}
//	 	/***************/	

	
	return 0;
}













