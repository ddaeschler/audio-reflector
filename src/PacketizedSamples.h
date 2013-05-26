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

		PacketizedSamples(size_t start, size_t count, int sampleRate, EncodedSamplesPtr samples) {
			SampleRate = sampleRate;
			Size = count;
			Samples = PacketBufferPool::getInstance().alloc();
		}
	};

	typedef boost::shared_ptr<PacketizedSamples> PacketizedSamplesPtr;
}


#endif /* PACKETIZEDSAMPLES_H_ */
