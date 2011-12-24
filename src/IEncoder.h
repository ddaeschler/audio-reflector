/*
 * IEncoder.h
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#ifndef IENCODER_H_
#define IENCODER_H_

#include "ARTypes.h"
#include "EncodedSamples.h"

#include <boost/shared_ptr.hpp>

namespace audioreflector
{

	class IEncoder
	{
	public:
		virtual ~IEncoder();


		virtual EncodedSamplesPtr encode(packet_buffer_ptr buffer, int numSamples, int sampleRate,
				short bitDepth) = 0;
	};

	typedef boost::shared_ptr<IEncoder> IEncoderPtr;
}


#endif /* IENCODER_H_ */
