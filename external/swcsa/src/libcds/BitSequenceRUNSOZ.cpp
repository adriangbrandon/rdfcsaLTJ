/* BitSequenceRUNSOZ.cpp
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * BitSequenceRUNSOZ definition
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

#include "BitSequenceRUNSOZ.h"

namespace cds_static
{

	BitSequenceRUNSOZ::BitSequenceRUNSOZ() {
		ones=z1= 0;
		zeroes=0;
		length=0;		
		Z = NULL;
		O = NULL;	
		bs_type = DEFAULT_C_BITSEQUENCE;	
		
		sample_rate = DEFAULT_SAMPLING_RUNS0Z;
		switch	(bs_type) {
			case RRR02_HDR:   {sample_rate=DEFAULT_SAMPLING; break;}
			case SDARRAY_HDR: {sample_rate=DEFAULT_SAMPLING_RUNS0Z; break;}  //no sampling parameter needed.
			case DELTA_HDR:   {sample_rate=DEFAULT_SAMPLING_DELTA; break;}
			case BRW32_HDR:   {sample_rate=20; break;}
				//BRW32-HDR ==> RG-bitmap:: factor=2 => overhead 50%
				//BRW32-HDR ==> RG-bitmap:: factor=3 => overhead 33%
				//BRW32-HDR ==> RG-bitmap:: factor=4 => overhead 25%
				//BRW32-HDR ==> RG-bitmap:: factor=20=> overhead 5%	   //DEFAULT HERE!	
		}
		
	}

	BitSequenceRUNSOZ::BitSequenceRUNSOZ(uint * bitseq, size_t len, unsigned char bs_type, uint sample_rate) {
		build(bitseq,len, bs_type, sample_rate);
	}
	
	BitSequenceRUNSOZ::BitSequenceRUNSOZ(uint * bitseq, size_t len, unsigned char bs_type) {

		sample_rate = DEFAULT_SAMPLING_RUNS0Z;
		switch	(bs_type) {
			case RRR02_HDR:   {sample_rate=DEFAULT_SAMPLING; break;}
			case SDARRAY_HDR: {sample_rate=DEFAULT_SAMPLING_RUNS0Z; break;}  //no sampling parameter needed.
			case DELTA_HDR:   {sample_rate=DEFAULT_SAMPLING_DELTA; break;}
			case BRW32_HDR:   {sample_rate=20; break;}
				//BRW32-HDR ==> RG-bitmap:: factor=2 => overhead 50%
				//BRW32-HDR ==> RG-bitmap:: factor=3 => overhead 33%
				//BRW32-HDR ==> RG-bitmap:: factor=4 => overhead 25%
				//BRW32-HDR ==> RG-bitmap:: factor=20=> overhead 5%	   //DEFAULT HERE!	
		}
		
		build(bitseq, len, bs_type, sample_rate);

	}

	BitSequenceRUNSOZ::BitSequenceRUNSOZ(const BitString & bs, unsigned char bs_type, uint sample_rate) {
		build(bs.getData(),bs.getLength(),bs_type, sample_rate);
	}

	BitSequenceRUNSOZ::BitSequenceRUNSOZ(const BitString & bs, unsigned char bs_type) {
		sample_rate = DEFAULT_SAMPLING_RUNS0Z;
		switch	(bs_type) {
			case RRR02_HDR:   {sample_rate=DEFAULT_SAMPLING; break;}
			case SDARRAY_HDR: {sample_rate=DEFAULT_SAMPLING_RUNS0Z; break;}  //no sampling parameter needed.
			case DELTA_HDR:   {sample_rate=DEFAULT_SAMPLING_DELTA; break;}
			case BRW32_HDR:   {sample_rate=20; break;}
				//BRW32-HDR ==> RG-bitmap:: factor=2 => overhead 50%
				//BRW32-HDR ==> RG-bitmap:: factor=3 => overhead 33%
				//BRW32-HDR ==> RG-bitmap:: factor=4 => overhead 25%
				//BRW32-HDR ==> RG-bitmap:: factor=20=> overhead 5%	   //DEFAULT HERE!	
		}
		
		build(bs.getData(),bs.getLength(),bs_type, sample_rate);
	}

void printBS(uint *v, uint len, char *hdr) {
	
	uint i;
	printf("\n print bitmap ----------\n%s",hdr);
	for (i=0;i<len;i++) {
		uint val = (bitget_lcds(v,i));		
		 printf(" %u", val);
	}
	printf("\n");
}


	void BitSequenceRUNSOZ::build(const uint * bitseq, size_t len,  unsigned char bs_type, uint sample_rate) 
	{
		if(bs_type!=RRR02_HDR && bs_type!=SDARRAY_HDR && bs_type!=DELTA_HDR && bs_type!=BRW32_HDR){
			cout << "Error: op_rs must be RRR02_HDR or SDARRAY_HDR OR DELTA_HDR OR BRW32_HDR\n" << endl;
			exit(1);
		}	

		this->b_length= len;
		this->Z = NULL;
		this->O = NULL;		
		this->sample_rate = sample_rate;
		this->bs_type = bs_type;				

		uint *O_aux, *Z_aux;

		// These are set within generate_OZ() function.
		this->ones=0;
		this->z1  = 0;		

		// copy bitseq into aux array to create a BitSequenceRRR over it.
			long long nb = 1;
			nb = (nb*len+Wlcds-1)/Wlcds;
			if(nb > MAXINT){
				cout << "Memory limit excess (in LCP)" << endl;
				exit(1);
			}
			
		cout << " b_length = " << b_length << ", nb = " << nb << endl;
		
		uint *aux = new uint[(uint)nb];

		    {uint i;
		     for (i=0; i< nb; i++)
		       aux[i] = bitseq[i];
			}
				
//		BitSequence *bs_aux = new BitSequenceRRR(aux, len);
		BitSequence *bs_aux = new BitSequenceRG(aux, len,2);
		generate_OZ(bs_aux, &O_aux, &Z_aux, b_length);
		this->zeroes = this->b_length- this->ones;
		
		zeroes +=2; //adds two extra 1 at the end or Z (was added within generateOZ)
		ones+=2;    //adds one extra 1 at the end or Z (was added within generateOZ)
		
		//printBS(O_aux,ones,"O:");
		//printBS(Z_aux,zeroes, "Z:");
		

		//delete (BitSequenceRRR *) bs_aux;
		delete (BitSequence *) bs_aux;
		delete [] aux;
		
/*		
		if(bs_type==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, b_length);
			Z = new BitSequenceRRR(Z_aux, b_length);
		}
		else if (bs_type==SDARRAY_HDR){
			O = new BitSequenceSDArray(O_aux, b_length);
			Z = new BitSequenceSDArray(Z_aux, b_length);
		}
		else {  
			O = new BitSequenceDelta(O_aux, b_length);
			Z = new BitSequenceDelta(Z_aux, b_length);
		}
		*/


		if(bs_type==BRW32_HDR){
			O = new BitSequenceRG(O_aux, ones, sample_rate);
			Z = new BitSequenceRG(Z_aux, zeroes, sample_rate);
		}
		else if(bs_type==RRR02_HDR){
			O = new BitSequenceRRR(O_aux, ones, sample_rate);
			Z = new BitSequenceRRR(Z_aux, zeroes, sample_rate);
		}
		else if (bs_type==SDARRAY_HDR){
			O = new BitSequenceSDArray(O_aux, ones);
			Z = new BitSequenceSDArray(Z_aux, zeroes);
		}
		else {  
			O = new BitSequenceDelta(O_aux, ones, sample_rate);
			Z = new BitSequenceDelta(Z_aux, zeroes, sample_rate);
		}		 
		
		delete [] O_aux;
		delete [] Z_aux;
		
		
		right_bsearch = max(O->rank1(ones-1-2), Z->rank1(zeroes-1-2));
	}


