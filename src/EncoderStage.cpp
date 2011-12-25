/*
 * EncoderStage.cpp
 *
 *  Created on: Dec 17, 2011
 *      Author: ddaeschler
 */

#include "EncoderStage.h"

namespace audioreflector
{
	EncoderStage::EncoderStage(IEncoderPtr encoder, EncodeCompleteCallback callBack)
	: _encoder(encoder), _callBack(callBack)
	{
		_encoder->setFramesReadyCallback(boost::bind(&EncoderStage::onEncodedFramesReady,
				this, _1));
	}

	EncoderStage::~EncoderStage()
	{

	}

	void EncoderStage::enqueue(packet_buffer_ptr buffer, int numSamples, int sampleRate)
	{
		EncoderQueueItem queueItem;
		queueItem.Buffer = buffer;
		queueItem.NumSamples = numSamples;
		queueItem.SampleRate = sampleRate;

		_waitingSamples.push(queueItem);
	}

	void EncoderStage::encodeNext()
	{
		if (_waitingSamples.size() > 0) {
			EncoderQueueItem topItem = _waitingSamples.front();
			_waitingSamples.pop();

			_encoder->encode(topItem.Buffer, topItem.NumSamples);
		}
	}

	void EncoderStage::onEncodedFramesReady(EncodedSamplesPtr samples)
	{
		_callBack(samples);
	}
}
