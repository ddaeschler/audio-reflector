/*
 * EncoderStage.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: ddaeschler
 */

#include "EncoderStage.h"

namespace audioreflector
{
	EncoderStage::EncoderStage(IEncoderPtr encoder) : _encoder(encoder)
	{

	}

	EncoderStage::~EncoderStage()
	{

	}

	void EncoderStage::enqueue(packet_buffer_ptr buffer, int numSamples, int sampleRate,
			short bitDepth, EncodeCompleteCallback callback)
	{
		EncoderQueueItem queueItem;
		queueItem.Buffer = buffer;
		queueItem.NumSamples = numSamples;
		queueItem.SampleRate = sampleRate;
		queueItem.BitDepth = bitDepth;
		queueItem.CallBack = callback;

		_waitingSamples.push(queueItem);
	}

	void EncoderStage::encodeNext()
	{
		if (_waitingSamples.size() > 0) {
			EncoderQueueItem topItem = _waitingSamples.front();
			_waitingSamples.pop();

			EncodedSamplesPtr samples = _encoder->encode(topItem.Buffer, topItem.NumSamples, topItem.SampleRate,
					topItem.BitDepth);

			topItem.CallBack(samples);
		}
	}
}
