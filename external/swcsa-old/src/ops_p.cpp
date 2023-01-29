
int p_0  (void *gindex, int P, uint **buffer){
	//	printf("\n@@@@@@@@@@@@@@@@ call to p (%d)\n",P);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);
	
	pattern[0] = pp;
	ulong numocc,l,r;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);
	
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
			uint n,s,o; 
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

			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);

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
	
int p  (void *gindex, int P, uint **buffer){
	//	printf("\n@@@@@@@@@@@@@@@@ call to p (%d)\n",P);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint pp = mapID(g,P,PREDICATE);	
	
	ulong numocc,l,r;	
	
	#ifndef USE_SELECT
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);		
	#else
		//l= getSelecticsa(g->myicsa, pp+1);
		//r = getSelecticsa(g->myicsa, pp+2)-1;
		
		l= getSelectTablePredicates(g->myicsa,pp+1);
		r= getSelectTablePredicates(g->myicsa,pp+2)-1;

		
		numocc = r-l+1;
	#endif
	
	
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
			uint n,s,o; 
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

			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);

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

int p_9  (void *gindex, int P, uint **buffer){
	return p(gindex,P,buffer);
}


/*************  TEST P *****************************************************************/

int test_p(void *gindex, uint **res, uint **res2) {
	printf("\n call to test_p\n");
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

	printf("\n now computing different triplets p\?\? and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]))  {
			j+=3;
			
			if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[j+2],B[j],B[j+1]);
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
	printf("\n source triplets = %zu, different triplets <\?,p,\?> = %lu\n",size_buffer/3,n);

	/***************/{
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,\?> queries over the existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = p(gindex, B[i*3], res);
		results1 = p_0(gindex, B[i*3], res2);
		
		
		if ((results != f[i]) || (results != results1)) { 
			printf("\n p operation failed (i=%lu)\n", i);
			printf("\n results=%d, f[i]=%d, results1= %d\n", results, f[i],results1);
			printf("\n<\?,p,\?>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
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
			if ( (res_p[x] != B[i*3])    ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,\?> = <\?,%u,\?> and should be  <\?,%u,\?>\n", 
				res_p[x],  B[i*3]);fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <\?p\?> passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/


	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <\?,p,\?>  queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		B[i*3] += ((B[i*3]> (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		

		results = p(gindex, B[i*3], res);
		results2 = p_0(gindex, B[i*3], res2);
				
		if (results != results2) { 
			printf("\n p operation failed (i=%lu)\n", i);
			printf("\n<\?,p,\?>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
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
			if ( (res_p[x] != B[i*3])    ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,\?> = <\?,%u,\?> and should be  <\?,%u,\?>\n", 
				res_p[x],  B[i*3]);fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <\?p\?> (rand) passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	
	
	return 0;
}

















































































///*************************************************************************************************/
///*   DUAL - RDFCSA - 																			 */
///*************************************************************************************************/

// uses 1 range obtained with select and applies psi twice
/* receives a rdfcsa in SPO order as 1st parameter */
int dual_rdfcsaSPO_p (void *gindex, int P, unsigned int **buffer) {

	return p(gindex, P,buffer);
	
}


// uses 1 range obtained with select and applies psi twice
/* receives a rdfcsa in OPS order as 1st parameter */
int dual_rdfcsaOPS_p (void *gindex, int P, unsigned int **buffer) {

	twcsa *g = (twcsa *)gindex;
	
	uint *res= *buffer; //no malloc performed here!!

	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaOPS_p (P)= (\?,%d,\?)\n",P);fflush (stdout);

	uint pp = mapID(g,P,PREDICATE);	
	
	ulong numocc,l,r;

	
	#ifndef USE_SELECT
		uint pattern[MAX_ENTRIES];
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);		
	#else
		//l= getSelecticsa(g->myicsa, pp+1);
		//r = getSelecticsa(g->myicsa, pp+2)-1;
		//geticsa_select_j_y_j_mas_1 (g->myicsa, pp+1, &l, &r);	r--;		
		
		l= getSelectTablePredicates(g->myicsa,pp+1);
		r= getSelectTablePredicates(g->myicsa,pp+2)-1;
		numocc = r-l+1;
		
	{	// PARA ELIMINAR TENER AQUÍ SÓLO PARA TESTEAR !!
		ulong lp=l, rp=r;
		ulong lp2,rp2, numocc_p2;
			geticsa_select_j_y_j_mas_1 (g->myicsa, pp+1, &lp2, &rp2);	rp2--;
			//			lp2 = getSelecticsa(g->myicsa, pp+1);
			//			rp2 = getSelecticsa(g->myicsa, pp+2)-1; 
			if ((lp2 != lp) || (rp2 !=rp)) {
				printf("\n error en getSelectTablePredicates for P=%d",P);
				printf("\n tabulated-select: [lp , rp ] = [%lu, %lu]",lp,rp);
				printf("\n computed-Bselect: [lp2, rp2] = [%lu, %lu]",lp2,rp2);
				exit(0);
			}
		//FIN PARA ELIMINAR TENER AQUI SÓLO PARA TESTEAR 
	}
		
	#endif
	
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
			uint n,s,o; 
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

			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);		


			// subjects
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







//Computes the range [left,right] where triple <?,P,?> is found.
//Returns the size of the range (right-left+1), or 0 if <S> is not found.
uint dual_rdfcsaSPO_init_p (void *gindex, int P,  uint *left, uint *right) {
	
	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaSPO_init_p (\?,%d,\?)\n",P);fflush (stdout);

	twcsa *g = (twcsa *)gindex;

	uint pp = mapID(g,P,PREDICATE);
	
	ulong numocc,l,r;

	#ifndef USE_SELECT
		uint pattern[MAX_ENTRIES];
		pattern[0] = pp;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);		
	#else
		//l= getSelecticsa(g->myicsa, pp+1);
		//r= getSelecticsa(g->myicsa, pp+2)-1;
		
		l= getSelectTablePredicates(g->myicsa,pp+1);
		r= getSelectTablePredicates(g->myicsa,pp+2)-1;
		numocc = r-l+1;				
	#endif
		
	if (!numocc) return 0;
	
	*left = l;     
	*right= r;
	return r-l+1;  /* number of matching triples */
}


//Computes the range [left,right] where triple <?,P,?> is found.
//Returns the size of the range (right-left+1), or 0 if <S> is not found.
uint dual_rdfcsaOPS_init_p (void *gindex, int P,  uint *left, uint *right) {
	
	return dual_rdfcsaSPO_init_p(gindex,P,left,right);
}





 //  TESTING s DUAL*************************************************************


void gr_resultsSort_opsP_SPO(uint *s, size_t n, uint nEntries) {
	array_nentriesGicsaSPO= nEntries;								   //see global variable in buildFacadeGraphOPS.c		
	qsort(s, n, sizeof(uint)* nEntries, array_graphsuffixCmpSPO);	   //see cmp function    in buildFacadeGraphOPS.c
}




/*************  TEST P*****************************************************/
//1st parameter is a pointer to a tdualrdfcsa
//typedef struct {
//	twcsa *spo;
//	twcsa *ops;
//} tdualrdfcsa;


int dual_test_p_p(void *index, uint **res, uint **res2) {
	printf("\n call to dual_test_p_p\n");

double start ,end;

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	twcsa *g = dualrdf->spo;
	twcsa *g2 = dualrdf->ops;
	

	uint *buffer;
	size_t size_buffer;
	printf("\n dumping source data \n");fflush(stdout);
		dumpSourceData(g, &buffer, &size_buffer);
	printf("... %zu triplets recovered (%zu expected)\n",size_buffer / 3, g->n);fflush(stdout);
	
	gr_sortRecords_SPO_into_POS(buffer , size_buffer/g->nEntries, g->nEntries);
	printf("\n sort records passed");fflush(stdout);
	//now triples are sorted by P,O,S
	gr_shiftRecords_SPO_into_POS(buffer, size_buffer/g->nEntries, g->nEntries);
	printf("\n shift records passed");fflush(stdout);	
	//now reordered internally accordingly
	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets \?p\? and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]))  {
			j+=3;
			
			if (!(j%1000000)) printf("\n<s,p,o>= %u, %u, %u", B[j+2],B[j],B[j+1]);
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
	printf("\n source triplets = %zu, different triplets <\?,p,\?> = %lu\n",size_buffer/3,n);






 
 	/***************/	
 	{
 	srand(time(NULL));
 start = getTime2();	
 
 
 	fflush(stdout);fflush(stderr);
 	//now perform spo queries.
 	printf("\n now performing <\?,p,\?> (dual_rdfcsaSPO_p() && dual_rdfcsaOPS_p()) queries over existing triplets:\n");
 	int results, results1 ;	
 	long total_results=0;

	i=0;
	printf("\n\t <*,%u,*> skipped\n", B[i*3]); fflush(stdout);fflush(stderr);
	//skips i=0;

 	for (i=1; i< n ; i++) {
		//results = p(gindex, B[i*3], res);
		//results1 = p_0(gindex, B[i*3], res2);
 		
 		results  = dual_rdfcsaSPO_p(g , B[i*3], res);   
 		results1 = dual_rdfcsaOPS_p(g2, B[i*3], res2);
 		
 	
 		if ((results != f[i]) || (results != results1)) { 
 			printf("\n p- operation failed (i=%lu)\n", i);
			printf("\n<\?,p,\?>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
 			exit(0);
 		}
 		if (results > 0) {
 			uint *res_s = (*res)+1;
 			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
 			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
 
 			uint *res2_s = (*res2)+1;
 			uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
 			uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		
 			
 
 			{	//sorts res1[] in SPO order
				uint * sortedRes = (uint *) malloc (sizeof(uint) * 3 * results);
				uint t,tt;
				tt=0;
				for (t=0; t<results;t++) {
					sortedRes[tt++] = res_s[t]; sortedRes[tt++] = res_p[t]; sortedRes[tt++] = res_o[t];
				}
				gr_resultsSort_opsS_SPO(sortedRes, t, 3);
				
				tt=0;
				for (t=0; t<results;t++) {
					res_s[t] = sortedRes[tt++]; res_p[t]=sortedRes[tt++];   res_o[t]=sortedRes[tt++];
				}
				free(sortedRes);
			}
 			{	//sorts res2[] in SPO order
				uint * sortedRes2 = (uint *) malloc (sizeof(uint) * 3 * results1);
				uint t,tt;
				tt=0;
				for (t=0; t<results1;t++) {
					sortedRes2[tt++] = res2_s[t]; sortedRes2[tt++] = res2_p[t]; sortedRes2[tt++] = res2_o[t];
				}
				gr_resultsSort_opsS_SPO(sortedRes2, t, 3);
				
				tt=0;
				for (t=0; t<results1;t++) {
					res2_s[t] = sortedRes2[tt++]; res2_p[t]=sortedRes2[tt++];   res2_o[t]=sortedRes2[tt++];
				}
				free(sortedRes2);
			}
 
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_p[x] != B[i*3])    ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,\?> = <\?,%u,\?> and should be  <\?,%u,\?>\n", 
				res_p[x],  B[i*3]);fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST \?p\?> passed **ok** (total results = %lu) \n", total_results);
 
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
 	printf("\n now performing <\?,p,\?> (dual_rdfcsaSPO_p() && dual_rdfcsaOPS_p()) queries over rather unexisting triplets:\n");
 	int results, results1 ;	
 	long total_results=0;
	
	//skips i=0;

	for (i=1; i< n ; i++) {
		B[i*3]   += ((B[i*3]  > (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		if (B[i*3+2] >= g->gaps[3]) B[i*3+2] = B[i*3+2] >= g->gaps[3] -1;

		//results = s(gindex, B[i*3], res);
		//results1 = s_0(gindex, B[i*3], res2); 		
 		
 		results  = dual_rdfcsaSPO_p(g , B[i*3], res);   
 		results1 = dual_rdfcsaOPS_p(g2, B[i*3], res2);


		if (results != results1) { 
 			printf("\n p- operation failed (i=%lu)\n", i);
			printf("\n<\?,p,\?>= <%u, %u, %u>",B[i*3+2], B[i*3],B[i*3+1]);
 			exit(0);
		}
		if (results > 0) {
 			uint *res_s = (*res)+1;
 			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
 			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
 
 			uint *res2_s = (*res2)+1;
 			uint *res2_p = (*res2)+1+ (MAX_RESULTS)*1;
 			uint *res2_o = (*res2)+1+ (MAX_RESULTS)*2;		


 			{	//sorts res1[] in SPO order
				uint * sortedRes = (uint *) malloc (sizeof(uint) * 3 * results);
				uint t,tt;
				tt=0;
				for (t=0; t<results;t++) {
					sortedRes[tt++] = res_s[t]; sortedRes[tt++] = res_p[t]; sortedRes[tt++] = res_o[t];
				}
				gr_resultsSort_opsS_SPO(sortedRes, t, 3);
				
				tt=0;
				for (t=0; t<results;t++) {
					res_s[t] = sortedRes[tt++]; res_p[t]=sortedRes[tt++];   res_o[t]=sortedRes[tt++];
				}
				free(sortedRes);
			}
 			{	//sorts res2[] in SPO order
				uint * sortedRes2 = (uint *) malloc (sizeof(uint) * 3 * results1);
				uint t,tt;
				tt=0;
				for (t=0; t<results1;t++) {
					sortedRes2[tt++] = res2_s[t]; sortedRes2[tt++] = res2_p[t]; sortedRes2[tt++] = res2_o[t];
				}
				gr_resultsSort_opsS_SPO(sortedRes2, t, 3);
				
				tt=0;
				for (t=0; t<results1;t++) {
					res2_s[t] = sortedRes2[tt++]; res2_p[t]=sortedRes2[tt++];   res2_o[t]=sortedRes2[tt++];
				}
				free(sortedRes2);
			}

 
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_p[x] != B[i*3])    ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <\?,p,\?> = <\?,%u,\?> and should be  <\?,%u,\?>\n", 
				res_p[x],  B[i*3]);fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST \?p\?> passed **ok** (total results = %lu) \n", total_results);
 
 end = getTime2();	
 	printf("\n time: %.3f secs\n\n", end-start );
	
	}
	/***************/	

	
	return 0;
}


