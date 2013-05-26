/*
 * PacketizedSamples.h
 *
 *  Created on: May 25, 2013
 *      Author: ddaeschler
 */

#ifndef PACKETIZEDSAMPLES_H_
#define PACKETIZEDSAMPLES_H_

#include "PacketBufferPool.h"
#include "EncodedSamples.h"

#include <cassert>

namespace audioreflector
{
	/**
	 * Samples that have been resized to be appropriate for sending
	 * to a remote destination
	 */
	struct PacketizedSamples
	{
		packet_buffer_ptr Samples;

		int SampleRate;
		size_t Size;

		/**
		 * When packetizing, this keeps tract of if this is the last packet of a single block.
		 * This allows us to properly support block based encoders
		 */
		bool IsLastPacket;

		PacketizedSamples(size_t start, size_t count, int sampleRate, EncodedSamplesPtr sourceSamples, bool lastPacket);
	};

	typedef boost::shared_ptr<PacketizedSamples> PacketizedSamplesPtr;
}


#endif /* PACKETIZEDSAMPLES_H_ */
