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

		PacketizedSamples(size_t start, size_t count, int sampleRate, EncodedSamplesPtr sourceSamples) {
			assert(count < packet_buffer::BUF_SZ);

			SampleRate = sampleRate;
			Size = count;
			Samples = PacketBufferPool::getInstance().alloc();

			std::memcpy(Samples->contents, (&sourceSamples->Samples->contents[0]) + start, count);
		}
	};

	typedef boost::shared_ptr<PacketizedSamples> PacketizedSamplesPtr;
}


#endif /* PACKETIZEDSAMPLES_H_ */
