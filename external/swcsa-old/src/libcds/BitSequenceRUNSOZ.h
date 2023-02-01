/* BitSequenceRUNSOZ.h
 * Copyright (C) 2008, Francisco Claude, all rights reserved.
 *
 * RUNSOZ Bitsequence -
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

#ifndef _STATIC_BITSEQUENCE_RUNSOZ_H
#define _STATIC_BITSEQUENCE_RUNSOZ_H

#include "libcdsBasics.h"
#include "BitString.h"
#include "BitSequence.h"
#include "TableOffsetRRR.h"

using namespace cds_utils;

namespace cds_static
{
    #define DEFAULT_C_BITSEQUENCE   SDARRAY_HDR
    #define DEFAULT_SAMPLING_RUNS0Z 64     //would depend on the method
  

	class BitSequenceRUNSOZ : public BitSequence
	{
		public:
			/** Builds an RUNSOZ bitsequence.
			 * @param bitseq bitstring
			 * @param len lenght of the bitstring
			 * @param type of bitSequence Representation
			 * @param sample_rate (if needed by bitSequence)
			 */
			BitSequenceRUNSOZ(uint * bitseq, size_t len, unsigned char bs_type = DEFAULT_C_BITSEQUENCE, uint sample_rate=DEFAULT_SAMPLING_RUNS0Z);

			/** Builds an RRR bitsequence.
			 * @param bs bitstring
			 * @param type of bitSequence Representation
			 * @param sample_rate (if needed by bitSequence)
			 */
			BitSequenceRUNSOZ(const BitString & bs, unsigned char bs_type = DEFAULT_C_BITSEQUENCE, uint sample_rate=DEFAULT_SAMPLING_RUNS0Z);

			BitSequenceRUNSOZ(uint * bitseq, size_t len, unsigned char bs_type);
			
			BitSequenceRUNSOZ(const BitString & bs, unsigned char bs_type);

			virtual ~BitSequenceRUNSOZ();

			virtual size_t rank0(const size_t i) const;

			virtual size_t rank1(const size_t i) const;

			virtual size_t select0(const size_t i) const;

			virtual size_t select1(const size_t i) const;

			virtual bool access(const size_t i) const;

			virtual size_t getSize() const;

			virtual void save(ofstream & fp) const;

			/** Reads the bitmap from a file pointer, returns NULL in case of error */
			static BitSequenceRUNSOZ * load(ifstream & fp);


		protected:
			/** Internal building function, same parameters as the base constructor. */
			void build(const uint * bitseq, size_t len,  unsigned char bs_type = DEFAULT_C_BITSEQUENCE, uint sample_rate=DEFAULT_SAMPLING_RUNS0Z);

			/** Protected constructor for loaders, you have to initialize data before
			 * using an object built with this constructor.
			 */
			BitSequenceRUNSOZ();

			virtual void generate_OZ(BitSequence *U, uint **O, uint **Z, size_t length);

			BitSequence *O, *Z;
			//size_t o_length, z_length;
			size_t b_length; //length of the bitmaps

			size_t right_bsearch;  //last j to search during bin_search_access_rank0_rank1
			              
			size_t ones;    //number of ones in the BitSequence   +1 extra 1 to mark the end of O
			size_t zeroes;  //number of zeroes in the BitSequence +1 extra 1 to mark the end of Z
                
			unsigned char bs_type;               
			unsigned char z1;    // If z1=0, then the first run contains "zeroes". See Gonzalo's book, page 86.
			/** Sample rate */
			uint sample_rate;  //if needed
			
			static const unsigned int  MAXINT=0x7fffffff;
		
	};

}
#endif							 /* _STATIC_BITSEQUENCE_RUNSOZ_H */
