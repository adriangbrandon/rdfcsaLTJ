#ifndef _INTERFACE_DUAL
#define _INTERFACE_DUAL

#include <vector>

/* interface.h
 * Copyright (C) 2011, Antonio Fariña, all rights reserved.
 *
 * interface.h: Interface including all the functions temporal graph needs.
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


/**
 * rdfcsaLTJ/external/swcsa/BUILDALLDUALwcsa $DATADIR/$DATASET indexes/$DATASET.rdfcsa.psi16-HUFFRLE-default  "sourceFormat=ADRIAN;sPsi=16; nsHuff=16;psiSF=4; psitypespo=111, psitypeops=111" 
 
	##CONSIDERATIONS FOR BUILDING INDEXES
	##We are using rdfcsa with a 3-ranges configuration ==> 
	##  Psi for subjects, predicates and objects are encoded independently
	##
	##Parameters: psitypespo=xyz; psitypeops=abc.
	##  Use a 3digit number for xyz and abc. 
	##    e.g. 123 (in xyz) means:
	##         ----> 1 (HuffmanRle) for 1st range (eg. subjects).
	##         ----> 2 (Plain)     for 2nd range (eg. predicates).
	##         ----> 3 (PlainRle)  for 3rd range (eg. objects).
	## Default option, if no "psitypespo=xyz; psitypeops=abc" options are
	## provided, is xyz=111 and abc=111 (HuffmanRle for the 3 ranges).
	## Indeed 111 = 000, and 000 is set to mark no psitypes??? where provided.

**/







 
/* General interface for using the compressed temporal graph */

#ifndef uchar
#define uchar unsigned char
#endif
#ifndef uint
#define uint unsigned int
#endif
#ifndef ulong
#define ulong unsigned long
#endif


//TYPE for use in mapID(id,type) and unmapID(id,type)   --> definido por agora en buildFacade.h
//also received from LTJ-side to specify type of value that is received from 
// leap (..) and down (..) functions
#ifndef SUBJECT 
		#define SUBJECT (0) 
#endif
#ifndef PREDICATE
		#define PREDICATE (1)
#endif
#ifndef OBJECT 
		#define OBJECT (2)
#endif

#ifndef NO_TYPE_SET_OPS
		#define NO_TYPE_SET_OPS (99)
#endif


#define NO_MORE_IDS_IN_BUFFER (0xFFFFFFFF)
#define MAX_PSI_VALUE_FOR_BUFFER (16)



/* creates first the spo permutation and the reutilizes s to create the ops permutation index */

//int build_index_dual (struct graphDB *graph, const char *build_options, void **index) ;
int build_index_dual (const char * filename, char *build_options, void **index) ;  

        /*  Saves index on disk by using single or multiple files, having 
          proper extensions. */
int save_index_dual (void *index, const char *filename);
        /*  Loads index from one or more file(s) named filename, possibly 
          adding the proper extensions. */
int load_index_dual (const char *filename, void **index);
        /* Frees the memory occupied by index. */
int free_index_dual (void *index);
        /* Gives the memory occupied by index in bytes. */
int index_size_dual(void *index, ulong *size);
        /* Gives the length of the text indexed (source asumming uint_32 values */
int get_length_dual(void *index, ulong *length);
		/* Gives info regarding the internals of the index */
int printInfo_dual(void *index);

		/* Recovers the source triples from the index in SPO ans OPS and then compares them */
int testRecoverAndCompareSPO_OPS_dump(void *index);   




/* --------------------------------------------------------------------*/
/* Iterator for LTJ algorithm && functions needed                      */
/* --------------------------------------------------------------------*/

