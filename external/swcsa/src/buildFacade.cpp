


/* buildFacade.c
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * BuildFacade.c: Implementation of the functions that the word-based self-index
 *   must implement following the "../pizza/interface.h" guidelines.
 *
 * See more details in:
 * Antonio Fariña, Nieves Brisaboa, Gonzalo Navarro, Francisco Claude, Ángeles Places, 
 * and Eduardo Rodríguez. Word-based Self-Indexes for Natural Language Text. ACM 
 * Transactions on Information Systems (TOIS), 2012. 
 * http://vios.dc.fi.udc.es/indexing/wsi/publications.html
 * http://www.dcc.uchile.cl/~gnavarro/ps/tois11.pdf	  
 * 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "buildFacade.h"
#include "utils/errors.c"
#include "graphReader.c"

// typedef struct {
// 	twcsa *spo;
// 	twcsa *ops;
// } tdualrdfcsa;
// 


	
/** ******************************************************************************
    * INI functions in Interface dual RDFCSA spo & ops.
*********************************************************************************/
#include "buildFacadeGraphOPS.c"


void testAdrian(uint32_t a, uint32_t b) {
		printf("\nAcabo de sumar %u + %u = %u\n", a,b, a+b);
		std::vector <uint> resultado = get_all_dual(NULL, PREDICATE);
		printf("\n resultado de get_all_dual () --> %u enteiros", (uint) resultado.size());
}




//Creates a ltf-iterator with none variables set.
//Returs a void * pointer to the created t_iterator structure.
void * createIterator_dual(void *index) {
	//titerator * createIterator(void *index);
	// inicializa os 2 rangos con [1,n], n=numero de triples.

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	size_t n = dualrdf->spo->n; //number of triples (including dummie).    
	size_t nE = dualrdf->spo->nEntries;  //numberofTriples
					//rdfcsa 3 ranges==> [0 .. n-1][n .. 2n-1][2n .. 3n-1]
								

	t_iterator *it = (t_iterator *) malloc (sizeof(t_iterator)*1);
	it->nFixed=0;
		//it->typeFixed[0]=0;
		//it->valueFixed[0]=0;
	it->range[0]=0; it->range[1]=n*nE -1; it->range[2]=0; it->range[3]=n*nE -1;

		memcpy(it->range_backup, it->range, 4*sizeof(uint));  //DONE ONLY WITHIN DOWN()
	
	it->dualrdfcsa = index;
	
	return it;
}


	char *printTypeFixed(uchar *typeFixed, uint nFixed);  //used in printIterator_dual()
	char *printValueFixed(uint *valueFixed, uint nFixed); //used in printIterator_dual()
	char *printActive(uchar *typeFixed, uint nFixed);     //used in printIterator_dual()
	
void printIterator_dual(void * iterator) {
	t_iterator *it = (t_iterator *) iterator;
	setColorAmarillo();
	printf("\n\n\t printing iterator info: "); setColorNormal();
	printf("\n\t - nFixed = %u", it->nFixed);
	printf("\n\t - typesFixed  => %s", printTypeFixed(it->typeFixed,it->nFixed));
	printf("\n\t - valuesFixed => %s", printValueFixed(it->valueFixed,it->nFixed));	

	printf("\n\t - range_backup  ==>  [%u,%u] [%u,%u]",it->range_backup[0],it->range_backup[1],it->range_backup[2],it->range_backup[3]);
	printf("\n\t - range         ==>  [%u,%u] [%u,%u]",it->range[0],it->range[1],it->range[2],it->range[3]);

	printf("\n\t - active rdfcsa(s)   => %s",     printActive (it->typeFixed,it->nFixed));
	
		{ //shows additional info 
			printf("\n\t\t@ ---- INI PRINTING TRIPLES around ranges[i]-----:");				
		
			if(it->range[0] != 0) {  // it is initialized
				printf("\n\t\t@ SPO-----:");
				printf("\n\t\t\t  -3 Triples at pos x-1, x=%u, x+1 are ::", it->range[0]);   
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->spo,it->range[0]-1); printf("  *");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->spo,it->range[0]);   printf("*  ");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->spo,it->range[0]+1); 
				printf("\n\t\t\t  -3 Triples at pos x-1, x=%u, x+1 are ::", it->range[1]);   
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->spo,it->range[1]-1); printf("  *");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->spo,it->range[1]);   printf("*  ");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->spo,it->range[1]+1);
			}

			if(it->range[2] != 0) {  // it is initialized
				printf("\n\t\t@ OPS-----:");                                                 
				printf("\n\t\t\t  -3 Triples at pos x-1, x=%u, x+1 are ::", it->range[2]);   
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->ops,it->range[2]-1); printf("  *");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->ops,it->range[2]);   printf("*  ");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->ops,it->range[2]+1); 
				printf("\n\t\t\t  -3 Triples at pos x-1, x=%u, x+1 are ::", it->range[3]);   
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->ops,it->range[3]-1); printf("  *");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->ops,it->range[3]);   printf("*  ");
					dual_printTriple( ((tdualrdfcsa *) it->dualrdfcsa)->ops,it->range[3]+1);
				
			}
			printf("\n\t\t@ ---- END PRINTING TRIPLES around ranges[i]-----\n");
		}	

}


//Makes a copy of the ltf-iterator.
//Returs a void * pointer to a new t_iterator structure.
void * cloneIterador_dual(void* iteratorsrc) {
	//básicamente facer un memcpy (sizeof(structura iterador) ...).
	//returns:
		//una copia del iteradorsrc.
		//malloc (sizeof(structura)
		//memcpy (...)
	//Se necesita xq  Adrián podría necesitar clonar los iteradores...
	
	t_iterator *itsrc = (t_iterator *) iteratorsrc;
	t_iterator *it    = (t_iterator *) malloc (sizeof(t_iterator)*1);
	memcpy(it,itsrc,sizeof(t_iterator)*1);
	
	return it;	
}





int init_spo_dual(void *iterator, uint s, uint p, uint o) {
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamén valería)
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3


	//valid or not valid s,p,o
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,SUBJECT,s)) return 0;
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,PREDICATE,p)) return 0;
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,OBJECT,o)) return 0;


	uint left,right;
	
	if (! dual_rdfcsaSPO_init_spo( dualrdf->spo, s,p,o, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	
	it->range[0]= left; it->range[1]= right;    //uset SPO
	it->range[2]= 0;    it->range[3]= n*nE -1;  //notUsed OPS yet initialized to avoid valgrind 
												//warnings in memcpy's to range_backup
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()
	
	it->nFixed=3;
	it->typeFixed[0]=SUBJECT;		it->valueFixed[0] = s;
	it->typeFixed[1]=PREDICATE;		it->valueFixed[1] = p;
	it->typeFixed[2]=OBJECT;		it->valueFixed[2] = o;
			
	return 1;
}
	
int init_so_dual(void *iterator, uint s, uint o) {
	//inicializa rdfcsa-ops
	//inicializa rdfcsa-spo non se usa. (tamen serviría se considero "O->S"... porque despois só podo facer down con P).
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3


	//valid or not valid s,o
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,SUBJECT,s)) return 0;
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,OBJECT,o)) return 0;


	uint left,right;

	if (! dual_rdfcsaOPS_init_so( dualrdf->ops, s,o, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	
	it->range[2]= left; it->range[3]= right;    //used OPS
	it->range[0]= 0;    it->range[1]= n*nE -1;  //notUsed SPO yet initialized to avoid valgrind 
												//warnings in memcpy's to range_backup
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()
	
//	/**FALTARÍA APLICAR PSI a los extremos del intervalo y actualizarlo en range[2..3], para saltar al rango de los objetos**/
//	/*done*/ left  = getPsiicsa(dualrdf->ops->myicsa,left);
//	/*done*/ right = getPsiicsa(dualrdf->ops->myicsa,right);
//	/*done*/ it->range[2]= left; it->range[3]= right; 
	
	it->nFixed=2;
	it->typeFixed[0]=SUBJECT;		it->valueFixed[0] = s;
	it->typeFixed[1]=OBJECT;		it->valueFixed[1] = o;
	
	return 1;
}

int init_sp_dual(void *iterator, uint s, uint p){
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamen serviría se considero "P->S"... porque despois só podo facer down con o).
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3

	uint left,right;

	//valid or not valid s,p
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,SUBJECT,s)) return 0;
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,PREDICATE,p)) return 0;

	
	if (! dual_rdfcsaSPO_init_sp( dualrdf->spo, s,p, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	
	it->range[0]= left; it->range[1]= right;    //used SPO
	it->range[2]= 0;    it->range[3]= n*nE -1;  //notUsed OPS yet initialized to avoid valgrind 
												//warnings in memcpy's to range_backup
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()
	
//	/**FALTARÍA APLICAR PSI a los extremos del intervalo y actualizarlo en range[0..1], para saltar al rango de los predicados**/
//	/*done*/ left  = getPsiicsa(dualrdf->spo->myicsa,left);
//	/*done*/ right = getPsiicsa(dualrdf->spo->myicsa,right);
//	/*done*/ it->range[0]= left; it->range[1]= right; 
//	
	
	it->nFixed=2;
	it->typeFixed[0]=SUBJECT;		it->valueFixed[0] = s;
	it->typeFixed[1]=PREDICATE;		it->valueFixed[1] = p;
			
	return 1;
}
	
int init_po_dual(void *iterator, uint p, uint o){
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamen serviría se considero "O->P"... porque despois só podo facer down con s).
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	
	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3


	//valid or not valid p,o
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,PREDICATE,p)) return 0;
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,OBJECT,o)) return 0;


	uint left,right;
	
	if (! dual_rdfcsaSPO_init_po( dualrdf->spo, p,o, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	it->range[0]= left; it->range[1]= right;    //uset SPO
	it->range[2]= 0;    it->range[3]= n*nE -1;  //notUsed OPS yet initialized to avoid valgrind 
												//warnings in memcpy's to range_backup
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()

//	/**FALTARÍA APLICAR PSI a los extremos del intervalo y actualizarlo en range[0..1], para saltar al rango de los objetos**/
//	/*done*/ left  = getPsiicsa(dualrdf->spo->myicsa,left);
//	/*done*/ right = getPsiicsa(dualrdf->spo->myicsa,right);
//	/*done*/ it->range[0]= left; it->range[1]= right; 
//		
	it->nFixed=2;
	it->typeFixed[0]=PREDICATE;		it->valueFixed[0] = p;
	it->typeFixed[1]=OBJECT;		it->valueFixed[1] = o;
			
	return 1;
}
	
int init_s_dual(void *iterator, uint s){
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar p ou o
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3

	//valid or not valid s
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,SUBJECT,s)) return 0;


	uint left,right;
	
	if (! dual_rdfcsaSPO_init_s( dualrdf->spo, s, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	
	it->range[0]= left; it->range[1]= right;    //used SPO
	
	if (! dual_rdfcsaOPS_init_s( dualrdf->ops, s, &left, &right)) {
		//should not enter here, because "s" was found above in the initialization of SPO rdfcsa.
		return 0;
	}
	it->range[2]= left;    it->range[3]= right; //used also OPS
												
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()
	
	it->nFixed=1;
	it->typeFixed[0]=SUBJECT;		it->valueFixed[0] = s;
			
	return 1;
}
	
int init_p_dual(void *iterator, uint p){
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar s ou o
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3

	//valid or not valid p
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,PREDICATE,p)) return 0;


	uint left,right;
	
	if (! dual_rdfcsaSPO_init_p( dualrdf->spo, p, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	
	it->range[0]= left; it->range[1]= right;    //used SPO
	
	if (! dual_rdfcsaOPS_init_p( dualrdf->ops, p, &left, &right)) {
		//should not enter here, because "p" was found above in the initialization of SPO rdfcsa.
		return 0;
	}
	it->range[2]= left;    it->range[3]= right; //used also OPS
												
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()
	
	it->nFixed=1;
	it->typeFixed[0]=PREDICATE;		it->valueFixed[0] = p;
			
	return 1;
}
	
int init_o_dual(void *iterator, uint o){
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar s ou p
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples
	size_t nE = dualrdf->spo->nEntries;  //numberofEntries per triple = 3

	//valid or not valid o
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,OBJECT,o)) return 0;

	uint left,right;
	
	if (! dual_rdfcsaSPO_init_o( dualrdf->spo, o, &left, &right)) {
		//DOES NOTHING with the fields of the iterator
		//             ... this iterator will no longer be used.
		
		return 0;
	}
	
	it->range[0]= left; it->range[1]= right;    //used SPO
	
	if (! dual_rdfcsaOPS_init_o( dualrdf->ops, o, &left, &right)) {
		//should not enter here, because "o" was found above in the initialization of SPO rdfcsa.
		return 0;
	}
	it->range[2]= left;    it->range[3]= right; //used also OPS
												
	
	/*TO-COMMENT*/ memcpy(it->range_backup, it->range, 4*sizeof(uint)); //DONE ONLY WITHIN DOWN()
	
	it->nFixed=1;
	it->typeFixed[0]=OBJECT;		it->valueFixed[0] = o;
			
	return 1;	
}





