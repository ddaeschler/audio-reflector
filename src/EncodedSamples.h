/*
 * EncodedSamples.h
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#ifndef ENCODEDSAMPLES_H_
#define ENCODEDSAMPLES_H_

#include "ARTypes.h"
#include <boost/shared_ptr.hpp>

namespace audioreflector
{
	struct EncodedSamples
	{
		packet_buffer_ptr Samples;
		int NumSamples;
		int SampleRate;
		int BitDepth;
		int EncodedSize;
	};

	typedef boost::shared_ptr<EncodedSamples> EncodedSamplesPtr;
}


#endif /* ENCODEDSAMPLES_H_ */