/**Definition of iterator */
typedef struct {
	//uchar is_empty;    // empty if numfijadas ==0;
		
	uint nFixed;        // number of variables that were given with a value		
	uchar typeFixed[3]; // type of variables fixed \in {0=SUBJECT,1=PREDICATE,2=OBJECT}
	uint valueFixed[3]; // actual value set (according to type above).

	uint range[4];     // [0..1] for twcsa-spo, ranges[2..3] for twcsa-ops.
	uint range_backup[4]; // backup of range[] to keep state upon call to down().
	
	void *dualrdfcsa;

	uchar estado_leap_type   ; //= -1; to use in leap v2.0  --default 99 (no type is set)
	uint  estado_leap_value  ; //= -1; to use in leap v2.0
	uint  estado_leap_leftpos; //= -1; to use in leap v2.0
	
	//support for get_one_dual_init() and get_one_dual_next().
	//uint *buffer;  //must be of sizemax tpsi value must be >= tpsi
	void *active_g; // (set in get_one_dual_init())
	void *iter_psi; // (set in get_one_dual_init())
	uint tpsi;
	uint IDtype;        //type of the IDS recovered (set in get_one_dual_init())
	
	//ulong left, right;  //we will recover IDS within range [left,right]. 
	                    //left is moved forward each within get_one_dual_next()
	
	//uint pos;           //pos = Psi[i], i \in [left, right] 

} t_iterator;



//frees the memory associated to the iterador
//returns NULL;
//call it as: iter = destroyIterator_dual(iter);
void * destroyIterator_dual(void *index);



//Creates a ltf-iterator with none variables set.
//Returs a void * pointer to the created t_iterator structure.
void * createIterator_dual(void *index);
	// inicializa os 2 rangos con [1,n], n=numero de triples.

void * cloneIterador_dual(void* iteradorsrc); 
	//básicamente facer un memcpy (sizeof(structura iterador) ...).
	//returns:
		//una copia del iteradorsrc.
		//malloc (sizeof(structura)
		//memcpy (...)
	//Se necesita xq  Adrián podría necesitar clonar los iteradores...

	//Prints to stdout the internal info of the iterator.
void printIterator_dual(void * iterator) ;

int init_spo_dual(void *iterator, uint s, uint p, uint o);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamén valería)
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	
int init_so_dual(void *iterator, uint s, uint o);
	//inicializa rdfcsa-ops
	//inicializa rdfcsa-spo non se usa. (tamen serviría se considero "O->S"... porque despois só podo facer down con P).
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise

int init_sp_dual(void *iterator, uint s, uint p);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamen serviría se considero "P->S"... porque despois só podo facer down con o).
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	
int init_po_dual(void *iterator, uint p, uint o);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamen serviría se considero "O->P"... porque despois só podo facer down con s).
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	
int init_s_dual(void *iterator, uint s);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar p ou o
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	
int init_p_dual(void *iterator, uint p);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar s ou o
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	
int init_o_dual(void *iterator, uint o);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar s ou p
	//returns:
    //  1 si el rango inicializado no es vacío.
	//	0 otherwise
	

// returns the 1st valid value for a given type
uint min_of_type_dual(void *iterator, int type);
	

uint leap_dual(void *iterator, int type, uint value);
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



int down_dual(void *iterator, int type, uint value);      //numfijadas ++ e actualizar rangos
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
	

int up_dual(void *iterator);                           //numfijadas -- e actualizar rangos
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

// para un iterador, devolver o tamaño mínimo dos rangos que ten restrinxidos. --- ou algo así ... a definir con Adrián.
// se están os 2 activos --> o menor
// se só hai un activo --> ese.
// ** sácase a partir dos rangos e facendo ifs ... ** isto está sen definir aínda.
uint get_range_length_dual (void *iterator);
	// devolver o tamaño do rango que esté definido
	// - se hai cero variables instanciadas -> [1, n] (n e o numero de triples)
	// - se hai unha variable instanciada -> devolver r-l+1 (en calquera dos 2 rangos activos).
	// - se hai dúas variables instanciadas -> devolver r-l+1 no rango que esté activo.
	// - se hai tres variables instanciadas - nunca se vai chamar pero -> devolver 1, por se as moscas (otherwise). ;)


// para un iterador.
int is_last_level_dual(void *iterator) ;
	//returns 1 se hai >2 variables fixadas   numFijadas >=2
	//returns 0 otherwise (se hai 0 o 1 variables fixadas)


