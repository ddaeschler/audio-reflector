/*
 * EncoderStage.h
 *
 *  Created on: Dec 17, 2011
 *      Author: ddaeschler
 */

#ifndef ENCODERSTAGE_H_
#define ENCODERSTAGE_H_

#include <boost/function.hpp>
#include <queue>

#include "ARTypes.h"
#include "IEncoder.h"
#include "EncodedSamples.h"

namespace audioreflector
{
struct EncoderQueueItem;

typedef boost::function<void (EncodedSamplesPtr)> EncodeCompleteCallback;

/**
 * A work item for the encode stage
 */
struct EncoderQueueItem
{
public:
	packet_buffer_ptr Buffer;
	int NumSamples;
	int SampleRate;
	short BitDepth;
	EncodeCompleteCallback CallBack;
};

/**
 * The stage that takes the raw PCM and encodes it with the given module
 */
class EncoderStage
{
private:
	std::queue<EncoderQueueItem> _waitingSamples;
	IEncoderPtr _encoder;

public:
	EncoderStage(IEncoderPtr encoder);
	virtual ~EncoderStage();

	/**
	 * Enqueues a buffer of raw samples to be encoded
	 */
	void enqueue(packet_buffer_ptr buffer, int numSamples, int sampleRate,
			short BitDepth, EncodeCompleteCallback callback);

	/**
	 * Encodes the next available sample and runs the callback
	 */
	void encodeNext();
};

}

#endif /* ENCODERSTAGE_H_ */
