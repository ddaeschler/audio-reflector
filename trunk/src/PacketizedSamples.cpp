/*
 * PacketizedSamples.cpp
 *
 *  Created on: May 26, 2013
 *      Author: ddaeschler
 */

#include "PacketizedSamples.h"

namespace audioreflector
{
	PacketizedSamples::PacketizedSamples(size_t start, size_t count, int sampleRate,
			EncodedSamplesPtr sourceSamples, bool lastPacket) {

		assert(count <= packet_buffer::BUF_MAX_DATA);

		SampleRate = sampleRate;
		Size = count + HEADER_SZ;
		Samples = PacketBufferPool::getInstance().alloc();


		//Header
		// 			0				1 - 7
		//[LAST PKT INDICATOR 0/1][RESERVED]

		if (lastPacket) {
			Samples->contents[0] = 1;
		} else {
			Samples->contents[0] = 0;
		}

		std::memcpy((&Samples->contents[0]) + HEADER_SZ, (&sourceSamples->Samples->contents[0]) + start, count);
	}
}