// B : 0 0 0 0 1 1 1 1 1 1 1 1 0 0 1 0 1 1 1 1
// O : 1 0 0 0 0 0 0 0 1 1 0 0 0 1
// Z : 1 0 0 0 1 0 1 1
//     - - - - - - - - - - - - - - - - - - - - 
//pos: 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9  


	bool BitSequenceRUNSOZ::access(const size_t i) const
	{
	//  // Seq Search
	//	size_t j=2;	
	//	while ( (Z->select1(j) + O->select1(j) ) <= i ) j++;
	//	j--;	
	//	cout << "j after seq search = " << j<< endl;
		
		//bin search
		size_t j=1;
		size_t l=1, r=right_bsearch+1;
		size_t sumZselOsel=0;
		while (l<r) {
			j=(l+r)/2;
			sumZselOsel = Z->select1(j) + O->select1(j) ;
			if (sumZselOsel < i)
				l=j+1;
			else if (sumZselOsel ==i)
				r=l=j+1;				
			else
				r=j;			
		}
		j=l-1;		
	    //	cout << "j after bsearch = " << j<< " ::[l,r] = "<< "[" << l << "," << r<<"]"<< " sumZselOsel=" << sumZselOsel << endl;
		// end bin search
		
	
		
		if (z1>0) {			
			if (i< ( Z->select1(j+1) + O->select1(j)  ) ) 
			    return 0;         //B[i] in jth run of zeroes
			else
				return 1;  //B[i] in jth run of ones
		}
		else{
			if (i< ( O->select1(j+1) + Z->select1(j)  ) ) 
			    return 1;    //B[i] in jth run of ones			
			else
				return 0;       //B[i] in jth run of zeroes
		}
		
		return -1;
	}


	size_t BitSequenceRUNSOZ::rank1(const size_t i) const
	{
	//  // Seq Search
	//	size_t j=2;	
	//	while ( (Z->select1(j) + O->select1(j) ) <= i ) j++;
	//	j--;	
	//	cout << "j after seq search = " << j<< endl;
		
		//bin search
		size_t j=1;
		size_t l=1, r=right_bsearch+1;
		size_t sumZselOsel=0;
		while (l<r) {
			j=(l+r)/2;
			sumZselOsel = Z->select1(j) + O->select1(j) ;
			if (sumZselOsel < i)
				l=j+1;
			else if (sumZselOsel ==i)
				r=l=j+1;				
			else
				r=j;			
		}
		j=l-1;		
	    //	cout << "j after bsearch = " << j<< " ::[l,r] = "<< "[" << l << "," << r<<"]"<< " sumZselOsel=" << sumZselOsel << endl;
		// end bin search
		
		if (z1>0) {			
			if (i< ( Z->select1(j+1) + O->select1(j)  ) ) 
			    return O->select1(j) ;//-1;         //B[i] in jth run of zeroes
			else
				return i - Z->select1(j+1) +1;  //B[i] in jth run of ones
		}
		else{
			if (i< ( O->select1(j+1) + Z->select1(j)  ) ) 
			    return i - Z->select1(j) +1;    //B[i] in jth run of ones			
			else
				return O->select1(j+1) ;//-1;       //B[i] in jth run of zeroes
		}
		
		printf("\n rank1 %zu: SHOULD NOT GET UP TO HERE",i);
		return -1;
	}	
	
	
	size_t BitSequenceRUNSOZ::rank0(size_t i) const
	{
	//  // Seq Search
	//	size_t j=2;	
	//	while ( (Z->select1(j) + O->select1(j) ) <= i ) j++;
	//	j--;	
	//	cout << "j after seq search = " << j<< endl;
		
		//bin search
		size_t j=1;
		size_t l=1, r=right_bsearch+1;
		size_t sumZselOsel=0;
		while (l<r) {
			j=(l+r)/2;
			sumZselOsel = Z->select1(j) + O->select1(j) ;
			if (sumZselOsel < i)
				l=j+1;
			else if (sumZselOsel ==i)
				r=l=j+1;				
			else
				r=j;			
		}
		j=l-1;		
	    //	cout << "j after bsearch = " << j<< " ::[l,r] = "<< "[" << l << "," << r<<"]"<< " sumZselOsel=" << sumZselOsel << endl;
		// end bin search
		
		if (z1>0) {			
			if (i< ( Z->select1(j+1) + O->select1(j)  ) ) 
			    return i - O->select1(j) +1 ;         //B[i] in jth run of zeroes
			else
				return Z->select1(j+1) ;//-1;;  //B[i] in jth run of ones
		}
		else{
			if (i< ( O->select1(j+1) + Z->select1(j)  ) ) 
			    return Z->select1(j) ;//-1;     //B[i] in jth run of ones			
			else
				return i - O->select1(j+1) +1;       //B[i] in jth run of zeroes
		}
		
		printf("\n rank1 %zu: SHOULD NOT GET UP TO HERE",i);
		return -1;
	}	

