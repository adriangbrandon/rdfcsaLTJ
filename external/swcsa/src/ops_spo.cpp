
int spo_0(void *gindex, int S, int P, int O, unsigned int **buffer) {
//int spo_orig(void *gindex, int S, int P, int O, unsigned int **buffer) {
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
 //indexes/dbpedia16	dbpedia.queries/spo.txt	1767	500	500	3.534000	3.534000

//int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);
	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = ss; pattern[1] = pp; pattern[2] = oo;
	ulong numocc,l,r;
	countIntIndex(g->myicsa, pattern , 3, &numocc, &l, &r);
	
	res[0]=0;
	if (numocc) {
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;
			
		ulong i;
		for (i=l; i<=r;i++) {
			uint n; 
			n = res[0];
			res[0]++;

			// subjects
			res_s[n] = S;
			// predicates
			res_p[n] = P;
			// objects
			res_o[n] = O;
		}	
	}
	return res[0];
}



//int spo_orig_buffer(void *gindex, int S, int P, int O, unsigned int **buffer) {
int spo_1(void *gindex, int S, int P, int O, unsigned int **buffer) {
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spo.txt	1589	500	500	3.178000	3.178000
	
	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = ss; pattern[1] = pp; pattern[2] = oo;
	ulong numocc,l,r;
	countIntIndex_Buffer(g->myicsa, pattern , 3, &numocc, &l, &r);
	
	res[0]=0;
	if (numocc) {
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;
			
		ulong i;
		for (i=l; i<=r;i++) {
			uint n; 
			n = res[0];
			res[0]++;

			// subjects
			res_s[n] = S;
			// predicates
			res_p[n] = P;
			// objects
			res_o[n] = O;
		}	
	}
	return res[0];
}



