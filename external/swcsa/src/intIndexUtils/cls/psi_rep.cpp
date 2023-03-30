

#include "psi_rep.h"

unsigned long   psi_rep::getPsiTotalMem() { 
  return totalMem;
}



psi_rep * psi_rep::psi_load(char * filename) {
	
  uint r;
  
  FILE *fp = fopen(filename,"r");
  if (fp == NULL) {return NULL;}
    
  if(fread(&r,sizeof(uint),1,fp)!=1) return NULL;
  fclose(fp);
  switch(r) {
	case 0: return NULL;
    case PSIREP_PLAIN:      	return psi_rep_plain::psi_load(filename);
   // case PSIREP_PLAIN_UINT: return psi_rep_plain_uint::psi_load(filename);
    case PSIREP_PLAIN_RLE:  	return psi_rep_plain_rle::psi_load(filename);
    case PSIREP_HUFFMAN_RLE:   	return psi_rep_huffman_rle::psi_load(filename);
    case PSIREP_HYB3R:      	return psi_rep_hyb3r::psi_load(filename);
  }

  return NULL;
}





//// protected function
//void psi_rep::setTotalMem(size_t totalMem) {
//	this->totalMem = totalMem;
//}