//...
uint leap_dual(void *iterator, int type, uint value) {
	//type: indica se estamos a buscar nun s, p ou o
	//buscamos no rango do iterator o primeiro valor de tipo (type) >= value
	//OLLO: hai que controlar o seguinte:
	// - se se chama sen nada fijado --> devolver o primeiro valor dese tipo >= value  
	//
	// - si tipo = K devolver:  si (min-value <= value <= max-value de ese tipo K) then value, else 0;
	// - se hai soamente unha variable instanciada -> podese usar calquera dos dous rangos (rdfcsa-spo e rdfcsa-ops), usar un ou outro depende do (tipo), o que sexa só un salto.
	// - se hai duas variables instanciadas -> sabese que hai solo un rango activo (ou ben rdfcsa-spo ou rdfcsa-ops)
	// - NUNCA se vai chamar coas tres instanciadas

    //-- leap devolve CERO se non se atopa.

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	size_t n = dualrdf->spo->n;          //numberofTriples

		uint *left;  uint right;  //source range for fixed variable according to it->range[...]
		ulong tl, tr; 			    //target range for "value" of type "type" 
		uint retval = 0;		
		twcsa *g;


	/**OJO ADRIAN...  TEÑO QUE CONTROLAR QUE EXISTA value ? dese tipo (ENTENDO QUE NON) **/
	/** SE SI --> crear funcion int/bool dual_isValidValue(twcsa *g, int type, uint value);
	 **           que mire o rango no vocabulario de 
	 * 			                predicados [1,ns]
	 * 			                SPO ou OPS (indistinto): sujetos [1,ns], objetos [gapobjects, gapobjects+no-1] ?
	 **/ 

	if ((type ==OBJECT) && (value <= dualrdf->spo->gapobjects)) value = 1+dualrdf->spo->gapobjects;
	if (! dual_isValidValue_for_Type_in_Input(dualrdf->spo,type,value)) return 0;
	
	/****/
	

	/* 0 fixed variables */
	if (it->nFixed ==0) {
		// if (type == SUBJECT) return 1;                           //value of the 1st subject   in the source data
		// else if (type == PREDICATE) return 1;                    //value of the 1st predicate in the source data 
		// if (type == OBJECT) return 1 + dualrdf->spo->gapobjects; //value of the 1st object    in the source data
		                               //recall dualrdf->spo->gapobjects == dualrdf->ops->gapobjects   

		return value;  
	}

		/* initial part :: 1 or 2 fixed variables */
		// depending on it->nFixed and it->typeFixed[0,1]
		// sets g to              dualrdf->spo   or dualrdf->ops
		//      left and right to it->range[0,1] or it->range[2,3], accordingly
	
	/* 1 fixed variable */
	else if (it->nFixed ==1) {
		
		if(it->typeFixed[0] ==SUBJECT) {
			if (type ==PREDICATE) {
					/**/ //usar dualrdf->spo para buscar PREDICADO "value": salto (S->P) 
					g=dualrdf->spo;
					left = &it->range[0]; right = it->range[1];
					//see-below:
					//dual_getRangeLR_for_type_and_value (dualrdf->spo, type, value, &tl, &tr);  
					//retval = dual_searchPsiTarget_to_leap (dualrdf->spo, &it->range[0],it->range[1], tl,tr);  
			}
			else {  //type = OBJECT
					/**/ //usar dualrdf->ops para buscar SUJETO "value": salto (S->O) 
					g=dualrdf->ops;
					left = &it->range[2]; right = it->range[3];				
			}
		}
		
		else
		if(it->typeFixed[0] ==PREDICATE) {
			if (type ==SUBJECT) {
					/**/ //usar dualrdf->ops para buscar SUJETO "value": salto (P->S) 
					g=dualrdf->ops;
					left = &it->range[2]; right = it->range[3];								
			}
			else {  //type = OBJECT
					/**/ //usar dualrdf->spo para buscar OBJETO "value": salto (P->0) 
					g=dualrdf->spo;
					left = &it->range[0]; right = it->range[1];				
			}
		}
		
		else {  //it->typeFixed[0] == OBJECT
			if (type ==SUBJECT) {
					/**/ //usar dualrdf->spo para buscar SUJETO "value": salto (O->S) 
					g=dualrdf->spo;
					left = &it->range[0]; right = it->range[1];					
			}
			else {  //type = PREDICATE
					/**/ //usar dualrdf->ops para buscar PREDICADO "value": salto (O->P) 
					g=dualrdf->ops;
					left = &it->range[2]; right = it->range[3];												
			}
		}		
		
		//1. computes target range [tl,tr] for value of type
		//dual_getRangeLR_for_type_and_value (g, type, value, &tl, &tr);  //target range for value - using conceptually 2 bitmap-selects
		
		dual_getRangeLR_for_type_and_GEQvalue (g, type, value, &tl, &tr);  //target range for value - using conceptually 2 
		
		//setColorMorado(); printf("\n target-left = %lu, target-right = %lu ",tl,tr); setColorNormal();


		//2. recovers the x=leap() value, and x>0 it also shortens the source interval (increases the left of the range)
					ulong longleft = *left;  		//TRICK-ini : copies *left (32bit value) to a ulong (64bit) variable "longleft"
<<<<<<< HEAD
		retval = dual_searchPsiTarget_to_leap (dualrdf->spo, &longleft, (ulong) right, tl,tr);  
=======
/** 	retval = dual_searchPsiTarget_to_leap (dualrdf->spo, &longleft, (ulong) right, tl,tr);  */
		retval = dual_searchPsiTarget_to_leap ( g,           &longleft, (ulong) right, tl,tr);  
>>>>>>> 5af18d1ce0456ea71be7973990110962d4669f5f
					//*left = longleft;	     		//TRICK-ends: restores posibly modified *left from the ulong variable "longleft"
				
		//recall that if retval>0 then "left" = (range[0] ir range [2]) was also updated (actually increased)..	
		return retval;
				
	}
	
	/* 2 fixed variables */
	else { // it->nFixed ==2

		if(it->typeFixed[0]==SUBJECT) {
			if (it->typeFixed[1]==PREDICATE) {
					/**/ //usar dualrdf->spo para buscar OBJETO "value": salto (S-P ->O) 
					g=dualrdf->spo;
					left = &it->range[0]; right = it->range[1];					
			}
			else { //it->typeFixed[1]==OBJECT
					/**/ //usar dualrdf->ops para buscar PREDICADO "value": salto (S-O ->P) 
					g=dualrdf->ops;
					left = &it->range[2]; right = it->range[3];										
			}				
		}		

		else
		if(it->typeFixed[0] ==PREDICATE) {
			if (it->typeFixed[1]==SUBJECT) {
					/**/ //usar dualrdf->ops para buscar OBJETO "value": salto (P-S ->O) 
					g=dualrdf->ops;
					left = &it->range[2]; right = it->range[3];					
			}
			else { //it->typeFixed[1]==OBJECT
					/**/ //usar dualrdf->spo para buscar SUBJECT "value": salto (P-O ->S) 
					g=dualrdf->spo;
					left = &it->range[0]; right = it->range[1];										
			}				
		}		

		else {  //it->typeFixed[0] == OBJECT
			if (it->typeFixed[1]==SUBJECT) {
					/**/ //usar dualrdf->spo para buscar SUJETO "value": salto (O-S ->P) 
					g=dualrdf->spo;
					left = &it->range[0]; right = it->range[1];					
			}
			else {  //it->typeFixed[1]==PREDICATE
					/**/ //usar dualrdf->ops para buscar PREDICADO "value": salto (O-P ->S) 
					g=dualrdf->ops;
					left = &it->range[2]; right = it->range[3];												
			}
		}
		/* final part :: 1 or 2 fixed variables */
			
			//1. computes target range [tl,tr] for value of type
			//dual_getRangeLR_for_type_and_value (g, type, value, &tl, &tr);  //target range for value - using conceptually 2 bitmap-selects
			
			dual_getRangeLR_for_type_and_GEQvalue (g, type, value, &tl, &tr);  //target range for value - using conceptually 2 


			//2. recovers the x=leap() value, and x>0 it also shortens the source interval (increases the left of the range)
						ulong longleft = *left;  		//TRICK-ini : copies *left (32bit value) to a ulong (64bit) variable "longleft"
	   /** retval = dual_searchPsiPsiTarget_to_leap (dualrdf->spo, &longleft, (ulong) right, tl,tr);  **/
		   retval = dual_searchPsiPsiTarget_to_leap ( g,           &longleft, (ulong) right, tl,tr);  
			
					//	*left = longleft;	     		//TRICK-ends: restores posibly modified *left from the ulong variable "longleft"
					
			//recall that if retval>0 then "left" = (range[0] ir range [2]) was also updated (actually increased)..	
			
			return retval;
	}

}





