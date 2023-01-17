/*  BitSequenceBuilderSDArray.cpp
 * Copyright (C) 2009, Francisco Claude, all rights reserved.
 *
 * Francisco Claude <fclaude@cs.uwaterloo.ca>
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

#include "BitSequenceBuilderRUNSOZ.h"

namespace cds_static
{

	BitSequenceBuilderRUNSOZ::BitSequenceBuilderRUNSOZ() {
		this->bs_type = DEFAULT_C_BITSEQUENCE;
		this->sample_rate = DEFAULT_SAMPLING_RUNS0Z;
		switch	(this->bs_type) {
			case RRR02_HDR:   {this->sample_rate=DEFAULT_SAMPLING; break;}
			case SDARRAY_HDR: {this->sample_rate=DEFAULT_SAMPLING_RUNS0Z; break;}  //no sampling parameter needed.
			case DELTA_HDR:   {this->sample_rate=DEFAULT_SAMPLING_DELTA; break;}
			case BRW32_HDR:   {this->sample_rate=20; break;}
				//BRW32-HDR ==> RG-bitmap:: factor=2 => overhead 50%
				//BRW32-HDR ==> RG-bitmap:: factor=3 => overhead 33%
				//BRW32-HDR ==> RG-bitmap:: factor=4 => overhead 25%
				//BRW32-HDR ==> RG-bitmap:: factor=20=> overhead 5%	   //DEFAULT HERE!	
		}		
	}

	BitSequenceBuilderRUNSOZ::BitSequenceBuilderRUNSOZ(unsigned char bs_type) {
		this->bs_type = bs_type;
		this->sample_rate = DEFAULT_SAMPLING_RUNS0Z;
		switch	(this->bs_type) {
			case RRR02_HDR:   {this->sample_rate=DEFAULT_SAMPLING; break;}
			case SDARRAY_HDR: {this->sample_rate=DEFAULT_SAMPLING_RUNS0Z; break;}  //no sampling parameter needed.
			case DELTA_HDR:   {this->sample_rate=DEFAULT_SAMPLING_DELTA; break;}
			case BRW32_HDR:   {this->sample_rate=20; break;}
				//BRW32-HDR ==> RG-bitmap:: factor=2 => overhead 50%
				//BRW32-HDR ==> RG-bitmap:: factor=3 => overhead 33%
				//BRW32-HDR ==> RG-bitmap:: factor=4 => overhead 25%
				//BRW32-HDR ==> RG-bitmap:: factor=20=> overhead 5%	   //DEFAULT HERE!	
		}		
		
	}


	BitSequenceBuilderRUNSOZ::BitSequenceBuilderRUNSOZ(unsigned char bs_type, unsigned int sample_rate) {
		this->bs_type = bs_type;
		this->sample_rate = sample_rate;
	}

	BitSequenceBuilderRUNSOZ::~BitSequenceBuilderRUNSOZ() {
	}

	BitSequence * BitSequenceBuilderRUNSOZ::build(uint * bitseq, size_t len) const
	{
		return new BitSequenceRUNSOZ(bitseq,len,bs_type,sample_rate);
	}

	BitSequence * BitSequenceBuilderRUNSOZ::build(const BitString & bs) const
	{
		return new BitSequenceRUNSOZ(bs,bs_type,sample_rate);
	}

};
