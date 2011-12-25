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
#include <boost/function.hpp>

namespace audioreflector
{
	typedef boost::function<void (EncodedSamplesPtr)> FramesReadyCallback;

	class IEncoder
	{
	public:
		virtual ~IEncoder();

		virtual void setFramesReadyCallback(FramesReadyCallback callBack) = 0;
		virtual void encode(packet_buffer_ptr buffer, int numSamples) = 0;
	};

	typedef boost::shared_ptr<IEncoder> IEncoderPtr;
}


#endif /* IENCODER_H_ */