int down_dual(void *iterator, int type, uint value){       //numfijadas ++ e actualizar rangos
	//type: indica que vamos a instanciar un s, p ou o
	//restrinximos o rango do iterator con value
	//OLLO: hai que controlar o seguinte:
	// - se non se restrinxiu nada previamente -> restrinxir os dous rangos (rdfcsa-spo e rdfcsa-ops)  --> podo charmar ao ini_X_dual correspondente.
	// - se hai unha variable instanciada previamente -> restrinxir soamente un rango (rdfcsa-spo ou rdfcsa-ops)
	// - se hai duas variables instanciadas -> non facer nada cos rangos, pero (xa estariamos no ultimo nivel do trie virtual)
	                     // -- backup rango = rangos_actuais, e non modificado rangos actuais.  (así evito if ao facer un posterior up()).
	                     // -- non facer nada cos rangos significa que non teño que actualizar o rango (pq xa sei que o novo valor é value), pero
	                     // -- si que hai que facer numfijadas ++ e gardar (type) en rango[2], por se as moscas;
	                     
				//PODE SER QUE ME CHAMEN CON Duas variables instanciadas, pero non fago nada.
				//NON PODE SER QUE chamen a down cun valor que non se poida atopar no rango actual fixado (porque Adrián chama antes a x=leap(), e leap devolvería 0,  e xa non chamaría a down)

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	//size_t n = dualrdf->spo->n;          //numberofTriples
	
		uint *left;  uint *right;   //source range for fixed variable according to it->range[...]
		ulong tl, tr; 			    //target range for "value" of type "type" 
		uint retval = 0;		
		twcsa *g;

	/* 0 fixed variables */
	if (it->nFixed ==0) {
		if (type == SUBJECT) {
			return init_s_dual(iterator,value);
		}
		else if (type == PREDICATE) {
			return init_p_dual(iterator,value);
		}
		else { //(type == OBJECT) 
			return init_o_dual(iterator,value);
		}
	}	
	
	/* 1 fixed variable */
	else if (it->nFixed ==1) {
		
		/* initial part: 1 fixed variable */
		// depending on it->typeFixed[0]
		// sets g to              dualrdf->spo   or dualrdf->ops
		//      left and right to it->range[0,1] or it->range[2,3], accordingly

		if(it->typeFixed[0] ==SUBJECT) {
			if (type ==PREDICATE) {
					/**/ //usar dualrdf->spo para buscar PREDICADO "value": salto (S->P) 
					g=dualrdf->spo;
					left = &it->range[0]; right = &it->range[1];
					//see-below:
					//dual_getRangeLR_for_type_and_value (dualrdf->spo, type, value, &tl, &tr);  
					//retval = dual_searchPsiTarget_to_leap (dualrdf->spo, &it->range[0],it->range[1], tl,tr);  
			}
			else {  //type = OBJECT
					/**/ //usar dualrdf->ops para buscar SUJETO "value": salto (S->O) 
					g=dualrdf->ops;
					left = &it->range[2]; right = &it->range[3];				
			}
		}
		
		else
		if(it->typeFixed[0] ==PREDICATE) {
			if (type ==SUBJECT) {
					/**/ //usar dualrdf->ops para buscar SUJETO "value": salto (P->S) 
					g=dualrdf->ops;
					left = &it->range[2]; right = &it->range[3];								
			}
			else {  //type = OBJECT
					/**/ //usar dualrdf->spo para buscar OBJETO "value": salto (P->0) 
					g=dualrdf->spo;
					left = &it->range[0]; right = &it->range[1];				
			}
		}
		
		else {  //it->typeFixed[0] == OBJECT
			if (type ==SUBJECT) {
					/**/ //usar dualrdf->spo para buscar SUJETO "value": salto (O->S) 
					g=dualrdf->spo;
					left = &it->range[0]; right = &it->range[1];					
			}
			else {  //type = PREDICATE
					/**/ //usar dualrdf->ops para buscar PREDICADO "value": salto (O->P) 
					g=dualrdf->ops;
					left = &it->range[2]; right = &it->range[3];												
			}
		}		
		
		memcpy(it->range_backup, it->range, 4*sizeof(uint)); 
		it->nFixed++;
		
		it->typeFixed[1] = type;
		it->valueFixed[1]=value;
		
		/* final part */
			//1. computes target range [tl,tr] for value of type
		dual_getRangeLR_for_type_and_value (g, type, value, &tl, &tr);  //target range for value - using conceptually 2 bitmap-selects

			//2. now we shorten the source interval 
					ulong longleft = *left;  		//TRICK-ini : copies *left  (32bit value) to a ulong (64bit) variable "longleft"
					ulong longright = *right;  		//TRICK-ini : copies *right (32bit value) to a ulong (64bit) variable "longright"
		/** retval = dual_searchPsiTarget_to_down (dualrdf->spo, &longleft, &longright, tl,tr);  */
		retval = dual_searchPsiTarget_to_down (g, &longleft, &longright, tl,tr);  
					*left  = longleft;	     		//TRICK-ends: restores ??modified?? *left from the ulong variable "longleft"
					*right = longright;	     		//TRICK-ends: restores modified *right from the ulong variable "longright"
				
		return retval;  //1 WHATEVER, return value is not actually used.
	}

	/* 2 fixed variables */
	else { // it->nFixed ==2
		it->nFixed++;		
		it->typeFixed[2]  = type;
		it->valueFixed[2] = value;		
		//ranges not modified
	}
	
	return 1;
}



int up_dual(void *iterator) {                           //numfijadas -- e actualizar rangos
	//subimos no trie virtual. Os rangos do iterator volven ao estado anterior a facer down_dual.
	//OLLO: hai que controlar o seguinte:
	// - NUNCA se vai chamar se non se restrinxiu nada previamente  **fari: non o teño que controlar**
	// - Si hai variables fijadas: facer numfijadas --, e restaurar backup rangos.
	
	//CREO QUE NON FAI FALLA// - se hai unha variable instanciada previamente -> os dous rangos volven ao estado anterior (rdfcsa-spo ou rdfcsa-ops).
	//CREO QUE NON FAI FALLA//   NOTA: estado inicial nos dous (non hai nada instanciado).  **fari: os 2 quedan 1-n, como cando está creado o iterador, pero sen chamar a init*_dual **
	//CREO QUE NON FAI FALLA// - se hai duas variables instanciadas -> **fari: ollo: podo estar cun único rango activo, e ao volver atrás, pasar a ter 2 activos de novo**
	//CREO QUE NON FAI FALLA//   NOTA: realmente so un deles se modificou no down. Ex. asume que temos so instanciados:
	//CREO QUE NON FAI FALLA//   (rdfcsa-spo) teremos o rango restrinxido con s
	//CREO QUE NON FAI FALLA//   (rdfcsa-ops) teremos o rango restrinxido con s e o, porque conten o orden sop
	//CREO QUE NON FAI FALLA//	  Polo que soamente habería que cambiar o rango de rdfcsa-ops a que quede restrinxido por s (quitar a restriccion de o)
	//CREO QUE NON FAI FALLA// - se hai tres variables instanciadas -> recupero backup e fago numfijadas --;

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;	
	
	//	if (it->nFixed ==1) {
	//		it->nFixed =0;
	//	}
	//	if (it->nFixed ==2) {
	//		it->nFixed =1;
	//		memcpy(it->range, it->range_backup, 4*sizeof(uint)); 
	//	}
	//	if (it->nFixed ==3) {
	//		it->nFixed =2;
	//	}


	/* borrar esto para experimentos*/	if (it->nFixed == 0) {  
	/* borrar esto para experimentos*/		/*TO-COMMENT*/
	/* borrar esto para experimentos*/		printf("\n UPS!: alguien ha llamado a up() con 0 variables fijadas \n"); exit(0);
	/* borrar esto para experimentos*/	}
	
	if (it->nFixed ==2) {
		memcpy(it->range, it->range_backup, 4*sizeof(uint)); 
	}	
	
	it->nFixed--;
	return 0;
}



// para un iterador, devolver o tamaño mínimo dos rangos que ten restrinxidos. --- ou algo así ... a definir con Adrián.
// se están os 2 activos --> o menor
// se só hai un activo --> ese.
// ** sácase a partir dos rangos e facendo ifs ... ** isto está sen definir aínda.
uint get_range_length_dual (void *iterator) {
	// devolver o tamaño do rango que esté definido
	// - se hai cero variables instanciadas -> [1, n] (n e o numero de triples)
	// - se hai unha variable instanciada -> devolver r-l+1 (en calquera dos 2 rangos activos).
	// - se hai dúas variables instanciadas -> devolver r-l+1 no rango que esté activo.
	// - se hai tres variables instanciadas - nunca se vai chamar pero -> devolver 1, por se as moscas (otherwise). ;)

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;
	size_t n = dualrdf->spo->n;          //numberofTriples	

	if (it->nFixed ==0) {
		return 3*n;
	}
	else if (it->nFixed ==1) {
		return (it->range[1]- it->range[0] +1);   //equals to -->	ulong len_ops = it->range[3]- it->range[2] +1;		
	}
	else if (it->nFixed ==2) {
		ulong len1 = it->range[1]- it->range[0] +1;   
		ulong len2 = it->range[3]- it->range[2] +1;
		ulong min_len=  (len1<len2) ? len1 : len2;
		return min_len;
	}	
	
	return 1 ; //3 values fixed --> range-lenght == 1	
}


// para un iterador.
int is_last_level_dual(void *iterator) {
	//returns 1 se hai >=2 variables fixadas   numFijadas >=2
	//returns 0 otherwise (se hai 0 o 1 variables fixadas)

	t_iterator *it = (t_iterator *) iterator;
	
	if (it->nFixed >=2) return 1;

	return 0;
}

