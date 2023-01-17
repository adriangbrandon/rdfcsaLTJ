
long  vvv  (void *gindex,  uint **buf) {
	//	printf("\n@@@@@@@@@@@@@@@@ call to operation VVV \n");fflush (stdout);

	twcsa *g = (twcsa *)gindex;
	size_t len;
	size_t i,j,z=0;
	uint * buffer;
	dumpICSASequence(g->myicsa, &buffer, &len);
	for (i=0; i<g->n ; i++) {
		for (j=0;j<g->nEntries;j++) {
			buffer[z] = unmapID(g,buffer[z],j);
			z++;
		}		
	}
	*buf = buffer;
	return g->n;
	
}	




//void dumpSourceData(twcsa *wcsa, uint **data, size_t *len) {
//	uint * buffer;
//	dumpICSASequence(wcsa->myicsa, &buffer, len);
//
//	size_t i,j,z=0;
//	for (i=0; i<wcsa->n; i++){
//		for (j=0;j<wcsa->nEntries;j++) {
//			//buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
//			buffer[z] = unmapID(wcsa,buffer[z],j);
//			z++;
//		}
//	}		
//	*data = buffer;
//	*len = *len -1;
//}
