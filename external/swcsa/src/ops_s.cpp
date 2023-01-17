
/*************************************************************************/

int s_0  (void *gindex, int S, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to s (%d)\n",S);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	
	pattern[0] = ss;
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

			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);

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





/*************************************************************************/

int s  (void *gindex, int S, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to s (%d)\n",S);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	
	ulong numocc,l,r;	
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);		
	#else
		//l= getSelecticsa(g->myicsa, ss+1);
		//r = getSelecticsa(g->myicsa, ss+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &l, &r);	r--;		
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

			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);

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



/*************************************************************************/

int s_9  (void *gindex, int S, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to s (%d)\n",S);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	
	ulong numocc,l,r;	
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);		
	#else
		l= getSelecticsa(g->myicsa, ss+1);
		r = getSelecticsa(g->myicsa, ss+2)-1;
		//geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &l, &r);	r--;		
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

			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);

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









//********************************************************************************


/*************  TEST SPO*****************************************************/
int test_s(void *gindex, uint **res, uint **res2) {
	printf("\n call to test_s\?\?\n");
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

	printf("\n now computing different triplets and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j])  ) {
			j+=3;
			
			if (!(j%1000000)) printf("\n<s,\?,\?>= %u, %u, %u", B[i],B[i+1],B[i+2]);
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
	printf("\n source triplets = %zu, different triplets = %lu\n",size_buffer/3,n);

		/***************/{
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,\?,\?> queries over the existing triplets:\n");
	int results, results1 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = s(gindex, B[i*3], res);
		results1 = s_0(gindex, B[i*3], res2);
		
		
		if ((results != f[i]) || (results != results1)) { 
			printf("\n s operation failed (i=%lu)\n", i);
			printf("\n<s,\?,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
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
			if ( (res_s[x] != B[i*3])    ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,\?,\?> = <%u,\?,\?> and should be  <%u,\?,\?>\n", 
				res_s[x],   B[i*3] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <s\?\?> passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/


	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform sp queries.
	printf("\n now performing <s,\?,\?> queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		B[i*3] += ((B[i*3]> (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		
		results2 = s_0(gindex, B[i*3], res);
		results = s(gindex, B[i*3], res2);
		if (results != results2) { 
			printf("\n s\?\? operation failed (i=%lu)\n", i);
			printf("\n<s,\?,\?>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
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
			if ( (res_s[x] != B[i*3])    ||
				 (res_s[x] != res2_s[x]) ||  (res_p[x] != res2_p[x]) ||(res_o[x] != res2_o[x])
			   ) {
				printf("\n\n Retrieved triplet failed: <s,\?,\?> = <%u,\?,\?> and should be  <%u,\?,\?>\n", 
				res_s[x],   B[i*3] );fflush (stdout);

				printf("\n\t actually retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u> \n", 
				res_s[x], res_p[x],  res_o[x],res2_s[x], res2_p[x],  res2_o[x]  );fflush (stdout);

				exit(0);
			}				
		}	
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <s\?\?> (rand) passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	
	
	return 0;
}