//  Dado un iterador que tiene 2 variables fijadas (is_last_level() devolvería 1).
//  devuelve un array de IDs de elementos de tipo "type" del elemento "no-fijado" asociado a ese rango.
//  el tipo no sería necesario, pero Adrián me lo pasa para que pueda verificar que coincide con el tipo del elemento NO-FIJADO. 
// (lo quitaremos para las pruebas).
std::vector <uint> get_all_dual (void *iterator, int type) {

	std::vector <uint> res;

/* borrar esto para experimentos*/	if (iterator ==NULL) return res;

	t_iterator *it = (t_iterator *) iterator;
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) it->dualrdfcsa;
	size_t n = dualrdf->spo->n;          //numberofTriples	
	
	if (it->nFixed ==2) {
		twcsa *g;
		ulong left, right;
		uint expectedtype;

		if(it->typeFixed[0]==SUBJECT) {
			if (it->typeFixed[1]==PREDICATE) {
					/**/ //usar dualrdf->spo para buscar OBJETO "value": salto (S-P ->O) 
					g=dualrdf->spo;
					left = it->range[0]; right = it->range[1];	expectedtype = OBJECT;
			}
			else { //it->typeFixed[1]==OBJECT
					/**/ //usar dualrdf->ops para buscar PREDICADO "value": salto (S-O ->P) 
					g=dualrdf->ops;
					left = it->range[2]; right = it->range[3]; 	expectedtype = PREDICATE;										
			}				
		}		

		else
		if(it->typeFixed[0] ==PREDICATE) {
			if (it->typeFixed[1]==SUBJECT) {
					/**/ //usar dualrdf->ops para buscar OBJETO "value": salto (P-S ->O) 
					g=dualrdf->ops;
					left = it->range[2]; right = it->range[3];	expectedtype = OBJECT;
			}
			else { //it->typeFixed[1]==OBJECT
					/**/ //usar dualrdf->spo para buscar SUBJECT "value": salto (P-O ->S) 
					g=dualrdf->spo;
					left = it->range[0]; right = it->range[1];	expectedtype = SUBJECT;	
			}				
		}		

		else {  //it->typeFixed[0] == OBJECT
			if (it->typeFixed[1]==SUBJECT) {
					/**/ //usar dualrdf->spo para buscar SUJETO "value": salto (O-S ->P) 
					g=dualrdf->spo;
					left = it->range[0]; right = it->range[1];	expectedtype = PREDICATE;
			}
			else {  //it->typeFixed[1]==PREDICATE
					/**/ //usar dualrdf->ops para buscar PREDICADO "value": salto (O-P ->S) 
					g=dualrdf->ops;
					left = it->range[2]; right = it->range[3];	expectedtype = SUBJECT;
			}
		}


	/* borrar esto para experimentos*/		if (type != expectedtype) {
	/* borrar esto para experimentos*/			printf("\n This should not happen in get_all_dual:");
	/* borrar esto para experimentos*/			printf("\n  type =%d but expected type = %u\n",type,expectedtype); exit(0);
	/* borrar esto para experimentos*/		}

		uint *bufferpsi = (uint *) my_malloc(sizeof(uint) * (right-left+1));
		getPsiBuffericsa(g->myicsa,bufferpsi,left,right);   //values of Psi(i) for i \in [left,right]
		
		uint i;
		uint pos; uint value;
		for (i=left; i<=right; i++) {
			pos=bufferpsi[i-left];
			pos = getPsiicsa(g->myicsa,pos);         //** now l,r is 2 ranges before than expectedtype --> pos = \psi(\psi(i-left))
			value = getRankicsa(g->myicsa,pos) -1;
			value = unmapID(g, value, type);
			
			res.push_back( value );			
		}
		free(bufferpsi);
	}

//  Adrian said we cannot call this function when nFixed =3 
//	if (it->nFixed ==3) {
//		return it->valueFixed[2];
//	}
//	
	return res;	  
	//Adrian, entendo que res é un vector valeiro, e ti miras o seu tamaño (por se pasas un nFixed !=2, e eu devolvo
	//        un vector sen facer ningún push_back();
}



//  //  Dado un iterador que tiene 2 variables fijadas (is_last_level() devolvería 1).
//  //  devuelve un array de IDs de elementos de tipo "type" del elemento "no-fijado" asociado a ese rango.
//  //  el tipo no sería necesario, pero Adrián me lo pasa para que pueda verificar que coincide con el tipo del elemento NO-FIJADO. 
//  // (lo quitaremos para las pruebas).
//  std::vector <uint> get_all_dual (void *iterator, int type)
//  {   std::vector <uint> res;
//  //    añadir a vector res -->    res.push_back( 3333);
//  	res.push_back(444);
//      return res;
//  }



	/**/ //private functions:: used in printIterator_dual() -- prototypes defined above	
	/**/	char *printTypeFixed(uchar *typeFixed, uint nFixed) {
	/**/		static char s[1000];
	/**/		s[0]='\0';
	/**/		
	/**/		if (!nFixed) return s;
	/**/		uint i;
	/**/		strcat(s,"[ ");
	/**/		for (i=0; i<nFixed; i++) {
	/**/			if (typeFixed[i] == SUBJECT)   strcat(s, "S");
	/**/			if (typeFixed[i] == PREDICATE) strcat(s, "P");
	/**/			if (typeFixed[i] == OBJECT)    strcat(s, "O");
	/**/			
	/**/			if ( i+1 != nFixed)strcat(s,", ");		
	/**/		}
	/**/		strcat(s,"]");
	/**/		return s;
	/**/	}
	/**/	
	/**/	char *printValueFixed(uint *valueFixed, uint nFixed) {
	/**/		static char s[1000];
	/**/		s[0]='\0';
	/**/		
	/**/		if (!nFixed) return s;
	/**/		uint i;
	/**/		strcat(s,"[ ");
	/**/		for (i=0; i<nFixed; i++) {
	/**/			char value[11];
	/**/			sprintf(value,"%u",valueFixed[i]);
	/**/			strcat (s,value);		
	/**/			if ( i+1 != nFixed)strcat(s,", ");		
	/**/		}
	/**/		strcat(s,"]");
	/**/		return s;
	/**/	}
	/**/	
	/**/	char *printActive(uchar *typeFixed, uint nFixed) {
	/**/		static char s[1000];
	/**/		uchar *tf = typeFixed;
	/**/		const char *spo = "*spo*";
	/**/		const char *ops = "*ops*";
	/**/		s[0]='\0';
	/**/		
	/**/		if (!nFixed) return s;
	/**/		
	/**/		if (nFixed ==1) sprintf(s,"%s - %s",spo,ops);
	/**/		else if (nFixed ==2) {
	/**/			if ( (tf[0] == SUBJECT)   && (tf[1] == OBJECT))    sprintf(s,"--- - %s",ops);
	/**/			if ( (tf[0] == SUBJECT)   && (tf[1] == PREDICATE)) sprintf(s,"%s - ---",spo);
	/**/			if ( (tf[0] == PREDICATE) && (tf[1] == OBJECT))    sprintf(s,"%s - ---",spo);
	/**/		}
	/**/		else sprintf(s,"%s - ---",spo); 
	/**/		
	/**/		return s;
	/**/	}










































/**********************************************************************/


#define FROM_HDT_PARSER_SOURCE_TYPE    1
#define FROM_ADRIAN_PARSER_SOURCE_TYPE 2


// returns 1, 2 
//     1: build_options does NOT include a param : sourceFormat=ADRIAN  (i.e. regular scripts from original)
//     2: build_options includes a param         : sourceFormat=ADRIAN: (i.e. source format for LTJ tests @ 2023).

int getSourceFileType(char *build_options) {
	char delimiters[] = " =;";
	int j,num_parameters;
	char ** parameters;

	int sourceFormatType = FROM_HDT_PARSER_SOURCE_TYPE;
	
	
	if (build_options != NULL) {
	parse_parameters(build_options,&num_parameters, &parameters, delimiters);
	for (j=0; j<num_parameters;j++) {
	  
		if ((strcmp(parameters[j], "sourceFormat") == 0 ) && (j < num_parameters-1) ) {
			if(strcmp(parameters[j+1], "ADRIAN") ==0) sourceFormatType =  FROM_ADRIAN_PARSER_SOURCE_TYPE;			
		} 	
		j++;
	}
	free_parameters(num_parameters, &parameters);
	}		

	return sourceFormatType;
	
}

char * describeSourceFileType(int type) {
	static char msg1[] = "input graphDB format is the regular one  (from HDT-parser)";
	static char msg2[] = "input graphDB format is the Adrian's one (from LTJ-project)";
	static char msg3[] = "undefined source format data for graphDB";
	
	if (type == FROM_HDT_PARSER_SOURCE_TYPE)         { return msg1;}
	else if (type == FROM_ADRIAN_PARSER_SOURCE_TYPE) { return msg2;}
	return msg3;
}


/* creates first the spo permutation and the reutilizes s to create the ops permutation index */
int build_index_dual (const char *filename,char *build_options, void **index) {      
//int build_index_dual (struct graphDB *graph, char *build_options, void **index)  {
	
	struct graphDB graph;


	int sourceFormatType = getSourceFileType(build_options);
			printf("\033[1;31m\n");
		printf("\n declared source data type is: \n\t\t %s \n",describeSourceFileType(sourceFormatType));
			printf("\033[0m\n");
			fflush(stdout);fflush(stderr);	

	//reads source data and adds dummie triple and terminator.        //@ 2023 - 
	if(sourceFormatType == FROM_HDT_PARSER_SOURCE_TYPE) { 
		    parseFileHDTformat_IntoGraph(filename,&graph);
	}
	else {	parseFileADRIANformat_IntoGraph(filename,&graph);     /****************/
		printf("\n parsing pending ... exitting \n"); //exit(0);
	}
	
	
	tdualrdfcsa *dualrdf;
	dualrdf = (tdualrdfcsa *) my_malloc (sizeof (tdualrdfcsa) * 1);


	//backups up graph.s and sets new gaps2 for ops permutation.
	
	uint *s = (uint *) my_malloc ((graph.nEntries * (graph.n) +1) * sizeof(uint)) ;  
	memcpy(s,graph.s, ((graph.nEntries * (graph.n) +1) * sizeof(uint))  );
			
	int *gaps2 = (int *) my_malloc (sizeof(int) * (graph.nEntries+1));
	
//	if(sourceFormatType == FROM_HDT_PARSER_SOURCE_TYPE) { 
//		gaps2[0] = 0;                      //objects
//	}
//	else  {  //ADRIAN-TYPE-FORMAT.
		gaps2[0] = 0-graph.gapobjects; //0 for HDT-PARSER-FORMAT; //objects
//	}
		gaps2[1] = graph.no;                                   //predicates 
		gaps2[2] = graph.no + graph.np;                        //subjects    
		gaps2[3] = graph.ns + graph.np + graph.no;             //terminator for suffix sort!

    //------------- 
 
 
	//builds permutation spo
	build_index_from_graph (&graph, build_options,  (void **) &dualrdf->spo); 		


	//restores graph-> and sets new gaps2
	graph.gaps = gaps2;
	graph.s = s;
	
	//builds permutation ops
	shiftIntoOPS_order(s, graph.n, graph.nEntries);
	gr_sortRecordsOPS(&graph);
	
		if (gr_areSortedRecordsOPS(&graph)) {
			fprintf(stderr,"\nGRAPH OPS-- SORTED RECORDS ARE SORTED INCREASINGLY");
		} else {
			fprintf(stderr,"\nGRAPH OPS-- SORTED RECORDS ARE #not# SORTED INCREASINGLY");
		}
        //we reuse the same "build_index", and therefore, gaps [0,1,2] are offset for objects, predicates and subjects.
        //in the rdfcsa with ops permutation.
	build_index_from_graph (&graph, build_options,  (void **) &dualrdf->ops); 	
	
		//now updates wcsaops->gaps so that gaps [0,1,2] are respectively the offsets to subjects, predicates and objects.
		//this enables using a unique mapID and unmapID both for spo and ops rdfcsa's
		
	twcsa *wcsaops = (twcsa *) dualrdf->ops;
	int gaps_s_build=gaps2[2];    // offset for subjects ( = no + np)
	int gaps_o_build=gaps2[0];    // offset for objects  ( = zero  - graph.gapobjects  )
	
		//	wcsaops->gaps[0] = gaps2[2];       // offset for subjects ( = no + np)
		//	//wcsaops->gaps[1] = gaps2[1];     // offset for predicat ( = no     )  -- unchanged.
		//	//wcsaops->gaps[2] = 0       ;       // offset for objects  ( = zero   )
		//	wcsaops->gaps[2] = 0-graph.gapobjects;       ;       // offset for objects  ( = zero   )
		//	
		//	//wcsaops->gaps[2] = gaps2[0];  <-- si pongo esto falla !! PORQUE AHORA gaps2[0] lo he modificado porque wcsa->gaps apunta a gaps2
		//	

	wcsaops->gaps[0] = gaps_s_build;   // offset for subjects is shifted to gaps[0] from gaps[2]
	//wcsaops->gaps[1] = gaps2[1];     // offset for predicat remains unchanged.
	wcsaops->gaps[2] = gaps_o_build;   // offset for objects  ( = zero - graph.gapobjects  )
	
	wcsaops->is_in_spo_order = 0;

	
	//>> save_index () properly saves this last "gaps[] array" for the ops-rdfcsa. 
		
	*index = dualrdf;
	return 0;
}

