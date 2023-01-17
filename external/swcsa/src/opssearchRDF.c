

int spo(void *gindex, int S, int P, int O, unsigned int **buffer) {
	//	printf("\n @@@@@@@@@@@@@@@@ call to spo (%d,%d,%d)\n",S,P,O);fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	uint pattern[MAX_ENTRIES];
	uint *res= *buffer; //no malloc performed here!!

	uint ss = mapID(g,S,SUBJECT);
	uint pp = mapID(g,P,PREDICATE);
	uint oo = mapID(g,O,OBJECT);  // uint x=unmapID(g,ti,TIME_INI);
	
	pattern[0] = ss; pattern[1] = pp; pattern[2] = oo;
	ulong numocc,l,r;
	countIntIndex(g->myicsa, pattern , 3, &numocc, &l, &r);
	
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
			res_s[n] = getRankicsa(g->myicsa,i) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);

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
			
			#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_s[n] != S) ||  (res_p[n] != P) ||  (res_o[n] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u,%u>\n", 
				                                               res_s[n], res_p[n], res_o[n],S,P,O );fflush (stdout);
				exit(0);
			}
			#endif			
			
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
			res_s[n] = getRankicsa(g->myicsa,i) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);

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
			
			#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_s[n] != S) ||  (res_p[n] != P) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,%u, \?>\n", 
				                                               res_s[n], res_p[n], res_o[n],S,P );fflush (stdout);
				exit(0);
			}
			#endif			
			
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
			
			//
/*			printf("\n PSI_WITH_BUFFER[]= ");

			for (i=l; i<=r;i++) {
				printf("[%8u]",bufferpsi[i-l]);				
			}
			printf("\n PSI__NO__BUFFER[]= ");
			for (i=l; i<=r;i++) {
				printf("[%8u]",getPsiicsa(g->myicsa, i));				
			}
			printf("\n");fflush(stdout);
*/			//		
		#endif		
		

		for (i=l; i<=r;i++) {
			uint n,s,o; 
			n = res[0];
			res[0]++;

			// predicates
			res_p[n] = getRankicsa(g->myicsa,i) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);			

			// objects
			#ifdef BUFFER_PSI_ON
				o=bufferpsi[i-l]; //o= getPsiicsa(g->myicsa, i);				
			#else
				o= getPsiicsa(g->myicsa, i);
			#endif		

			res_o[n] = getRankicsa(g->myicsa,o) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);		

			// subjects
			s= getPsiicsa(g->myicsa, o);
			res_s[n] = getRankicsa(g->myicsa,s) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);


//			printf("\n Retrieved triplet  <s,p,o> = <%u,%u,%u> ", res_s[n], res_p[n], res_o[n]);fflush (stdout);

			
			#ifndef EXPERIMENTS
//			#ifdef EXPERIMENTS
			//checks results are "expected"
			if ( (res_p[n] != P) ||  (res_o[n] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <\?, %u,%u>\n", 
				                                               res_s[n], res_p[n], res_o[n],P,O );fflush (stdout);
				//exit(0);
			}
			#endif			
			
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
			res_o[n] = getRankicsa(g->myicsa,i) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);		

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
			
			#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_s[n] != S) ||  (res_o[n] != O) ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u, \? ,%u>\n", 
				                                               res_s[n], res_p[n], res_o[n],S,O );fflush (stdout);
				exit(0);
			}
			#endif			
			
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
			res_s[n] = getRankicsa(g->myicsa,i) -1;
			res_s[n] = unmapID(g,res_s[n],SUBJECT);

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
			
			#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_s[n] != S)  ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <%u,\?, \?>\n", 
				                                               res_s[n], res_p[n], res_o[n],S );fflush (stdout);
				exit(0);
			}
			#endif			
			
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
			res_p[n] = getRankicsa(g->myicsa,i) -1;
			res_p[n] = unmapID(g,res_p[n],PREDICATE);

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
			
			#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_p[n] != P)  ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <\?, %u, \?>\n", 
				                                               res_s[n], res_p[n], res_o[n],P );fflush (stdout);
				exit(0);
			}
			#endif			
			
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
			res_o[n] = getRankicsa(g->myicsa,i) -1;
			res_o[n] = unmapID(g,res_o[n],OBJECT);
			
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
			
			
			#ifndef EXPERIMENTS
			//checks results are "expected"
			if ( (res_o[n] != O)  ) {
				printf("\n\n Retrieved triplet failed: <s,p,o> = <%u,%u,%u> and should be  <\?, \?, %u>\n", 
				                                               res_s[n], res_p[n], res_o[n], O );fflush (stdout);
				exit(0);
			}
			#endif			
			
		}

	#ifdef BUFFER_PSI_ON
			my_free_array(bufferpsi);
	#endif
	
	}
	return res[0];
}	

/*************************************************************************/