int spo_3BINSEARCH(void *gindex, int S, int P, int O, unsigned int **buffer) {
//int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spo.txt	2664	500	500	5.328000	5.328000
	
	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = ss;
	ulong numocc,l,r;
	countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

	pattern[0] = pp;
	ulong numoccp,lp,rp;
	countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
	
	pattern[0] = oo;
	ulong numocco,lo,ro;
	countIntIndex(g->myicsa, pattern , 1, &numocco, &lo, &ro);
	

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
				if ((o>=lo) && (o<=ro)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S+1;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	return res[0];
}






/******************************************************************************/
// 3 selects for the intervals + checking upon the S-interval
// 2nd faster choice 
/******************************************************************************/

int spo_fast_3s_and_check(void *gindex, int S, int P, int O, unsigned int **buffer) {
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spo.txt	1088	500	500	2.176000	2.176000

//int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = pp;
		ulong numoccp,lp,rp;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
		
		pattern[0] = oo;
		ulong numocco,lo,ro;
		countIntIndex(g->myicsa, pattern , 1, &numocco, &lo, &ro);
	#else
		l= getSelecticsa(g->myicsa, ss+1);
		r = getSelecticsa(g->myicsa, ss+2)-1;
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1;

		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;


		ulong lo,ro,numocc_o;
		lo= getSelecticsa(g->myicsa, oo+1);
		ro = getSelecticsa(g->myicsa, oo+2)-1;
		numocc_o = ro-lo+1;
		
/*		printf("\n l,r = [%lu, %lu]",l,r);
		printf("\n lp,rp = [%lu, %lu]",lp,rp);
		printf("\n lo,ro = [%lu, %lu]",lo,ro);
		printf("\n lenS= %lu, lenP=%lu, lenO=%lu",r-l+1, rp-lp+1,ro-lo+1);
*/	
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
				if ((o>=lo) && (o<=ro)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	return res[0];
}


//int spo_opt2_mejor_de_S_O(void *gindex, int S, int P, int O, unsigned int **buffer) {
int spo_9(void *gindex, int S, int P, int O, unsigned int **buffer) {
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spo.txt	1020	500	500	2.040000	2.040000

	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = pp;
		ulong numoccp,lp,rp;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
		
		pattern[0] = oo;
		ulong numocc_o,lo,ro;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
	#else

		//** BEST CHOICE:
		// SUBJECTS  : use getseleccticsa375 for l and getSelect1NextRGK for r
		// PREDICATES: use tableLookups (getSelectTablePredicates())
		// OBJECTS   : use getselecticsa375 for both l and r
		// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
		// indexes/dbpedia8	dbpedia.queries/spo.txt	593	500	500	1.186000	1.186000
		
		//using 375 for s and o (table lookup for p)
		// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
		//	indexes/dbpedia8	dbpedia.queries/spo.txt	626	500	500	1.252000	1.252000

		//using 375 for s, p, and o (submitted spire)
		// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
		// indexes/dbpedia8	dbpedia.queries/spo.txt	751	500	500	1.502000	1.502000



		
		//l= getSelecticsa375(g->myicsa, ss+1);
		//r = getSelecticsa375(g->myicsa, ss+2)-1;

		l= getSelecticsa(g->myicsa, ss+1);
		//r = getSelecticsaNext(g->myicsa, ss+1,l)-1;		
		r = getSelecticsa(g->myicsa, ss+2)-1;
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		//lp= getSelecticsa375(g->myicsa, pp+1);
		//rp = getSelecticsa375(g->myicsa, pp+2)-1;
		
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp=getSelecticsaNext(g->myicsa, pp+1,lp)-1;
		//rp = getSelecticsa(g->myicsa, pp+2)-1;

		
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
		//ulong lpx = getSelectTablePredicates(g->myicsa,pp+1);
		//ulong rpx= getSelectTablePredicates(g->myicsa,pp+2)-1;
		/*
		if (lpx != lp) {
			cout << "distintos lp=" << lp <<", lpx=" << lpx << endl;
			cout << "pp+1="<< pp+1<< ", pp+1-g->ns=" << pp+1-g->ns << endl;
			cout << "g->ns="<< g->ns << endl;
			cout << "select1[predicate1]="<< getSelecticsa(g->myicsa, mapID(g,0,PREDICATE)+1) <<endl;
			cout << "getSelectTablePredicates[predicate1]="<< getSelectTablePredicates(g->myicsa, mapID(g,0,PREDICATE)+1 ) << endl;
			exit(0);
		}

		if (rpx != rp) {
			cout << "distintos rp=" << lp <<", rpx=" << lpx << endl;
			exit(0);
		}
		*/

		ulong lo,ro,numocc_o;


		//lo= getSelecticsa375(g->myicsa, oo+1);
		//ro = getSelecticsa375(g->myicsa, oo+2)-1;
		
		lo= getSelecticsa(g->myicsa, oo+1);
		//ro=getSelecticsaNext(g->myicsa, oo+1,lo)-1;
		ro = getSelecticsa(g->myicsa, oo+2)-1;
		numocc_o = ro-lo+1;
	
		//printf("\n*****\n \t l,r = [%lu, %lu]",l,r);
		//printf("\n \t lp,rp = [%lu, %lu]",lp,rp);
		//printf("\n \t lo,ro = [%lu, %lu]",lo,ro);
		//printf("\n lenS= %lu, lenP=%lu, lenO=%lu",r-l+1, rp-lp+1,ro-lo+1);
	
	#endif

	
	res[0]=0;

	if (numocc<=numocc_o) {
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		
		//printf("\n start with S");
		
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
				if ((o>=lo) && (o<=ro)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	else   //empezar por los objetos
	{
		//printf("\n start with O");
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (ro-lo+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,lo,ro);
		#endif		
		

		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		ulong i;
		for (i=lo; i<=ro;i++) {
			uint p,s; 
			// SUBJECTS
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-lo];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			if ((s>=l) && (s <=r)) {
				// predicates
				p= getPsiicsa(g->myicsa, s);
				if ((p>=lp) && (p<=rp)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	
	
				#ifndef EXPERIMENTS
				uint *res_s = res+1;
				uint *res_p = res+1+ (MAX_RESULTS)*1;
				uint *res_o = res+1+ (MAX_RESULTS)*2;
			//checks results are "expected"
			if ( (res_s[0] != S) ||  (res_p[0] != P) ||  (res_o[0] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[0], res_p[0], res_o[0],S,P,O );fflush (stdout);
				exit(0);
			}
			#endif	
	
	return res[0];
}







//int spo_opt2_mejor_de_S_O(void *gindex, int S, int P, int O, unsigned int **buffer) {
int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spo.txt	1020	500	500	2.040000	2.040000

	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	
	
//	getSelecticsaNext(g->myicsa, 1,1);
//	printf("\n test test_select1ymas funciono bien!!\n");
//	exit(0);
	
	
	
	
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = pp;
		ulong numoccp,lp,rp;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
		
		pattern[0] = oo;
		ulong numocc_o,lo,ro;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
	#else

		//** BEST CHOICE:
		// SUBJECTS  : use getseleccticsa375 for l and getSelect1NextRGK for r
		// PREDICATES: use tableLookups (getSelectTablePredicates())
		// OBJECTS   : use getselecticsa375 for both l and r
		// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
		// indexes/dbpedia8	dbpedia.queries/spo.txt	593	500	500	1.186000	1.186000
		
		//using 375 for s and o (table lookup for p)
		// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
		//	indexes/dbpedia8	dbpedia.queries/spo.txt	626	500	500	1.252000	1.252000

		//using 375 for s, p, and o (submitted spire)
		// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
		// indexes/dbpedia8	dbpedia.queries/spo.txt	751	500	500	1.502000	1.502000


//
//		
//		//l= getSelecticsa375(g->myicsa, ss+1);
//		//r = getSelecticsa375(g->myicsa, ss+2)-1;
//
//		l= getSelecticsa(g->myicsa, ss+1);
//		r = getSelecticsaNext(g->myicsa, ss+1,l)-1;		
//		//r = getSelecticsa(g->myicsa, ss+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &l, &r);	r--;
		
		
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		//lp= getSelecticsa375(g->myicsa, pp+1);
		//rp = getSelecticsa375(g->myicsa, pp+2)-1;
		
		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp=getSelecticsaNext(g->myicsa, pp+1,lp)-1;
		//rp = getSelecticsa(g->myicsa, pp+2)-1;

		
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
		//ulong lpx = getSelectTablePredicates(g->myicsa,pp+1);
		//ulong rpx= getSelectTablePredicates(g->myicsa,pp+2)-1;
		/*
		if (lpx != lp) {
			cout << "distintos lp=" << lp <<", lpx=" << lpx << endl;
			cout << "pp+1="<< pp+1<< ", pp+1-g->ns=" << pp+1-g->ns << endl;
			cout << "g->ns="<< g->ns << endl;
			cout << "select1[predicate1]="<< getSelecticsa(g->myicsa, mapID(g,0,PREDICATE)+1) <<endl;
			cout << "getSelectTablePredicates[predicate1]="<< getSelectTablePredicates(g->myicsa, mapID(g,0,PREDICATE)+1 ) << endl;
			exit(0);
		}

		if (rpx != rp) {
			cout << "distintos rp=" << lp <<", rpx=" << lpx << endl;
			exit(0);
		}
		*/

		ulong lo,ro,numocc_o;


//		//lo= getSelecticsa375(g->myicsa, oo+1);
//		//ro = getSelecticsa375(g->myicsa, oo+2)-1;
//		
//		lo= getSelecticsa(g->myicsa, oo+1);
//		ro = getSelecticsa(g->myicsa, oo+2)-1;
//		//ro=getSelecticsaNext(g->myicsa, oo+1,lo)-1;
	geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;

			//
			//		size_t roo,loo;
			//		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &loo, &roo);	roo--;
			//		if (ro != roo) {
			//			printf("\n lo =  %zu, occs=%zu ",lo, ro-lo+1);
			//			printf("\n geticsa_select_j_y_j_mas_1 FALLO, %zu != %zu",ro, roo);
			//		}
			//		ro=roo;
			//		lo=loo;
			
	
		numocc_o = ro-lo+1;
	
//		printf("\n*****\n \t l,r = [%lu, %lu]",l,r);
//		printf("\n \t lp,rp = [%lu, %lu]",lp,rp);
//		printf("\n \t lo,ro = [%lu, %lu]",lo,ro);
//		printf("\n lenS= %lu, lenP=%lu, lenO=%lu",r-l+1, rp-lp+1,ro-lo+1);
	
	#endif

	
	res[0]=0;

	if (numocc<=numocc_o) {
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		
		//printf("\n start with S");
		
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
				if ((o>=lo) && (o<=ro)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
					
//					res_s[n] = getRankicsa(g->myicsa,i) -1; 
//					res_s[n] =unmapID(g,res_s[n],SUBJECT);
//					res_p[n] = getRankicsa(g->myicsa,p) -1;
//					res_p[n] =unmapID(g,res_p[n],PREDICATE);					
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	else   //empezar por los objetos
	{
		//printf("\n start with O");
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (ro-lo+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,lo,ro);
		#endif		
		

		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		ulong i;
		for (i=lo; i<=ro;i++) {
			uint p,s; 
			// SUBJECTS
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-lo];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			if ((s>=l) && (s <=r)) {
				// predicates
				p= getPsiicsa(g->myicsa, s);
				if ((p>=lp) && (p<=rp)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;


//					res_s[n] = getRankicsa(g->myicsa,s) -1;
//					res_s[n] =unmapID(g,res_s[n],SUBJECT);
//					res_o[n] = getRankicsa(g->myicsa,i) -1;
//					res_o[n] =unmapID(g,res_o[n],OBJECT);


				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	
	
				#ifndef EXPERIMENTS
				uint *res_s = res+1;
				uint *res_p = res+1+ (MAX_RESULTS)*1;
				uint *res_o = res+1+ (MAX_RESULTS)*2;
			//checks results are "expected"
			if ( (res_s[0] != S) ||  (res_p[0] != P) ||  (res_o[0] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[0], res_p[0], res_o[0],S,P,O );fflush (stdout);
				exit(0);
			}
			#endif	
	
	return res[0];
}





























/*
//int spo_opt2_mejor_de_S_O(void *gindex, int S, int P, int O, unsigned int **buffer) {
int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
//	 <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>
// indexes/dbpedia16	dbpedia.queries/spo.txt	1020	500	500	2.040000	2.040000

	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc, &l, &r);

		pattern[0] = pp;
		ulong numoccp,lp,rp;
		countIntIndex(g->myicsa, pattern , 1, &numoccp, &lp, &rp);
		
		pattern[0] = oo;
		ulong numocc_o,lo,ro;
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
	#else
		//l= getSelecticsa375(g->myicsa, ss+1);
		//r = getSelecticsa375(g->myicsa, ss+2)-1;

		l= getSelecticsa(g->myicsa, ss+1);
		//r = getSelecticsaNext(g->myicsa, ss+1,l)-1;		
		r = getSelecticsa(g->myicsa, ss+2)-1;
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		//lp= getSelecticsa375(g->myicsa, pp+1);
		//rp = getSelecticsa375(g->myicsa, pp+2)-1;
		
		lp= getSelecticsa(g->myicsa, pp+1);
		//rp=getSelecticsaNext(g->myicsa, pp+1,lp)-1;
		rp = getSelecticsa(g->myicsa, pp+2)-1;

		

		ulong lpx = getSelectTablePredicates(g->myicsa,pp+1);
		if (lpx != lp) {
			cout << "distintos lp=" << lp <<", lpx=" << lpx << endl;
			cout << "pp+1="<< pp+1<< ", pp+1-g->ns=" << pp+1-g->ns << endl;
			cout << "g->ns="<< g->ns << endl;
			cout << "select1[predicate1]="<< getSelecticsa(g->myicsa, mapID(g,0,PREDICATE)+1) <<endl;
			cout << "getSelectTablePredicates[predicate1]="<< getSelectTablePredicates(g->myicsa, mapID(g,0,PREDICATE) ) << endl;
			exit(0);
		}
		{uint i;

			cout << "select1[predicate1]="<< getSelecticsa(g->myicsa, mapID(g,0,PREDICATE)+1) <<endl;
			cout << "getSelectTablePredicates[predicate1]="<< getSelectTablePredicates(g->myicsa, mapID(g,0,PREDICATE)+1 ) << endl;

			for (i=0;i<g->np;i++) {
				uint p= mapID(g,i,PREDICATE);
				uint s=getSelecticsa(g->myicsa, p+1-1);
				uint sl=getSelectTablePredicates(g->myicsa,p+1-1);
				if (s!=sl) {
					printf("\n DISTINTOS checkSELECT: i=%u, %u != %u",i,s,sl);
					exit(0);
				}
			}
			printf("\n iguales");
			exit(0);
	
		}


		ulong lo,ro,numocc_o;


		//lo= getSelecticsa375(g->myicsa, oo+1);
		//ro = getSelecticsa375(g->myicsa, oo+2)-1;
		
		lo= getSelecticsa(g->myicsa, oo+1);
		//ro=getSelecticsaNext(g->myicsa, oo+1,lo)-1;
		ro = getSelecticsa(g->myicsa, oo+2)-1;
		numocc_o = ro-lo+1;
	
		//printf("\n*****\n \t l,r = [%lu, %lu]",l,r);
		//printf("\n \t lp,rp = [%lu, %lu]",lp,rp);
		//printf("\n \t lo,ro = [%lu, %lu]",lo,ro);
		//printf("\n lenS= %lu, lenP=%lu, lenO=%lu",r-l+1, rp-lp+1,ro-lo+1);
	
	#endif

	
	res[0]=0;

	if (numocc<=numocc_o) {
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (r-l+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,l,r);
		#endif		
		//printf("\n start with S");
		
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
				if ((o>=lo) && (o<=ro)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	else   //empezar por los objetos
	{
		//printf("\n start with O");
		#ifdef BUFFER_PSI_ON
			uint *bufferpsi = (uint*) my_malloc(sizeof(uint) * (ro-lo+1));
			getPsiBuffericsa(g->myicsa,bufferpsi,lo,ro);
		#endif		
		

		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;

		ulong i;
		for (i=lo; i<=ro;i++) {
			uint p,s; 
			// SUBJECTS
			#ifdef BUFFER_PSI_ON
				s=bufferpsi[i-lo];
			#else
				s= getPsiicsa(g->myicsa, i);
			#endif		

			if ((s>=l) && (s <=r)) {
				// predicates
				p= getPsiicsa(g->myicsa, s);
				if ((p>=lp) && (p<=rp)) {
					uint n = res[0];
					res[0]++;
					// subjects
					res_s[n] = S;
					// predicates
					res_p[n] = P;
					// objects
					res_o[n] = O;
				}
			}

		}

		#ifdef BUFFER_PSI_ON
				my_free_array(bufferpsi);
		#endif
	}
	
	
	
				//#ifndef EXPERIMENTS
				uint *res_s = res+1;
				uint *res_p = res+1+ (MAX_RESULTS)*1;
				uint *res_o = res+1+ (MAX_RESULTS)*2;
			//checks results are "expected"
			if ( (res_s[0] != S) ||  (res_p[0] != P) ||  (res_o[0] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[0], res_p[0], res_o[0],S,P,O );fflush (stdout);
				exit(0);
			}
			//#endif	
	
	return res[0];
}
*/




/******************************************************************************/
// 3 selects for the intervals + 2 binary searches (backward-search type) to check:
// (a)Ps into Os interval
// (b)Ss into POs interval
// Created 2015/01/14
/******************************************************************************/

//ABRIL DE 2020. CAMBIADO POR SPO, QUE COMEZA EN OBXECTOS
int spo_2(void *gindex, int S, int P, int O, unsigned int **buffer) {
//int spo_3s_and_2backsearch(void *gindex, int S, int P, int O, unsigned int **buffer) {
// <index>	<query-set>	<microsec>	<nqueries>	<totalres>	<microsec/query>	<microsec/totalres>	
// indexes/dbpedia16	dbpedia.queries/spo.txt	1647	500	500	3.294000	3.294000
	
	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lp,rp, numocc_p;
	ulong lo,ro, numocc_o; 
	
	#ifndef USE_SELECT
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &l, &r);ls=l; rs=r;

		pattern[0] = pp;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
		
		pattern[0] = oo;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
	#else
	
		ls= getSelecticsa(g->myicsa, ss+1);
		rs = getSelecticsa(g->myicsa, ss+2)-1;
		numocc_s = rs-ls+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;

		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1; 
		numocc_p = rp-lp+1;

		lo= getSelecticsa(g->myicsa, oo+1);
		ro = getSelecticsa(g->myicsa, oo+2)-1;
		numocc_o = ro-lo+1;
	/*	
		printf("\n ls,rs = [%lu, %lu]",ls,rs);
		printf("\n lp,rp = [%lu, %lu]",lp,rp);
		printf("\n lo,ro = [%lu, %lu]",lo,ro);
		printf("\n lenS= %lu, lenP=%lu, lenO=%lu",rs-ls+1, rp-lp+1,ro-lo+1);
	*/
	#endif
		
	//	printf("\n ls,rs = [%lu, %lu]",ls,rs);
	//	printf("\n lp,rp = [%lu, %lu]",lp,rp);
	//	printf("\n lo,ro = [%lu, %lu]",lo,ro);
	//	printf("\n lenS= %lu, lenP=%lu, lenO=%lu",rs-ls+1, rp-lp+1,ro-lo+1);		
		
	res[0]=0;
	
	if (numocc_s && numocc_p && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;


		l=lp;r=rp;
		//printf("\n Before BSearchP-O= lp,rp = [%lu, %lu]",l,r);
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);
	//	printf("\n **BSearchP-O= lp,rp = [%lu, %lu]",l,r);
		
		if (!numocc) return res[0];
		
	//	printf("\n BSearchP-O= lp,rp = [%lu, %lu]",l,r);
			
		ulong LP =l, RP=r;
		l=ls;r=rs;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, LP, RP);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, LP, RP);
		if (!numocc) return res[0];
	//	printf("\n **BSearchS-PO= ls,rs = [%lu, %lu]",l,r);

		ulong i;
		for (i=l; i<=r;i++) {
			uint n = res[0];
			res[0]++;
			// subjects
			res_s[n] = S;
			// predicates
			res_p[n] = P;
			// objects
			res_o[n] = O;
			
			
/*
			uint p,o; 
			// subjects
			res_s[n] = getRankicsa(g->myicsa,i) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);

			// predicates
			p= getPsiicsa(g->myicsa, i);
			res_p[n] = getRankicsa(g->myicsa,p) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);

			// objects
			o= getPsiicsa(g->myicsa, p);
			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);
			
			//#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_s[n] != S) ||  (res_p[n] != P) ||  (res_o[n] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[n], res_p[n], res_o[n],S,P,O );fflush (stdout);
				exit(0);
			}
			//#endif	
*/

			
		}
	}
	
	return res[0];
}







//********************************************************************************


/*************  TEST SPO*****************************************************/
int test_spo(void *gindex, uint **res) {
	printf("\n call to test_spo\n");
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
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) && (B[i+2]==B[j+2])  ) {
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
	printf("\n source triplets = %zu, different triplets = %lu\n",size_buffer/3,n);




	
	/***************/{
	fflush(stdout);fflush(stderr);
	//now perform spo queries.
	printf("\n now performing <s,p,o> queries over the existing triplets:\n");
	int results ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		results = spo(gindex, B[i*3],B[i*3+1],B[i*3+2], res);
		
		if (results != f[i]) { 
			printf("\n spo operation failed (i=%lu)\n", i);
			printf("\n<s,p,o>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
			//checks results are "expected"
			int x;
			for (x=0; x<results;x++)
			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ||  (res_o[x] != B[i*3+2]) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[x], res_p[x], res_o[x], B[i*3],B[i*3+1],B[i*3+2] );fflush (stdout);
				exit(0);
			}				
		}
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <spo> passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	



	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform spo queries.
	printf("\n now performing <s,p,o> queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
	for (i=0; i< n ; i++) {
		B[i*3]   += ((B[i*3]  > (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );

		results2 = spo_fast_3s_and_check(gindex, B[i*3],B[i*3+1],B[i*3+2], res);
		//results2  = spo_orig(gindex, B[i*3],B[i*3+1],B[i*3+2], res);
		
		results  = spo(gindex, B[i*3],B[i*3+1],B[i*3+2], res);
		
		if (results != results2) { 
			printf("\n spo operation failed (i=%lu) results1=%d, results2=%d\n", i,results, results2);
			printf("\n<s,p,o>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
			//checks results are the "expected" ones
			int x;
			for (x=0; x<results;x++)
			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ||  (res_o[x] != B[i*3+2]) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				res_s[x], res_p[x], res_o[x], B[i*3],B[i*3+1],B[i*3+2] );fflush (stdout);
				exit(0);
			}				
		}		
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
	}	
	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
	printf("\n TEST <spo> (rand) passed **ok** (total results = %lu) \n", total_results);
	}
	/***************/	

	
	return 0;
}






































///*************************************************************************************************/
///*   DUAL - RDFCSA - 																			 */
///*************************************************************************************************/

// uses backward search, given 3 ranges obtained with select
/* receives a rdfcsa in SPO order as 1st parameter */
int dual_rdfcsaSPO_spo (void *gindex, int S, int P, int O, unsigned int **buffer) {

	return spo_2(gindex, S,P,O,buffer);
	
//	twcsa *g = (twcsa *)gindex;
//
//	//printf("\n @@@@@@@@@@@@@@@@ call to dual_spo (S,P,O)= (%d,%d,%d)\n",S,P,O);fflush (stdout);
//	
//	uint *res= *buffer; //no malloc performed here!!
//
//	uint ss = mapID(g,S,SUBJECT);
//	uint pp = mapID(g,P,PREDICATE);
//	uint oo = mapID(g,O,OBJECT);
//	
//	ulong numocc,l,r;
//
//	ulong ls,rs, numocc_s;
//	ulong lp,rp, numocc_p;
//	ulong lo,ro, numocc_o; 
//	
//	#ifndef USE_SELECT
//		uint pattern[MAX_ENTRIES];
//		pattern[0] = ss;
//		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &l, &r);
//
//		pattern[0] = pp;		
//		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
//		
//		pattern[0] = oo;		
//		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
//	#else
//		//ls= getSelecticsa(g->myicsa, ss+1);
//		//rs = getSelecticsa(g->myicsa, ss+2)-1;
//		//numocc_s = rs-ls+1;
//	
//		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		
//		numocc_s = rs-ls+1;
//		
//		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);
//
//		//lp= getSelecticsa(g->myicsa, pp+1);
//		//rp=getSelecticsaNext(g->myicsa, pp+1,lp)-1;
//		//rp = getSelecticsa(g->myicsa, pp+2)-1;
//		lp= getSelectTablePredicates(g->myicsa,pp+1);
//		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
//		numocc_p = rp-lp+1;
//		
//		//		lo= getSelecticsa(g->myicsa, oo+1);
//		//		ro = getSelecticsa(g->myicsa, oo+2)-1;
//		//		//ro=getSelecticsaNext(g->myicsa, oo+1,lo)-1;
//
//		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;	
//		numocc_o = ro-lo+1;
//		
//			//	printf("\n** Selects - ranges***\n ");
//			//	printf("\n \t ls,rs = [%lu, %lu]",ls,rs);
//			//	printf("\n \t lp,rp = [%lu, %lu]",lp,rp);
//			//	printf("\n \t lo,ro = [%lu, %lu]",lo,ro);
//			//	printf("\n lenS= %lu, lenP=%lu, lenO=%lu",numocc_s, numocc_p, numocc_o);
//		
//	#endif
//	
//	res[0]=0;
//
//	if (numocc_s && numocc_p && numocc_o) {	
//		uint *res_s = res+1;
//		uint *res_p = res+1+ (MAX_RESULTS)*1;
//		uint *res_o = res+1+ (MAX_RESULTS)*2;
//
//
//		l=lp;r=rp;
//			//printf("\n Before BSearchP-O= lp,rp = [%lu, %lu]",l,r);
//			//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);
//		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);
//			//	printf("\n **BSearchP-O= lp,rp = [%lu, %lu]",l,r);
//		
//		if (!numocc) return res[0];
//		
//			//	printf("\n BSearchP-O= lp,rp = [%lu, %lu]",l,r);
//			
//		ulong LP =l, RP=r;
//		l=ls;r=rs;
//			//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, LP, RP);
//		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, LP, RP);
//		if (!numocc) return res[0];
//			//	printf("\n **BSearchS-PO= ls,rs = [%lu, %lu]",l,r);
//
//		ulong i;
//		for (i=l; i<=r;i++) {
//			uint n = res[0];
//			res[0]++;
//			// subjects
//			res_s[n] = S;
//			// predicates
//			res_p[n] = P;
//			// objects
//			res_o[n] = O;
//						
//			/*
//				uint p,o; 
//				// subjects
//				res_s[n] = getRankicsa(g->myicsa,i) -1;
//				res_s[n] = unmapID(g,res_s[n],SUBJECT);
//
//				// predicates
//				p= getPsiicsa(g->myicsa, i);
//				res_p[n] = getRankicsa(g->myicsa,p) -1;
//				res_p[n] = unmapID(g,res_p[n],PREDICATE);
//
//				// objects
//				o= getPsiicsa(g->myicsa, p);
//				res_o[n] = getRankicsa(g->myicsa,o) -1;
//				res_o[n] = unmapID(g,res_o[n],OBJECT);			
//			*/			
//		}
//	}
//	
//	
//	
//			#ifndef EXPERIMENTS
//			uint *res_s = res+1;
//			uint *res_p = res+1+ (MAX_RESULTS)*1;
//			uint *res_o = res+1+ (MAX_RESULTS)*2;
//			//checks results are "expected"
//			if ( (res_s[0] != S) ||  (res_p[0] != P) ||  (res_o[0] != O) ) {
//				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
//				                                               res_s[0], res_p[0], res_o[0],S,P,O );fflush (stdout);
//				exit(0);
//			}
//			#endif	
//	
//	return res[0];
}


// uses backward search, given 3 ranges obtained with select
/* receives a rdfcsa in OPS order as 1st parameter */
int dual_rdfcsaOPS_ops (void *gindex, int O, int P, int S, unsigned int **buffer) {

	twcsa *g = (twcsa *)gindex;
	
	uint *res= *buffer; //no malloc performed here!!

	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaOPS_ops (S,P,O)= (%d,%d,%d)\n",S,P,O);fflush (stdout);


	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lp,rp, numocc_p;
	ulong lo,ro, numocc_o; 
	
	#ifndef USE_SELECT
		uint pattern[MAX_ENTRIES];
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &ls, &rs);

		pattern[0] = pp;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
		
		pattern[0] = oo;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
	#else
		//ls= getSelecticsa(g->myicsa, ss+1);
		//rs = getSelecticsa(g->myicsa, ss+2)-1;
		//numocc_s = rs-ls+1;
	
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		
		numocc_s = rs-ls+1;
		
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp=getSelecticsaNext(g->myicsa, pp+1,lp)-1;
		//rp = getSelecticsa(g->myicsa, pp+2)-1;
		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;
		numocc_p = rp-lp+1;


	{	// PARA ELIMINAR TENER AQUÍ SÓLO PARA TESTEAR !!
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
		
		//		lo= getSelecticsa(g->myicsa, oo+1);
		//		ro = getSelecticsa(g->myicsa, oo+2)-1;
		//		//ro=getSelecticsaNext(g->myicsa, oo+1,lo)-1;

		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;	
		numocc_o = ro-lo+1;
		
		//		printf("\n*****\n \t l,r = [%lu, %lu]",l,r);
		//		printf("\n \t lp,rp = [%lu, %lu]",lp,rp);
		//		printf("\n \t lo,ro = [%lu, %lu]",lo,ro);
		//		printf("\n lenS= %lu, lenP=%lu, lenO=%lu",r-l+1, rp-lp+1,ro-lo+1);
		
	#endif
	
	res[0]=0;

	if (numocc_s && numocc_p && numocc_o) {	
		uint *res_s = res+1;
		uint *res_p = res+1+ (MAX_RESULTS)*1;
		uint *res_o = res+1+ (MAX_RESULTS)*2;


		l=lp;r=rp;
			//printf("\n Before BSearchP-S= lp,rp = [%lu, %lu]",l,r);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, ls, rs);
			//	printf("\n **BSearchP-S= lp,rp = [%lu, %lu]",l,r);
		
		if (!numocc) return res[0];
		
			//	printf("\n BSearchP-S= lp,rp = [%lu, %lu]",l,r);
			
		ulong LP =l, RP=r;
		l=lo;r=ro;
			//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, LP, RP);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, LP, RP);
		if (!numocc) return res[0];
			//	printf("\n **BSearch O-Ps= ls,rs = [%lu, %lu]",l,r);

		ulong i;
		for (i=l; i<=r;i++) {
			uint n = res[0];
			res[0]++;
			// subjects
			res_s[n] = S;
			// predicates
			res_p[n] = P;
			// objects
			res_o[n] = O;
						
			/*
				uint p,o; 
				// subjects
				res_s[n] = getRankicsa(g->myicsa,i) -1;
				res_s[n] = unmapID(g,res_s[n],SUBJECT);

				// predicates
				p= getPsiicsa(g->myicsa, i);
				res_p[n] = getRankicsa(g->myicsa,p) -1;
				res_p[n] = unmapID(g,res_p[n],PREDICATE);

				// objects
				o= getPsiicsa(g->myicsa, p);
				res_o[n] = getRankicsa(g->myicsa,o) -1;
				res_o[n] = unmapID(g,res_o[n],OBJECT);			
			*/			
		}
	}
	
	
	
			#ifndef EXPERIMENTS
			uint *res_s = res+1;
			uint *res_p = res+1+ (MAX_RESULTS)*1;
			uint *res_o = res+1+ (MAX_RESULTS)*2;
			//checks results are "expected"
			if ( (res_s[0] != S) ||  (res_p[0] != P) ||  (res_o[0] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[0], res_p[0], res_o[0],S,P,O );fflush (stdout);
				exit(0);
			}
			#endif	
	
	return res[0];
}


//Computes the range [left,right] where triple <S,P,O> is found.
//Returns the size of the range (right-left+1), or 0 if <S,P,O> is not found.
uint dual_rdfcsaSPO_init_spo (void *gindex, int S, int P, int O, uint *left, uint *right) {
	
	//printf("\n @@@@@@@@@@@@@@@@ call to dual_rdfcsaSPO_init_spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);
	
	ulong numocc,l,r;

	ulong ls,rs, numocc_s;
	ulong lp,rp, numocc_p;
	ulong lo,ro, numocc_o; 
	
	#ifndef USE_SELECT
	uint pattern[MAX_ENTRIES];
		pattern[0] = ss;
		countIntIndex(g->myicsa, pattern , 1, &numocc_s, &l, &r); ls=l; rs=r;

		pattern[0] = pp;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_p, &lp, &rp);
		
		pattern[0] = oo;		
		countIntIndex(g->myicsa, pattern , 1, &numocc_o, &lo, &ro);
	#else
	
		//ls= getSelecticsa(g->myicsa, ss+1);
		//rs = getSelecticsa(g->myicsa, ss+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, ss+1, &ls, &rs);	rs--;		

		numocc_s = rs-ls+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		lp= getSelectTablePredicates(g->myicsa,pp+1);
		rp= getSelectTablePredicates(g->myicsa,pp+2)-1;

		//lp= getSelecticsa(g->myicsa, pp+1);
		//rp = getSelecticsa(g->myicsa, pp+2)-1; 
		numocc_p = rp-lp+1;

		//lo= getSelecticsa(g->myicsa, oo+1);
		//ro = getSelecticsa(g->myicsa, oo+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, oo+1, &lo, &ro);	ro--;
		
		numocc_o = ro-lo+1;
	#endif

		
	//	printf("\n ls,rs = [%lu, %lu]",ls,rs);
	//	printf("\n lp,rp = [%lu, %lu]",lp,rp);
	//	printf("\n lo,ro = [%lu, %lu]",lo,ro);
	//	printf("\n lenS= %lu, lenP=%lu, lenO=%lu",rs-ls+1, rp-lp+1,ro-lo+1);		
			
	if (numocc_s && numocc_p && numocc_o) {	

		l=lp;r=rp;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, lo, ro);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, lo, ro);		
		if (!numocc) return 0;
					
		ulong LP =l, RP=r;
		l=ls;r=rs;
		//binSearchPsiTarget(g->myicsa, &l,&r, &numocc, LP, RP);
		binSearchPsiTarget_samplesFirst(g->myicsa, &l,&r, &numocc, LP, RP);
		if (!numocc) return 0;

		*left = l;
		*right= r;
		return r-l+1;  /* should be 1 */
		
		//	ulong i;		
		//	for (i=l; i<=r;i++) {
		//		uint n = res[0];
		//		res[0]++;
		//		// subjects
		//		res_s[n] = S;
		//		// predicates
		//		res_p[n] = P;
		//		// objects
		//		res_o[n] = O;
		//	
		//	}
	}
	
	return 0;
}




int dual_test_init_spo_ops  (void *index) {
	
	return 0;
}







/*************  TEST SPO*****************************************************/
//1st parameter is a pointer to a tdualrdfcsa
//typedef struct {
//	twcsa *spo;
//	twcsa *ops;
//} tdualrdfcsa;

int dual_test_spo_ops(void *index, uint **res) {
	printf("\n call to dual_test_spo_ops\n");

double start ,end;

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	twcsa *g = dualrdf->spo;
	twcsa *g2 = dualrdf->ops;
	

	uint *buffer;
	size_t size_buffer;
	printf("\n dumping source data \n");fflush(stdout);
		dumpSourceData(g, &buffer, &size_buffer);
	printf("... %zu triples recovered (%zu expected)\n",size_buffer / 3, g->n);fflush(stdout);

	
	uint *B = buffer;
	uint *f = (uint *) malloc (sizeof(uint) * size_buffer);
	
	ulong z=0, i=0, n=size_buffer, j;

	printf("\n now computing different triplets and its number of occs \n");fflush(stdout);
	while (i<n) {
		j=i+3;
		while ((j<n) && (B[i]==B[j]) && (B[i+1]==B[j+1]) && (B[i+2]==B[j+2])  ) {
			j+=3;
			
			if (!(j%1000000)   && (i<=50000000)) printf("\n\t<s,p,o>= %u, %u, %u", B[i],B[i+1],B[i+2]);
		}
		f[z/3]=(j-i)/3;   //num-occs of triple.
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




	
//	/***************/{
//	fflush(stdout);fflush(stderr);
//	//now perform spo queries.
//	printf("\n now performing <s,p,o> (dual_spo()) queries over the existing triplets:\n");
//	int results ;	
//	long total_results=0;
//	for (i=0; i< n ; i++) {
//		results = dual_rdfcsaSPO_spo(g, B[i*3],B[i*3+1],B[i*3+2], res);
//		
//		if (results != f[i]) { 
//			printf("\n spo operation failed (i=%lu)\n", i);
//			printf("\n<s,p,o>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
//			exit(0);
//		}
//		if (results > 0) {
//			uint *res_s = (*res)+1;
//			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
//			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
//			//checks results are "expected"
//			int x;
//			for (x=0; x<results;x++)
//			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ||  (res_o[x] != B[i*3+2]) ) {
//				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
//				                                               res_s[x], res_p[x], res_o[x], B[i*3],B[i*3+1],B[i*3+2] );fflush (stdout);
//				exit(0);
//			}				
//		}
//		total_results +=results;
//		
//		if((i%(n/1000)==0)) fprintf(stderr, "Processing %.1f%%\r", (float)i/n*100);
//	}	
//	fprintf(stderr, "Processing %.1f%%", (float)i/n*100); fflush(stderr);
//	printf("\n TEST <spo> passed **ok** (total results = %lu) \n", total_results);
//	}
//	/***************/	








	/***************/	
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform spo queries.
	printf("\n now performing <s,p,o> (dual_rdfcsaSPO_spo() && dual_rdfcsaOPS_ops()) queries over existing triplets:\n");
	int results, results2 ;	
	long total_results=0;
	

start = getTime2();	
	i=0;
	printf("\n\t  <%u,%u,%u> skipped\n", B[i*3],B[i*3+1],B[i*3+2]); fflush(stdout);fflush(stderr);
	//skips i=0;
	for (i=1; i< n ; i++) {
//		results2 = spo_fast_3s_and_check(g, B[i*3],B[i*3+1],B[i*3+2], res);
		//results2  = spo_orig(gindex, B[i*3],B[i*3+1],B[i*3+2], res);
//		results  = spo(g, B[i*3],B[i*3+1],B[i*3+2], res);
		
		results  = dual_rdfcsaSPO_spo(g , B[i*3],B[i*3+1],B[i*3+2], res);
		results2 = dual_rdfcsaOPS_ops(g2, B[i*3+2] ,B[i*3+1],B[i*3], res);
		
		if (results != f[i]) {
			printf("\n spo operation failed (i=%lu) results1=%d, results2=%d\n", i,results, results2);
			printf("\n<s,p,o>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}		
		if (results != results2) { 
			printf("\n spo operation failed (i=%lu) results1=%d, results2=%d\n", i,results, results2);
			printf("\n<s,p,o>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
			//checks results are the "expected" ones
			int x;
			for (x=0; x<results;x++)
			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ||  (res_o[x] != B[i*3+2]) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				res_s[x], res_p[x], res_o[x], B[i*3],B[i*3+1],B[i*3+2] );fflush (stdout);
				exit(0);
			}				
		}		
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <spo> (rand) passed **ok** (total results = %lu) ", total_results);
end = getTime2();	
	printf("\n time: %.3f secs\n\n", end-start );
	}
	/***************/	


	/***************/
	{
	srand(time(NULL));
	fflush(stdout);fflush(stderr);
	//now perform spo queries.
	printf("\n now performing <s,p,o> (dual_rdfcsaSPO_spo() && dual_rdfcsaOPS_ops()) queries over rather unexisting triplets:\n");
	int results, results2 ;	
	long total_results=0;
start = getTime2();	
	
	//skips i=0;
	for (i=1; i< n ; i++) {
		B[i*3]   += ((B[i*3]  > (g->gaps[0]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+1] += ((B[i*3+1]> (g->gaps[1]+1)) ?  (rand01()*(-1)) : rand01() );
		B[i*3+2] += ((B[i*3+2]> (g->gaps[2]+1)) ?  (rand01()*(-1)) : rand01() );
		if (B[i*3+2] >= g->gaps[3]) B[i*3+2] = B[i*3+2] >= g->gaps[3] -1;

//		results2 = spo_fast_3s_and_check(g, B[i*3],B[i*3+1],B[i*3+2], res);
		//results2  = spo_orig(gindex, B[i*3],B[i*3+1],B[i*3+2], res);
//		results  = spo(g, B[i*3],B[i*3+1],B[i*3+2], res);
		
		results  = dual_rdfcsaSPO_spo(g , B[i*3],B[i*3+1],B[i*3+2], res);
		results2 = dual_rdfcsaOPS_ops(g2, B[i*3+2] ,B[i*3+1],B[i*3], res);

		if (results != results2) { 
			printf("\n spo operation failed (i=%lu) results1=%d, results2=%d\n", i,results, results2);
			printf("\n<s,p,o>= <%u, %u, %u>", B[i*3],B[i*3+1],B[i*3+2]);
			exit(0);
		}
		if (results > 0) {
			uint *res_s = (*res)+1;
			uint *res_p = (*res)+1+ (MAX_RESULTS)*1;
			uint *res_o = (*res)+1+ (MAX_RESULTS)*2;		
			//checks results are the "expected" ones
			int x;
			for (x=0; x<results;x++)
			if ( (res_s[x] != B[i*3]) ||  (res_p[x] != B[i*3+1]) ||  (res_o[x] != B[i*3+2]) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				res_s[x], res_p[x], res_o[x], B[i*3],B[i*3+1],B[i*3+2] );fflush (stdout);
				exit(0);
			}				
		}		
		total_results +=results;
		
		if((i%(n/1000)==0)) fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start);;
 	}	
 	
 	fprintf(stderr, "\rProcessing %.1f%% (%.1f secs)", (float)i/n*100, getTime2()-start); fflush(stderr);
	printf("\n TEST <spo> (rand) passed **ok** (total results = %lu) ", total_results);
end = getTime2();	
	printf("\n time: %.3f secs\n\n", end-start );
	
	}
	/***************/	

	
	return 0;
}