/* returns the name of the baseName for files storing twcsa for ops permutation */
char *dualbasename(const char *filename) {
	static char dualfile[10000];
	sprintf(dualfile,"%s-%s",filename,DUAL_FILE_EXT);
	return dualfile; 
}

int save_index_dual (void *index, const char *filename) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	
	save_index ( (void *) dualrdf->spo, filename);
	save_index ( (void *) dualrdf->ops, dualbasename(filename)); 
	return 0;
}


int load_index_dual (const char *filename, void **index) {
	tdualrdfcsa *dualrdf;
	dualrdf = (tdualrdfcsa *) my_malloc (sizeof (tdualrdfcsa) * 1);
	
	load_index (filename, (void **) &dualrdf->spo);
	//dualrdf->ops->is_in_spo_order = 1;  //default for twcsa structure.
	
	load_index (dualbasename(filename), (void **) &dualrdf->ops);
	dualrdf->ops->is_in_spo_order = 0;  //
	
	*index = dualrdf;
	return 0;
}

int free_index_dual (void *index) {
	
//	testRecoverAndCompareSPO_OPS_dump(index);	

	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	free_index ( (void *) dualrdf->spo);
	free_index ( (void *) dualrdf->ops);
	
	my_free(dualrdf);
	return 0;
}

int index_size_dual(void *index, ulong *size) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	ulong sizespo,sizeops;
	index_size( (void *) dualrdf->spo, &sizespo);
	index_size( (void *) dualrdf->ops, &sizeops);
	
	*size = sizespo+sizeops;
	return 0;
}
int get_length_dual(void *index, ulong *length) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	ulong lengthspo,lengthops;
	get_length( (void *) dualrdf->spo, &lengthspo);
	get_length( (void *) dualrdf->ops, &lengthops);
	
	*length = lengthspo;//+lengthops;
	return 0;
}

int printInfo_dual(void *index) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;

		fflush(stdout);fflush(stderr);
		printf("\033[1;31m\n");
		fflush(stdout);fflush(stderr);
	printf("\n --------------------------------"); fflush(stdout);fflush(stderr);
	printf("\n STATS FOR RDFCSA1-in-SPO-order \n");
		printf("\033[0m\n");
		fflush(stdout);fflush(stderr);	
	
	printInfo( (void *) dualrdf->spo);
	
		fflush(stdout);fflush(stderr);
		printf("\033[1;31m\n");
		fflush(stdout);fflush(stderr);
	printf("\n --------------------------------"); fflush(stdout);fflush(stderr);
	printf("\n STATS FOR RDFCSA2-in-OPS-order \n");
		printf("\033[0m\n");
		fflush(stdout);fflush(stderr);	

	printInfo( (void *) dualrdf->ops);

		fflush(stdout);fflush(stderr);
		printf("\033[1;31m\n");
		fflush(stdout);fflush(stderr);
	printf("\n --------------------------------"); fflush(stdout);fflush(stderr);
		printf("\033[0m\n");
		fflush(stdout);fflush(stderr);	


	
	return 0;	
}