//  Dado un iterador que tiene 2 variables fijadas (is_last_level() devolvería 1).
//  devuelve un array de IDs de elementos de tipo "type" del elemento "no-fijado" asociado a ese rango.
//  el tipo no sería necesario, pero Adrián me lo pasa para que pueda verificar que coincide con el tipo del elemento NO-FIJADO. 
// (lo quitaremos para las pruebas).
std::vector <uint> get_all_dual (void *iterator, int type);
//{   std::vector <uint> res;
//    añadir a vector res -->    res.push_back( 3333);
//    return res;
//}


//inicializa recuperación de todos los IDS del tipo "type" del elemento "no fijado" asociado al iterador
//para ir devolviendo elementos como en get_all_dual(), pero de 1 en 1 en get_one_dual_next().
//returns -1 if no 2 variables are fixed ==> get_one_dual_next() should not be called.
int get_one_dual_init (void *iterator, int type);
//recupera el siguiente ID del rango "no fijado" en el iterador. 
//devuelve ID=0 cuando no quedan más elementos por devolver.
uint get_one_dual_next (void *iterator); 







/** DEBUGGING ONLY see files ops_spo.cpp, ops_so.cpp, ops_sp.cpp, ops_op.cpp ops_s.cpp ops_o.cpp ops_p.cpp ********/
void testAdrian(uint a, uint b);
int dual_test_spo_ops(void *index, uint **res);
int dual_test_so_os(void *index, uint **res, uint **res2) ;
int dual_test_sp_ps(void *index, uint **res, uint **res2) ;
int dual_test_sp_ps_TARGET_BIN_VS_FIRSTLEFT(void *index, uint **res, uint **res2);
int dual_test_po_op(void *index, uint **res, uint **res2) ;
int dual_test_s_s(void *index, uint **res, uint **res2);
int dual_test_p_p(void *index, uint **res, uint **res2);
int dual_test_o_o(void *index, uint **res, uint **res2);


/** more funcions defined in those ops_*.c files **/
/** called during initialization of intervals in interfaceDual.h --> buildFacade.cpp */

uint dual_rdfcsaSPO_init_spo (void *gindex, int S, int P, int O, uint *left, uint *right) ;  // see ops_spo.cpp
uint dual_rdfcsaOPS_init_so  (void *gindex, int S, int O, uint *left, uint *right) ;         // see ops_so.cpp
uint dual_rdfcsaSPO_init_sp  (void *gindex, int S, int P, uint *left, uint *right) ;         // see ops_sp.cpp
int dual_test_sp_ps_TARGET_BIN_VS_FIRSTLEFT(void *index, uint **res, uint **res2) ;         // see ops_sp.cpp
uint dual_rdfcsaSPO_init_po  (void *gindex, int P, int O, uint *left, uint *right) ;         // see ops_po.cpp

uint dual_rdfcsaSPO_init_s   (void *gindex, int S, uint *left, uint *right) ;         // see ops_s.cpp
uint dual_rdfcsaOPS_init_s   (void *gindex, int S, uint *left, uint *right) ;         // see ops_s.cpp
uint dual_rdfcsaSPO_init_p   (void *gindex, int P, uint *left, uint *right) ;         // see ops_p.cpp
uint dual_rdfcsaOPS_init_p   (void *gindex, int P, uint *left, uint *right) ;         // see ops_p.cpp
uint dual_rdfcsaSPO_init_o   (void *gindex, int O, uint *left, uint *right) ;         // see ops_o.cpp
uint dual_rdfcsaOPS_init_o   (void *gindex, int O, uint *left, uint *right) ;         // see ops_o.cpp




/** DEBUGING ONLY TESTS_INIT **/  // to be included in ops_spo.cpp
// NOT IMPLEMENTED YET.
int dual_test_init_spo_ops  (void *index);
int dual_test_init_so_os  (void *index);






/****************************/   //see buildFacade.cpp
void setColorRojo();
void setColorAzul();
void setColorMorado();
void setColorVerde();
void setColorAmarillo();
void setColorNormal();


#endif