/*
 
	bool BitSequenceRUNSOZ::rank1xx(const size_t i) const
	{
		size_t j=1;
		
		while ( (Z->select1(j) + O->select1(j) -1) <= i ) j++;
		
		if (z1>0) {			
			if (i< ( Z->select1(j+1) + O->select1(j) -1 ) ) 
			    return O->select1(j) -1;         //B[i] in jth run of zeroes
			else
				return i - Z->select1(j+1) +1;  //B[i] in jth run of ones
		}
		else{
			if (i< ( O->select1(j+1) + Z->select1(j) -1 ) ) 
			    return i - Z->select1(j) +1;    //B[i] in jth run of ones			
			else
				return O->select1(j+1) -1;       //B[i] in jth run of zeroes
		}
		
		return -1;
	}
	
*/	

/*
	bool BitSequenceRUNSOZ::access(const size_t i) const
	{
		size_t j=1;
		
		while ( (Z->select1(j) + O->select1(j) -1) <= i ) j++;
		
		if (z1>0) {			
			if (i< ( Z->select1(j+1) + O->select1(j) -1 ) ) 
			    return 0;         //B[i] in jth run of zeroes
			else
				return 1;  //B[i] in jth run of ones
		}
		else{
			if (i< ( O->select1(j+1) + Z->select1(j) -1 ) ) 
			    return 1;    //B[i] in jth run of ones			
			else
				return 0;       //B[i] in jth run of zeroes
		}
		
		return -1;
	}

	size_t BitSequenceRUNSOZ::rank0(size_t i) const
	{
		//if(i+1==0) return 0;
		//return 1+i-rank1(i);
	}
*/
//	size_t BitSequenceRUNSOZ::rank1(size_t i) const
//	{
//		//if(i+1==0) return 0;
//		//						 // patch for 32-64 bits_lcds bad coding, to be removed in the future
//		//if((uint)(i+1)==0) return 0;
//	
//	}

	size_t BitSequenceRUNSOZ::select0(size_t i) const
	{
		if(i==0) return -1;
		if(i>zeroes) return -1;

		size_t rnk;
		size_t sel;
		if (z1!=0) {
			rnk= Z->rank1(i-1);
			sel = O->select1(rnk);
		}
		else {
			rnk= Z->rank1(i-1);
			sel = O->select1(rnk+1);		
		}
				

		//assert(rank0(i + sel -1)==i);
		//assert(!access(i + sel -1));
		return i + sel -1;
		//return pos;
	}



	size_t BitSequenceRUNSOZ::select1(size_t i) const
	{
		if(i==0) return -1;
		if(i>ones) return -1;
		
		size_t rnk;
		size_t sel;
		if (z1==0) {
			rnk= O->rank1(i-1);
			sel = Z->select1(rnk);
		}
		else {
			rnk= O->rank1(i-1);
			//sel=s1Z(rnk+1);	
			sel = Z->select1(rnk+1);		
		}
		return i + sel -1;

		
		//assert(acc==i);
		//assert(rank1(pos)==i);
		//assert(access(pos));
		//return pos;
	}

	// bool show_once = false;
	size_t BitSequenceRUNSOZ::getSize() const
	{
		size_t sum = sizeof(BitSequenceRUNSOZ);
		sum += Z->getSize();
		sum += O->getSize();
		
		return sum;
	}

	BitSequenceRUNSOZ::~BitSequenceRUNSOZ() {
		if(O!= NULL)
			delete (BitSequence *) O;

		if(Z!= NULL)
			delete (BitSequence *) Z;
/*
		if(O!= NULL){
			if(bs_type == BRW32_HDR)
				delete (BitSequenceRG *)O;
			else if(bs_type == RRR02_HDR)
				delete (BitSequenceRRR *)O;
			else if(bs_type == SDARRAY_HDR)
				delete (BitSequenceSDArray *)O;
			else if (bs_type == DELTA_HDR) 
				delete (BitSequenceDelta *)O;
		}
		if(Z!=NULL){
			if(bs_type == BRW32_HDR)
				delete (BitSequenceRG *)Z;			
			else if(bs_type == RRR02_HDR)
				delete (BitSequenceRRR *)Z;
			else if(bs_type == SDARRAY_HDR)
				delete (BitSequenceSDArray *)Z;
			else if (bs_type == DELTA_HDR) 
				delete (BitSequenceDelta *)Z;
		}
		*/ 
	}



	void BitSequenceRUNSOZ::save(ofstream & f) const
	{		
		uint wr = RUNSOZ_HDR;
		//cout << "pos SAVE wr RUNSOZ=" <<f.tellp() << endl;
		saveValue(f,wr);
		
		saveValue(f,b_length);
		saveValue(f,ones);
		saveValue(f,zeroes);
		saveValue(f,right_bsearch);
		
		saveValue(f,bs_type);
		saveValue(f,z1);
		saveValue(f,sample_rate);
		
		O->save(f);
		Z->save(f);				
		
		cout << "b_length    = " << this->b_length << endl;		
		cout << "ones        = " << this->ones << endl;
		cout << "z1          = " << (size_t)this->z1 << endl;
		cout << "right_bsearch=" << this->right_bsearch << endl;
		cout << "bs_type     = " << (size_t)this->bs_type << ": "; //  << endl;

		if (bs_type == BRW32_HDR ) 
			cout << "BRW32_HDR[RG]" << endl;
		else if (bs_type == RRR02_HDR) 
			cout << "RRR02_HDR" << endl;
		else if (bs_type == SDARRAY_HDR) 
			cout << "SDARRAY_HDR" << endl;
		else if (bs_type == DELTA_HDR) 
			cout << "DELTA_HDR" << endl;
		else 
			cout << "UNDEFINED BITSEQUENCE" << endl;
		
		cout << "sample_rate = " << this->sample_rate << endl;
		cout << "bs.getSize() = " << this->getSize() << endl;
		cout << "-- --------------------------------- --" << endl;		
		cout << "-- BitSequenceRUNSOZ.SAVE() COMPLETE --" << endl;
		cout << "-- --------------------------------- --" << endl;		
	}

	BitSequenceRUNSOZ * BitSequenceRUNSOZ::load(ifstream & f) {
		BitSequenceRUNSOZ * ret = NULL;
		try
		{
			ret = new BitSequenceRUNSOZ();
//			cout << "pos load type RUNSOZ=" <<f.tellg() << endl;
			
			uint type = loadValue<uint>(f);
			// TODO:throw an exception!
			if(type!=RUNSOZ_HDR) {
				abort();
			}

			ret->b_length = loadValue<size_t>(f);
			ret->ones = loadValue<size_t>(f);
			ret->zeroes = loadValue<size_t>(f);
			

			ret->right_bsearch = loadValue<size_t>(f);
			ret->bs_type = loadValue<unsigned char>(f);
			ret->z1 = loadValue<unsigned char>(f);
			ret->sample_rate = loadValue<uint>(f);


			cout << "b_length    = " << ret->b_length << endl;
			cout << "ones        = " << ret->ones << endl;
			cout << "z1          = " << (size_t)ret->z1 << endl;			
			cout << "right_bsearch=" << ret->right_bsearch << endl;			
			cout << "bs_type     = " << (size_t)ret->bs_type << ": "; //  << endl;

			if (ret->bs_type == BRW32_HDR ) 
				cout << "BRW32_HDR[RG]" << endl;			
			else if (ret->bs_type == RRR02_HDR) 
				cout << "RRR02_HDR" << endl;
			else if (ret->bs_type == SDARRAY_HDR) 
				cout << "SDARRAY_HDR" << endl;
			else if (ret->bs_type == DELTA_HDR) 
				cout << "DELTA_HDR" << endl;
			else 
				cout << "UNDEFINED BITSEQUENCE" << endl;
							
			cout << "sample_rate = " << ret->sample_rate << endl;
			cout << "-- --------------------------------- --" << endl;

						
			switch (ret->bs_type) {
				case BRW32_HDR: {ret->O = BitSequenceRG::load(f); break;}
				case DELTA_HDR: {ret->O = BitSequenceDelta::load(f); break;}
				case RRR02_HDR: {ret->O = BitSequenceRRR::load(f); break;}
				case SDARRAY_HDR: {ret->O = BitSequenceSDArray::load(f); break;}
			}

			switch (ret->bs_type) {
				case BRW32_HDR: {ret->Z = BitSequenceRG::load(f); break;}
				case DELTA_HDR: {ret->Z = BitSequenceDelta::load(f); break;}
				case RRR02_HDR: {ret->Z = BitSequenceRRR::load(f); break;}
				case SDARRAY_HDR: {ret->Z = BitSequenceSDArray::load(f); break;}
			}
			cout << "-- BitSequenceRUNSOZ.LOAD() COMPLETE --" << endl;
			cout << "-- --------------------------------- --" << endl;
			
//			ret->O = BitSequence::load(f);
//			ret->Z = BitSequence::load(f);


			return ret;
		}
		catch(exception e) {
			delete ret;
		}
		return NULL;
	}
	

	void BitSequenceRUNSOZ ::generate_OZ(BitSequence *U, uint **O, uint **Z, size_t length){
			uint *o, *z;
			bool bit_1 = true; //tell if we are counting 1 bits or 0 bits
			uint cont_1 = 0;
			uint cont_0 = 0;
			long long nb = 1;
			nb = (nb*(length+2)+Wlcds-1)/Wlcds;
			if(nb > MAXINT){
				cout << "Memory limit excess (in generate_OZ)" << endl;
				exit(1);
			}
			o = new uint[(uint)nb];
			z = new uint[(uint)nb];

			for(uint i=0; i< (uint)nb; i++){
				o[i]=0;
				z[i]=0;
			}

/*
			if (U->access(0)) {
				z1=0;
			}
			else {
				z1=1;
			}
*/
			uint i=0; 
			while (i<length) {
				if(U->access(i)){
					cont_1++;
					bitset_lcds(o, cont_1 - 1);
					i++;
					while (i<length && U->access(i)) {
						cont_1++;
						i++;
					}
				}
				else i++;				
			}

			i=0;
			while (i<length) {
				if(!U->access(i)){
					cont_0++;
					bitset_lcds(z, cont_0 - 1);
					i++;
					while (i<length && !U->access(i)) {
						cont_0++;
						i++;
					}
				}
				else i++;				
			}

			bitset_lcds(z, cont_0);    //@@ adds an extra 1 at the end of z (so that select works fine at the end) 
			bitset_lcds(z, cont_0+1);  //@@ adds an extra 1 at the end of z (so that select works fine at the end)
			 
			bitset_lcds(o, cont_1);    //@@ adds an extra 1 at the end of o (so that select works fine at the end)
			bitset_lcds(o, cont_1+1);  //@@ adds an extra 1 at the end of o (so that select works fine at the end)

	
			*O = o;
			*Z = z;

			// sets ones, and z1
			this->ones = cont_1;
			this->z1  = (U->access(0)==0);  // If z1=0, then the first run contains "zeroes".
			
//@			uint j;
//@			for (j=0; j<length; j++)
//@			  cout << "U-->ACCESS("<<j <<" ) = " << U->access(j) << endl;
			
		}