int testRecoverAndCompareSPO_OPS_dump(void *index) {
	tdualrdfcsa *dualrdf = (tdualrdfcsa *) index;
	
	uint   *data1, *data2;
	size_t   len1,   len2;

	fflush(stderr);fflush(stdout);
	printf("\n\n ------- INI: COMPARING SPO and OPS recovered sequences ---------\n");
	dumpSourceData(dualrdf->spo, &data1, &len1);     	
	dumpSourceDataOPS_to_SPO(dualrdf->ops, &data2, &len2);

			
	printf("\n len-spo = %zu, len-ops = %zu",len1,len2);
	size_t i;
	for (i=0; i< len1; i++) {
		if (data1[i] != data2[i]) {
			printf("\n --DISTINTOS (pos %zu)!! --\n",i);
			break;
		}
	}
	if (i==len1) {
		printf("\n \t **** SPO and OPS->SPO sequences are identical: %zu uints *** \n",len1);
		printf("\n \t\tSPO.data[0..6] = <%u, %u,%u><%u,%u,%u>", data1[0],data1[1],data1[2],data1[3],data1[4],data1[5]);
		printf("\n \t\tOPS-shifted-SPO.data[0..6] = <%u, %u,%u><%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5]);
		printf("\n \t\tthe above data1 and data2 should be identical :D\n");
	}
	printf("\n\n ------- END: COMPARING SPO and OPS recovered sequences ---------\n");
	fflush(stderr);fflush(stdout); 
	free(data1); free(data2);
	
	return 0;
}




/** ******************************************************************************
    * ENDS Interface dual RDFCSA spo & ops.
*********************************************************************************/































































/** Building the index */

//private function called by build_index(filename,...) and build_index_dual(filename,...)

int build_index_from_graph (struct graphDB *graph, char *build_options, void **index) {
	int returnvalue;
	
	//struct graphDB graph;
    printf("\n parameters: \"%s\"\n",build_options); fflush(stderr);

//	//reads source data and adds dummie triple and terminator.
//	parseFileHDTformat_IntoGraph(filename,&graph);
        
    returnvalue = build_WCSA (graph, build_options, index);
 
    if (!returnvalue) 
    	returnvalue = build_iCSA (build_options,*index);

    return returnvalue;  
}



    /* Creates index from a file (or various files whose prefix-name is filename)
      Note that the index is an 
      opaque data type. Any build option must be passed in string 
      build_options, whose syntax depends on the index. The index must 
      always work with some default parameters if build_options is NULL. 
      The returned index is ready to be queried. */
      
int build_index (char *filename, char *build_options, void **index) {      
	//int build_index (struct graphDB *graph, char *build_options, void **index) {
	int returnvalue;
	
	struct graphDB graph;
	//     printf("\n parameters: \"%s\"\n",build_options); fflush(stderr);


	int sourceFormatType = getSourceFileType(build_options);
			printf("\033[1;31m\n");
		printf("\n declared source data type is: \n\t\t %s \n",describeSourceFileType(sourceFormatType));
			printf("\033[0m\n");
			fflush(stdout);fflush(stderr);	

	//reads source data and adds dummie triple and terminator.        //@ 2023 - 
	if(sourceFormatType == FROM_HDT_PARSER_SOURCE_TYPE) { 
		    parseFileHDTformat_IntoGraph(filename,&graph);
	}
	else {	parseFileADRIANformat_IntoGraph(filename,&graph);     /****************/
		printf("\n parsing pending ... exitting \n"); exit(0);
	}


	returnvalue = build_index_from_graph (&graph,build_options, index);
    return returnvalue;  
}


	/**  Saves index on disk by using single or multiple files, having 
	proper extensions. */
int save_index (void *index, const char *filename) {
	const char *basename = filename;
	twcsa *wcsa=(twcsa *) index;

	char *outfilename;
	int file;

	printf("\n Saving structures to disk: %s.*",filename);			
	outfilename = (char *)my_malloc(sizeof(char)*(strlen(basename)+10));

	/**dummy file for dcaro-script --> NOT NECESSARY **/ 
	/*
	{
		strcpy(outfilename, basename);
		unlink(outfilename);
		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}	
		char msg[256];
		sprintf(msg, "G-icsa structure :: %s\n",filename);
		int write_err = write(file,msg,strlen(msg));
		close(file);
	}	
	*/

	/**File with some constants  */
	{
		strcpy(outfilename, basename);
		strcat(outfilename, ".");
		strcat(outfilename, CONSTANTS_FILE_EXT);
		unlink(outfilename);
		if( (file = open(outfilename, O_WRONLY|O_CREAT,S_IRWXG | S_IRWXU)) < 0) {
			printf("Cannot open file %s\n", outfilename);
			exit(0);
		}	
		int write_err=0;
		write_err=write(file, &(wcsa->nEntries), sizeof(uint));
		write_err=write(file, wcsa->gaps, sizeof(int) * (wcsa->nEntries+1));
		write_err=write(file, &(wcsa->ns), sizeof(uint));
		write_err=write(file, &(wcsa->np), sizeof(uint));
		write_err=write(file, &(wcsa->no), sizeof(uint));
		write_err=write(file, &(wcsa->nso), sizeof(uint));
		write_err=write(file, &(wcsa->gapobjects), sizeof(uint));

		write_err=write(file, &(wcsa->n), sizeof(size_t));	
		write_err=write(file, &(wcsa->ssize), sizeof(size_t));
		
		close(file);		
	}	
	
			
	my_free_array(outfilename);

	if (wcsa->myicsa) {
		/******** saves index on integers (bottom layer) ******/
		saveIntIndex((void *) wcsa->myicsa, basename);		
		fprintf(stderr, " ** iCSA saved!\n");
	}	
	
	if (wcsa->s) {
		saveSEfile(basename, wcsa->s, (ulong) wcsa->ssize+1);
		//free(wcsa->se);	
		fprintf(stderr, " ** SE saved!\n");	
	}

	return 0;
}

    /**  Loads index from one or more file(s) named filename, possibly 
      adding the proper extensions. */
int load_index(const char *filename, void **index){
	twcsa *wcsa;
	wcsa = loadWCSA (filename);
	(*index) = (void *) wcsa;
	return 0;
}

	/** Frees the memory occupied by index. */
int free_index(void *index){
	twcsa *wcsa=(twcsa *) index;
	ulong size=0;
	index_size(index,&size);
	fflush(stderr);
	fflush(stdout);
	printf("\n[destroying index] ...Freed %lu bytes... RAM", size);	

	//frees gaps array
	my_free_array(wcsa->gaps);
	
	//frees the array SE.
	if (wcsa->s)
		my_free_array (wcsa->s);

	//the iCSA
	if (wcsa->myicsa) {
		int err = freeIntIndex((void *) wcsa->myicsa);
	}
	
	
	
	//the pointer to wcsa.		
	my_free(wcsa);
	return 0;
}

	/** Gives the memory occupied by index in bytes. */
int index_size(void *index, ulong *size) {

	twcsa *wcsa=(twcsa *)index;

	*size=0;
	*size += sizeof(twcsa);	
	
	if (wcsa->myicsa) {
		size_t nbytes;
		int err = sizeIntIndex((void *) wcsa->myicsa, &nbytes);
		*size += nbytes;
	}
	return 0;	
}

   /** Gives the number of integers in the indexed sequence */
int get_length(void *index, ulong *length) {
	twcsa *wcsa=(twcsa *) index;
	*length = wcsa->ssize;
	return 0;
}

   /** Gives the number of integers in the indexed sequence */
int length (void *index, ulong *length) {
	return (get_length(index,length));
}
/** *******************************************************************************
  * Showing some statistics and info of the index
  * *******************************************************************************/

		/* Shows summary info of the index */
		/* Shows summary info of the index */
int printInfo(void *index) {

	twcsa *wcsa = (twcsa *) index;
	
	unsigned long indexSize;
	size_t intIndexSize;
	int err;
	
	err = index_size(index, &indexSize);
	if (err!=0) return err;
	err = sizeIntIndex(wcsa->myicsa, &intIndexSize); 	
	if (err!=0) return err;
	
		printf("\n   Vocabulary info -----");
//		printf("\n      Num entries in map[]   (map_size)  = %zu",(size_t) wcsa->map_size);
//		printf("\n      Num entries in unmap[] (zeronode)  = %zu",(size_t) wcsa->zeronode);

//		printf("\n        vocabulary unmap size (zeronode*4)  =   %zu bytes",(size_t) wcsa->zeronode * sizeof(uint));
//		printf("\n        vocabulary map[] size <as uints>    =   %zu bytes",(size_t) (wcsa->zeronode + wcsa->map_size)*sizeof(uint) );
		
//		printf("\n   -----");
//		printf("\n   ");


		printf("\n   wcsa structure = %zu bytes", (size_t) sizeof(twcsa));
		printf("\n   no map[]/unmap[] structures needed (no vocabulary/permutation stored)");

		printf("\n\n @@ Summary of self-index on Integers:");
		err = printInfoIntIndex(wcsa->myicsa, " ");
		if (err!=0) return err;
 			
		printf("\n ========================================================:");		
		printf("\n ** TOTAL SIZE IN MEMORY WHOLE STRUCTURE= %zu bytes **", (size_t) indexSize);
		printf("\n ========================================================:");		
		printf("\n\n");
		return 0;
	}




/** ***********************************************************************************
	 CONSTRUCTION OF THE INDEX WCSA
    ***********************************************************************************/

/**
  * BUILDS THE WCSA INDEX
  */

int build_WCSA (struct graphDB *graph, char *build_options, void **index) {
	twcsa *wcsa;
	wcsa = (twcsa *) my_malloc (sizeof (twcsa) * 1);

	size_t i,j;
	
	wcsa->nEntries = graph->nEntries;
	wcsa->gaps     = graph->gaps;
	//for (j=0;j<=wcsa->nEntries; j++) wcsa->gaps[j]+=1 ;  //we want ID=0 to use it as a marker for searches
	wcsa->ns    = graph->ns;
	wcsa->np    = graph->np;
	wcsa->no    = graph->no;
	wcsa->nso   = graph->nso;
	wcsa->gapobjects = graph->gapobjects;

	wcsa->n        = graph->n;       
	wcsa->s 	   = graph->s;
	wcsa->ssize	   = graph->nEntries*graph->n;
	
	wcsa->is_in_spo_order = 1;  //2023 rdfcsa-dual  (default value = 1).

	{
		uint mins = 1999999999, minp = 1999999999, mino=1999999999;
		uint maxs = 0, maxp = 0, maxo=0;

		for (i=0; i< wcsa->n; i++) {
				if (wcsa->s[i*(wcsa->nEntries) + 0]  > maxs) maxs = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  > maxp) maxp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  > maxo) maxo = wcsa->s[i*(wcsa->nEntries) + 2];

				if (wcsa->s[i*(wcsa->nEntries) + 0]  < mins) mins = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  < minp) minp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  < mino) mino = wcsa->s[i*(wcsa->nEntries) + 2];
		}
		
		printf("\n SUBJECTS: minID found in source= %u, maxID= %u :: collection-S = %u", mins, maxs, wcsa->ns);
		printf("\n PREDICTS: minID found in source= %u, maxID= %u :: collection-P = %u", minp, maxp, wcsa->np);
		printf("\n OBJJECTS: minID found in source= %u, maxID= %u :: collection-O = %u", mino, maxo, wcsa->no);
		printf("\n SUBJECTS-OBJECTS is %u ", wcsa->nso);
	}

   printf("\n GAPS=  ");
   printf("\n GAPS[0]= %d", wcsa->gaps[0]);
   printf("\n GAPS[1]= %d", wcsa->gaps[1]);
   printf("\n GAPS[2]= %d", wcsa->gaps[2]);
   printf("\n GAPS[3]= %d", wcsa->gaps[3]);



   printf("\n\n antes de sumar gaps ");
   printf("\n SE[0]= %u", wcsa->s[0]);
   printf("\n SE[1]= %u", wcsa->s[1]);
   printf("\n SE[2]= %u", wcsa->s[2]);
   printf("\n SE[3]= %u", wcsa->s[3]);
   printf("\n SE[4]= %u", wcsa->s[4]);
   printf("\n SE[5]= %u", wcsa->s[5]);
   printf("\n SE[6]= %u", wcsa->s[6]);
   printf("\n SE[7]= %u", wcsa->s[7]);

	// ADDS + GAPS[j] to each S[i] value but the dummie node (the 1st one)
	i=0;
		for (j=0;j<wcsa->nEntries;j++)
			wcsa->s[0+j] += wcsa->gaps[j]; 			

	for (i=1; i< wcsa->n; i++) {
		for (j=0;j<wcsa->nEntries;j++)
			wcsa->s[i*(wcsa->nEntries) + j] += wcsa->gaps[j] ; // +1; 
	}


   printf("\n\n tras sumar gaps ");
   printf("\n SE[0]= %u", wcsa->s[0]);
   printf("\n SE[1]= %u", wcsa->s[1]);
   printf("\n SE[2]= %u", wcsa->s[2]);
   printf("\n SE[3]= %u", wcsa->s[3]);
   printf("\n SE[4]= %u", wcsa->s[4]);
   printf("\n SE[5]= %u", wcsa->s[5]);
   printf("\n SE[6]= %u", wcsa->s[6]);
   printf("\n SE[7]= %u", wcsa->s[7]);
    
   //terminator for suffix sorting (greater than all the other values in s[])   	
	wcsa->s[wcsa->ssize] = wcsa->gaps[wcsa->nEntries]; //terminator for suffix sorting (greater than all the other values in s[])



   printf("\n SE[%zu]= %u",wcsa->ssize-3, wcsa->s[wcsa->ssize-3]);
   printf("\n SE[%zu]= %u",wcsa->ssize-2, wcsa->s[wcsa->ssize-2]);
   printf("\n SE[%zu]= %u",wcsa->ssize-1, wcsa->s[wcsa->ssize-1]);
   printf("\n SE[%zu]= %u",wcsa->ssize-0, wcsa->s[wcsa->ssize]);


	{
		uint mins = 1999999999, minp = 1999999999, mino=1999999999;
		uint maxs = 0, maxp = 0, maxo=0;

		for (i=0; i< wcsa->n; i++) {
				if (wcsa->s[i*(wcsa->nEntries) + 0]  > maxs) maxs = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  > maxp) maxp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  > maxo) maxo = wcsa->s[i*(wcsa->nEntries) + 2];

				if (wcsa->s[i*(wcsa->nEntries) + 0]  < mins) mins = wcsa->s[i*(wcsa->nEntries) + 0];
				if (wcsa->s[i*(wcsa->nEntries) + 1]  < minp) minp = wcsa->s[i*(wcsa->nEntries) + 1];
				if (wcsa->s[i*(wcsa->nEntries) + 2]  < mino) mino = wcsa->s[i*(wcsa->nEntries) + 2];
		}
		
		printf("\n SUBJECTS: minID found in source2= %u, maxID= %u :: collection-S = %u", mins, maxs, wcsa->ns);
		printf("\n PREDICTS: minID found in source2= %u, maxID= %u :: collection-S = %u", minp, maxp, wcsa->np);
		printf("\n OBJJECTS: minID found in source2= %u, maxID= %u :: collection-S = %u", mino, maxo, wcsa->no);
		printf("\n SUBJECTS-OBJECTS-2 is %u ", wcsa->nso);
		fflush(stdout);
	}

	
	//if no holes appear, that is enough... otherwise we need a dictionary.
	//for RDF it would not be needed...
		
	//wcsa->map=map;
	//wcsa->map_size = map_size;
	//wcsa->unmap = unmap;
	//wcsa->zeronode = zeronode;
	/**/
		
		
	wcsa->myicsa = NULL;
	
	*index = wcsa;
	return 0;
}


int build_iCSA (char *build_options, void *index)
{
	twcsa *wcsa = (twcsa *) index;
	/********* creates the self-index on ints (bottom layer) *********/
	//creating CSA from Edu's code...
	{
		size_t total;
		fprintf(stderr,"\n **** CREATING CSA-bottom-layer *****");
		void *bottomIntIndex;
		int err =  buildIntIndex(wcsa->s,wcsa->ssize+1, wcsa->nEntries, build_options,(void **)&bottomIntIndex);				
		wcsa->myicsa = bottomIntIndex;

		err = sizeIntIndex((void *) wcsa->myicsa, &total);

		printf("\n\t**** [iCSA built on %zu integers (included dummy (3)). Size = %zu bytes... RAM\n",(size_t) wcsa->ssize, total);fflush(stdout);
	}		
	return 0;
}


	/** saves the content of the file SE (ids of the source words) **/
