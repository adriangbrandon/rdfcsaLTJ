

int spo_orig(void *gindex, int S, int P, int O, unsigned int **buffer) {
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



#define USE_SELECT
int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
//int spo_opt_1(void *gindex, int S, int P, int O, unsigned int **buffer) {
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
		lp= getSelecticsa(g->myicsa, pp+1);
		rp = getSelecticsa(g->myicsa, pp+2)-1;

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


#define USE_SELECT
int spo_opt2_no_mejora(void *gindex, int S, int P, int O, unsigned int **buffer) {
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
		l= getSelecticsa(g->myicsa, ss+1);
		r = getSelecticsa(g->myicsa, ss+2)-1;
		numocc = r-l+1;
		//printf("\n ll=%lu and l=%lu :: rr=%lu and r=%lu",ll,l,rr,r);

		ulong lp,rp;
		lp= getSelecticsa(g->myicsa, pp+1);
		rp = getSelecticsa(g->myicsa, pp+2)-1;

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
	
	
	return res[0];
}



int spo_3BINSEARCH(void *gindex, int S, int P, int O, unsigned int **buffer) {
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










/*************************************************************************/

int sp (void *gindex, int S, int P, uint **buffer){ 
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


/*************************************************************************/	

int po (void *gindex, int P, int O, uint **buffer) { 
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

int so (void *gindex, int S, int O, uint **buffer){ 
	//	printf("\n@@@@@@@@@@@@@@@@ call to so (%d,%d)\n",S,O);fflush (stdout);

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

/*************************************************************************/

int s  (void *gindex, int S, uint **buffer) {
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
	
int p  (void *gindex, int P, uint **buffer){
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
	
int o  (void *gindex, int O, uint **buffer) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to o (%d)\n",O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint oo = mapID(g,O,OBJECT);
	
	pattern[0] = oo;
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
			uint n,s,p; 
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

			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);

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

/*************************************************************************/