/*  //@@From Alberto's code!

	void BitSequenceRUNSOZ ::generate_OZ2(BitSequence *U, uint **O, uint **Z, size_t length){
			uint *o, *z;
			bool bit_1 = true; //tell if we are counting 1 bits or 0 bits
			uint cont_1 = 0;
			uint cont_0 = 0;
			long long nb = 1;
			nb = (nb*length+Wlcds-1)/Wlcds;
			if(nb > MAXINT){
				cout << "Memory limit excess (in LCP)" << endl;
				exit(1);
			}
			o = new uint[(uint)nb];
			z = new uint[(uint)nb];

			for(uint i=0; i< (uint)nb; i++){
				o[i]=0;
				z[i]=0;
			}

			uint i=0;
			if (U->access(i)) {
				z1=0;
				bit_1=true;
				cont_1++;
			}
			else {
				z1=1;
				bit_1=false;
				cont_0++;
			}
			i++;
		
			for(i; i<length; i++){
				if( U->access(i)){
					if(bit_1)
						cont_1++;
					else{
						bit_1 = true;
						cont_1++;
						//bit_set(z, cont_0 - 1);
						bitset_lcds(z, cont_0 - 1);						
					}
				}
				else{
					if(bit_1){
						bit_1 = false;
						cont_0++;
						//bit_set(o, cont_1 - 1);
						bitset_lcds(o, cont_1 - 1);
					}
					else
						cont_0++;
				}
			}
			
			if(bit_1) {
				//bit_set(o, cont_1 - 1);
				bitset_lcds(o, cont_1 - 1);
			}
			else{
				//bit_set(z, cont_0 - 1);
				bitset_lcds(z, cont_0 - 1);
			}
			*O = o;
			*Z = z;

			// sets ones, and z1
			this->ones = cont_1;
			this->z1  = (U->access(0)==0);  // If z1=0, then the first run contains "zeroes".
			
			uint j;
			for (j=0; j<length; j++)
			  cout << "U-->ACCESS("<<j <<" ) = " << U->access(j) << endl;
			
		}
*/	
	
	
	
}