int saveSEfile (const char *basename, uint *v, ulong n) {
	char outfilename[255];
	FILE *file;
	sprintf(outfilename,"%s.%s",basename,SE_FILE_EXT);
	unlink(outfilename);
	if( (file = fopen(outfilename, "w")) ==NULL) {
		printf("Cannot open file %s\n", outfilename);
		exit(0);
	}

	//write(file, v, sizeof(uint) * n );
	int err= fwrite(v,sizeof(uint),n,file); 
	fclose(file);
	return 0;
}


/** ********************************************************************
  * Loading from disk
  **********************************************************************/ 

/**-----------------------------------------------------------------  
 * LoadWCSA.                                                       
 * Loads all the data structures of WCSA (included the icsa)     
 ----------------------------------------------------------------- */ 

twcsa *loadWCSA(const char *filename) {
	twcsa *wcsa;
	
	wcsa = (twcsa *) my_malloc (sizeof (twcsa) * 1);
	wcsa->n=0;
	wcsa->is_in_spo_order = 1;  //2023 - rdfcsa-dual

	int err = loadIntIndex(filename, (void **)&wcsa->myicsa);
	
	int sel = getSelecticsa(wcsa->myicsa, 2822333);	
	printf("got sel = %d\n", sel);
	loadStructs(wcsa,filename);   									  

	return wcsa;
}

/** ------------------------------------------------------------------
 * LoadStructs.
 *	Reads files and loads all the data needed for searcherFacade
 ----------------------------------------------------------------- */ 
 void loadStructs(twcsa *wcsa, const char *basename) {
	
	char *filename;
	int file;
		
	filename = (char *)my_malloc(sizeof(char)*(strlen(basename)+10));
	fprintf(stderr,"Loading Index from file %s.*\n", basename);
	
	/**File with some constants  */
	{	strcpy(filename, basename);
		strcat(filename, ".");
		strcat(filename, CONSTANTS_FILE_EXT);
		
		if( (file = open(filename, O_RDONLY)) < 0) {
			printf("Cannot open file %s\n", filename);
			exit(0);
		}

	int read_err=0;
		read_err= read(file, &(wcsa->nEntries), sizeof(uint));
		wcsa->gaps = (int *) my_malloc(sizeof(int) * (wcsa->nEntries+1));
		read_err= read(file, wcsa->gaps, sizeof(int) * (wcsa->nEntries+1));			
		read_err= read(file, &(wcsa->ns), sizeof(uint));			
		read_err= read(file, &(wcsa->np), sizeof(uint));
		read_err= read(file, &(wcsa->no), sizeof(uint));
		read_err= read(file, &(wcsa->nso), sizeof(uint));
		read_err= read(file, &(wcsa->gapobjects), sizeof(uint));
		
		read_err= read(file, &(wcsa->n), sizeof(size_t));			
		read_err= read(file, &(wcsa->ssize), sizeof(size_t));
				
		close(file);
	}		
	
	printf("\nLOAD: ns = %u, np=%u, no=%u, nso=%u, gapoffsets=%u, total = %u",wcsa->ns, wcsa->np, wcsa->no, wcsa->nso,wcsa->gapobjects,  wcsa->ns + wcsa->np + wcsa->no); 	

	printf("\nLOAD: gaps=");
	{uint i;
		for (i=0;i<=3;i++) printf("[%d]",wcsa->gaps[i]);
	}
	printf("\n");
		
	wcsa->s= NULL;	
	my_free_array(filename);
}

	


/** ****************************************************************
  * Querying the index WCSA
  * ***************************************************************/
///////////////////////////////////////////////////////////////////////////////////////
//					   FUNCTIONS NEEDED FOR SEARCHING A PATTERN    					 //
///////////////////////////////////////////////////////////////////////////////////////




/** Querying the index =============================================================*/
	



uint mapID (twcsa *g, uint value, uint type) {
	return value + g->gaps[type];	
}

uint unmapID (twcsa *g, uint value, uint type) {
	return value - g->gaps[type];
	
}





/**************************/
//** debugging only

uint get_num_subjects(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->ns;
}

uint get_num_predicates(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->np;
}

uint get_num_objects(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->no;
}

uint get_num_subjects_objects(void *gindex) {
	twcsa *g = (twcsa*) gindex;
	return g->nso;
}

uint get_num_nodes(void *gindex) {   //para borrar no se usa!
	return 0;
}

/*  ------------*/
/******** end debugging only **********************/





/*************************************************************************/
/******** RDF GRAPHS ****************************************************/
/*************************************************************************/
/*
#ifdef EXPERIMENTS
#include "opssearchRDF_experiments.c"
    //more optimized ... but little improvements !!
#else
#include "opssearchRDF.c"
	//with some tests ... and only slightly slower !!
#endif
*/
/*************************************************************************/

#include "opssearchRDF_experiments.c"




















	/** ------------------------------------------------------------------
	 * recovers the source text by calling display(0,fileSize);
	 * ------------------------------------------------------------------ */


//-----------------------------------------------------------------------------------//
// needed for qsort of entries in dumpSourceDataSorted function
uint nentries_buildFacade; // Global variable used in quicksort graphsuffixCmp function
int graphsuffixCmp_buildFacade(const void *arg1, const void *arg2) {

	register uint *a,*b;
	a=(uint *)arg1;
	b=(uint *)arg2;

	uint i;
	for (i=0; i<nentries_buildFacade;i++) {
		if (*a <*b) return -1;
		if (*a >*b) return +1;
		a++;b++;
	}
	//if (i== nentries_buildFacade) 
	return 0;

}

void sortRecords_buildFacade(twcsa *wcsa, uint*buffer) {
	nentries_buildFacade= wcsa->nEntries;
	qsort(buffer, wcsa->n, sizeof(uint)*wcsa->nEntries, graphsuffixCmp_buildFacade);
}
//-----------------------------------------------------------------------------------//
	


