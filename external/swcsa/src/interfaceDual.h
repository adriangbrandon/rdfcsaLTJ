#ifndef _INTERFACE_DUAL
#define _INTERFACE_DUAL


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
 
/* General interface for using the compressed temporal graph */

// #ifndef uchar
// #define uchar unsigned char
// #endif
// #ifndef uint
// #define uint unsigned int
// #endif
// #ifndef ulong
// #define ulong unsigned long
// #endif


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



/* creates first the spo permutation and the reutilizes s to create the ops permutation index */
//int build_index_dual (struct graphDB *graph, char *build_options, void **index) ;
int build_index_dual (char * filename, char *build_options, void **index) ;  //   <--- Será así: Adrián Dixit !!


int save_index_dual (void *index, char *filename);
int load_index_dual (char *filename, void **index);
int free_index_dual (void *index);
int index_size_dual(void *index, ulong *size);
int get_length_dual(void *index, ulong *length);

int testRecoverAndCompareSPO_OPS(void *index);   //


//titerator * createIterator(void *index);
void * createIterator_dual(void *index);
	// inicializa os 2 rangos con [1,n], n=numero de triples.


int init_spo_dual(void *iterator, uint s, uint p, uint o);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamén valería)


int init_so_dual(void *iterator, uint s, uint o);
	//inicializa rdfcsa-ops
	//inicializa rdfcsa-spo non se usa. (tamen serviría se considero "O->S"... porque despois só podo facer down con P).

int init_sp_dual(void *iterator, uint s, uint p);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamen serviría se considero "P->S"... porque despois só podo facer down con o).
	
int init_po_dual(void *iterator, uint p, uint o);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops non se usa. (tamen serviría se considero "O->P"... porque despois só podo facer down con s).
	
int init_s_dual(void *iterator, uint s);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar p ou o
	
int init_p_dual(void *iterator, uint p);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar s ou o
	
int init_o_dual(void *iterator, uint p);
	//inicializa rdfcsa-spo
	//inicializa rdfcsa-ops
	//necesitanse os dous porque non sabemos se posteriormente se vai instanciar s ou p
	
	
//...
int leap_dual(void *iterator, int type, uint value);
	//type: indica se estamos a buscar nun s, p ou o
	//buscamos no rango do iterator o primeiro valor de tipo (type) >= value
	//OLLO: hai que controlar o seguinte:
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

void testAdrian(uint a, uint b);


#endif