void dumpSourceData(twcsa *wcsa, uint **data, size_t *len) {
	uint * buffer;
	dumpICSASequence(wcsa->myicsa, &buffer, len);

	size_t i,j,z=0;
	for (i=0; i<wcsa->n; i++){
		for (j=0;j<wcsa->nEntries;j++) {
			//buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			buffer[z] = unmapID(wcsa,buffer[z],j);
			z++;
		}
	}		
	uint *data2 = buffer;
	printf("\n \t\tSPO.in.SPOorder[0..9] = <%u, %u,%u> <%u,%u,%u> <%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5],data2[6],data2[7],data2[8]);
	
	*data = buffer;
	*len = *len -1;
}


//dual 2023 --
//dumps data from a twcsa in OPS order, and recreates a sort SPO sequence (in spo order).
void dumpSourceDataOPS_to_SPO(twcsa *wcsa, uint **data, size_t *len){
	uint * buffer;
	dumpICSASequence(wcsa->myicsa, &buffer, len);

	size_t i,j,z=0;
	for (i=0; i<wcsa->n; i++){
		for (j=0;j<wcsa->nEntries;j++) {
			//buffer[z] = wcsa->unmap[buffer[z]] - wcsa->gaps[j];
			buffer[z] = unmapID(wcsa,buffer[z],wcsa->nEntries-1-j);
			z++;
		}
	}		
	
	uint *data2 = buffer;
	
	printf("\n \t\tCALL TO dumpSourceOPS_to_SPO DATA \n");
	printf("\n \t\tOPS.data[0..6] = <%u, %u,%u><%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5]);

	shiftIntoOPS_order(buffer,wcsa->n, wcsa->nEntries);     //shifts OPS sequence into an SPO sequence
	array_sortRecordsSPO(buffer, wcsa->n, wcsa->nEntries);  //orders SPO sequence by s->p->o order.
	printf("\n \t\tOPS.data-shiftedSPO-and-inSPOorder[0..9] = <%u, %u,%u> <%u,%u,%u> <%u,%u,%u>", data2[0],data2[1],data2[2],data2[3],data2[4],data2[5],data2[6],data2[7],data2[8]);
	
	*data = buffer;
	*len = *len -1;
}








// dual 2023--
// for a given position "pos" within 0..3n-1, returns its type  (for a rdfcsa in SPO order)
uint typeFromPos_SPO(size_t pos, size_t n) {
	//s \in [0,n-1], p \in [0,2n-1], o \in [2n,3n-1]
	if (pos < n)        return SUBJECT;
	else if (pos < 2*n) return PREDICATE;	
	return OBJECT;
}

// for a given position "pos" within 0..3n-1, returns its type  (for a rdfcsa in OPS order)
uint typeFromPos_OPS(size_t pos, size_t n) {
	//o \in [0,n-1], p \in [0,2n-1], s \in [2n,3n-1]
	if (pos < n)        return OBJECT;
	else if (pos < 2*n) return PREDICATE;	
	return SUBJECT;
}

// for a given position "pos" within 0..3n-1, returns its actual value (for a rdfcsa in SPO order)
uint valueFromPos_SPO(twcsa *g, size_t pos) {
	//s \in [0,n-1], p \in [0,2n-1], o \in [2n,3n-1]
	size_t n = g->n;
	uint tipo = typeFromPos_SPO( pos, n);
	uint value;
	value = getRankicsa(g->myicsa,pos) -1;
	value = unmapID(g, value, tipo);	
	return value;	
}

// for a given position "pos" within 0..3n-1, returns its actual value  (for a rdfcsa in OPS order)
uint valueFromPos_OPS(twcsa *g, size_t pos){
	//o \in [0,n-1], p \in [0,2n-1], s \in [2n,3n-1]
	size_t n = g->n;
	uint tipo = typeFromPos_OPS( pos, n);
	uint value;
	value = getRankicsa(g->myicsa,pos) -1;
	value = unmapID(g, value, tipo);	
	return value;	

}


//	//prints and returns a triple, whose components include position pos (0 <= pos <3n)
//	//receives a wcsa in order s-p-o
//	uint * printTripleSPO(twcsa *wcsa, size_t pos) {
//		static uint res[3];
//		twcsa *g = wcsa;
//	
//		uint nE = wcsa->nEntries;
//		size_t n = wcsa->n;      //number of triples.
//	
//		uint z;	
//		size_t i,j;
//		i=pos;
//		for (j=0;j<nE;j++) {
//			z = typeFromPos_SPO(i,n);
//			res[z] = valueFromPos_SPO(g,i);
//					//uint value = getRankicsa(g->myicsa,i) -1;
//					//res[z] = unmapID(wcsa, value, z);
//			i = getPsiicsa(g->myicsa, i);
//		}		
//	
//		printf("<%u, %u,%u>", res[0],res[1],res[2]);	
//		return res;
//	}
//	
//	
//	//prints and returns a triple, whose components include position pos (0 <= pos <3n)
//	//receives a wcsa in order o-p-s
//	uint * printTripleOPS(twcsa *wcsa, size_t pos) {
//		static uint res[3];
//		twcsa *g = wcsa;
//	
//		uint nE = wcsa->nEntries;
//		size_t n = wcsa->n;      //number of triples.
//	
//		uint z;	
//		size_t i,j;
//		i=pos;
//		for (j=0;j<nE;j++) {
//			z = typeFromPos_OPS(i,n);
//			res[z] = valueFromPos_OPS(g,i);
//					//uint value = getRankicsa(g->myicsa,i) -1;
//					//res[z] = unmapID(wcsa, value, z);
//			i = getPsiicsa(g->myicsa, i);
//		}		
//	
//		printf("<%u, %u,%u>", res[0],res[1],res[2]);	
//		return res;
//	}


//	//prints and returns a triple, whose components include position pos (0 <= pos <3n)
//	// allows g to be either a twcsa in spo- or ops-order	
//	uint * printTriple(twcsa *wcsa, size_t pos) {
//		if (wcsa->is_in_spo_order) 
//			return printTripleSPO(wcsa,pos);
//		
//		return printTripleOPS(wcsa,pos);	
//	}


// for a given position "pos" within 0..3n-1, returns its type of component 
// allows g to be either a twcsa in spo- or ops-order
uint dual_typeFromPos(twcsa *g, size_t pos){
	if (g->is_in_spo_order) 
		return  typeFromPos_SPO(pos, g->n);

	return  typeFromPos_OPS(pos, g->n);
}

// for a given position "pos" within 0..3n-1, returns its actual value 
// allows g to be either a twcsa in spo- or ops-order
uint dual_valueFromPos(twcsa *g, size_t pos){
	if (g->is_in_spo_order) 
		return  valueFromPos_SPO(g, pos);

	return  valueFromPos_OPS(g, pos);
}



// prints and returns a triple, whose components include position pos (0 <= pos <3n)
// allows g to be either a twcsa in spo- or ops-order

uint * dual_printTriple(twcsa *g, size_t pos) {
	static uint res[3];

	uint nE = g->nEntries;
	size_t n = g->n;      //number of triples.

	uint z;	
	size_t i,j;
	i=pos;
	for (j=0;j<nE;j++) {
		z = dual_typeFromPos(g,i);
		res[z] = dual_valueFromPos(g,i);
				//uint value = getRankicsa(g->myicsa,i) -1;
				//res[z] = unmapID(wcsa, value, z);
		i = getPsiicsa(g->myicsa, i);
	}		

	printf("<%u, %u,%u>", res[0],res[1],res[2]);	
	return res;
}



//*********************************************************************************
//Computes the 1st position i in [left,right] such that \Psi[i] \in in [tl,rt].
//Returns: 
//   0 if no i \in [left,right] maps into [tl,tr]
//   Otherwise: 
//		- Computes x=Psi[i]. 
//		- Updates *left =i;
//		- Returns dual_valueFromPos(g,x);
//
uint dual_searchPsiTarget_to_leap(twcsa *g, ulong *left, ulong right, ulong tl, ulong tr) {
	ulong l=*left;
	ulong r=right;
	ulong numocc;
	binSearchPsiTarget_samplesFirst(g->myicsa,&l,&r,&numocc, tl, tr);
	if (numocc) {  
		*left = l; //updates only *left   (right is not modified) 
		ulong x = getPsiicsa(g->myicsa,l);
		uint ret = dual_valueFromPos(g,x); //the value at *left :D 
										   // could also be returned by a creating a modified 
										   // binSearchPsiTarget_samplesFirst - variant. TO-DO
										   // that also computes it (so we avoid the call to getPsiicsa() above)
		return ret;
	}
	return 0;	
}


uint dual_searchPsiPsiTarget_to_leap(twcsa *g, ulong *left, ulong right, ulong tl, ulong tr) {
	ulong l=*left;
	ulong r=right;
	ulong numocc;
	binSearchPsiPsiTarget(g->myicsa,&l,&r,&numocc, tl, tr);
	if (numocc) {  
		*left = l; //updates only *left   (right is not modified) 
		ulong x = getPsiicsa(g->myicsa,l);
		      x = getPsiicsa(g->myicsa,x);
		uint ret = dual_valueFromPos(g,x); //the value at *left :D 
										   // could also be returned by a modified 
										   // binSearchPsiPsiTarget  ??
		return ret;
	}
	return 0;	
}



//*********************************************************************************
//Computes the positions i in [left,right] such that forall i, \Psi[i] \in in [tl,rt].
//		- Updates *left  and *right is updated
//		- Then updates *left = getPsiValue( *left), and *right = getPsiValue (*right).  <-- no
int dual_searchPsiTarget_to_down(twcsa *g, ulong *left, ulong *right, ulong tl, ulong tr) {
	ulong l=*left;
	ulong numocc;
//	printf("\n left = %lu, right = %lu",*left, *right);
	binSearchPsiTarget_samplesFirst(g->myicsa,left,right,&numocc,tl,tr);
//	printf("\n left = %lu, right = %lu",*left, *right);
	
	//if (*left != l) {
	//	printf("\n buildFacade.c dual_searchPsiTarget_to_down():: LEFT HA CAMBIADO Y NO DEBERIA: (viejo) %lu != %lu (nuevo)\n",l, *left);
	//}
	
//	*left  = getPsiicsa(g->myicsa, *left);
//	*right = getPsiicsa(g->myicsa,*right);
//	printf("\n PSI left = %lu, right = %lu",*left, *right);

	return 1;
}








//*********************************************************************************
//Given an twcsa (either rdfcsa-spo or rdfcsa-ops), computes the range: 
// [0,n-1] || [n,2n-1] || [2n, 3n-1], that corresponds to "type"
// depending on the spo|ops order of g.
//
int dual_getRangeLR_for_type(twcsa *g, uint type, ulong *left, ulong *right) {
	ulong n = g->n;
	
	if (g->is_in_spo_order) {  //g in s->p->o order
		if      (type == SUBJECT  ) {*left =  0; *right=  n-1;}
		else if (type == PREDICATE) {*left =  n; *right=2*n-1;}
		else  		/*OBJECT*/	    {*left =2*n; *right=3*n-1;}		
	}
	else {   					//g in o->p->s order
		if      (type == OBJECT  )  {*left =  0; *right=  n-1;}
		else if (type == PREDICATE) {*left =  n; *right=2*n-1;}
		else  		/*SUBJECT*/ 	{*left =2*n; *right=3*n-1;}				
	}
		
return 1;	
}

//int dual_getRangeLR_uint32_for_type(twcsa *g, uint type, uint *left, uint *right) {
//	ulong l,r;
//	int retval = dual_getRangeLR_for_type(g,type,&l,&p);
//	*left =l; *right=r;
//	return retval;
//} 


//*********************************************************************************
//Given an twcsa "g" (either rdfcsa-spo or rdfcsa-ops), 
//  and a value of type "type"  (e.g. value 5 of type PREDICATE), uses the 
//  underlying D->select() to obtain its range [left,right] that corresponds to the
//  value "value" of type "type".
// * We use g->mapID(value, type) and then conceptually use 2 selects().
// * Recall such mapID internally dependes on the order (either spo or ops of g).
//
int dual_getRangeLR_for_type_and_value(twcsa *g, uint type, uint value, ulong *left, ulong *right) {

	uint vv = mapID(g,value,type);
	ulong l,r;
	
	if ((type == SUBJECT) || (type == OBJECT)){
		//l = getSelecticsa(g->myicsa, vv+1);
		//r = getSelecticsa(g->myicsa, vv+2)-1;
		geticsa_select_j_y_j_mas_1 (g->myicsa, vv+1, &l, &r);	r--;				
	}
	else /* (type == PREDICATE)*/ {
		l= getSelectTablePredicates(g->myicsa,vv+1);
		r= getSelectTablePredicates(g->myicsa,vv+2)-1;		
	}

	*left =l;*right=r;
	return 1;
}

//int dual_getRangeLR_uint32_for_type_and_value(twcsa *g, uint type, uint value,  uint *left, uint *right) {
//	ulong l,r;
//	int retval = dual_getRangeLR_for_type_and_value(g,type,value,&l,&p);
//	*left =l; *right=r;
//	return retval;
//} 


//*********************************************************************************
//Given an twcsa "g" (either rdfcsa-spo or rdfcsa-ops), 
// returns the range [lp,rp] that corresponds for values x of type "type", such that x>=value 
// depending on the spo|ops order of g.
// the same as previous function, yet with a wider right value, to the end of the range for that type.

int dual_getRangeLR_for_type_and_GEQvalue(twcsa *g, uint type, uint value, ulong *left, ulong *right){
	uint vv = mapID(g,value,type);
	ulong l,r;
	
	dual_getRangeLR_for_type(g, type, &l, &r);  //fixes right value
	
	// and now computes left value
	
	if ((type == SUBJECT) || (type == OBJECT)){
		l = getSelecticsa(g->myicsa, vv+1);
		//r = getSelecticsa(g->myicsa, vv+2)-1;
		//geticsa_select_j_y_j_mas_1 (g->myicsa, vv+1, &l, &r);	r--;				
	}
	else /* (type == PREDICATE)*/ {
		l= getSelectTablePredicates(g->myicsa,vv+1);
		//r= getSelectTablePredicates(g->myicsa,vv+2)-1;		
	}

	*left =l;*right=r;
	return 1;
}




//*********************************************************************************
// checks if a given value of a given type is a valid value in the source alphabets
// eg p in [1,npi] (npi=number of predicates in source dataset => npi = np -1;
// eg s in [1,nsi] (nsi=number of subjects   in source dataset => nsi = ns -1;
// eg o in [gapobjects,gapobjects+noi-1] 
//                 (noi=number of objects    in source dataset => noi = no -1;
//
int dual_isValidValue_for_Type_in_Input(twcsa *g, int type, uint value){
	
	ulong l,r;
	uint ns = g->ns;     ns--;   //skips dummie
	uint np = g->np;     np--;   //skips dummie
	uint no = g->no;     no--;   //skips dummie
	uint gapobjects = g->gapobjects;
	
		if      (type == SUBJECT  ) {l=1; r=ns;}
		else if (type == PREDICATE) {l=1; r=np;}
		else  		/*OBJECT*/	    {l=gapobjects+1; r= gapobjects + no;}		
	
	if( (l<=value) && (r>=value)) return 1;
	return 0;	
}












//*************************/
//http://es.tldp.org/COMO-INSFLUG/COMOs/Bash-Prompt-Como/Bash-Prompt-Como-5.html
void setColorRojo(){
	printf("\033[1;31m");
}
void setColorAzul(){
	printf("\033[0;34m");
}
void setColorMorado(){
	printf("\033[0;35m");
}
void setColorVerde(){
	printf("\033[1;32m");
}
void setColorAmarillo(){
	printf("\033[1;33m");
}
void setColorNormal(){
	printf("\033[0m");
}
