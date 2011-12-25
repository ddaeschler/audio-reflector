/*
 * EncoderStage.h
 *
 *  Created on: Dec 17, 2011
 *      Author: ddaeschler
 */

#ifndef ENCODERSTAGE_H_
#define ENCODERSTAGE_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <queue>

#include "ARTypes.h"
#include "EncodedSamples.h"
#include "IEncoder.h"

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
		EncodeCompleteCallback _callBack;

		boost::shared_ptr<boost::thread> _encoderThread;

	public:
		EncoderStage(IEncoderPtr encoder, EncodeCompleteCallback callBack);
		virtual ~EncoderStage();

		/**
		 * Enqueues a buffer of raw samples to be encoded
		 */
		void enqueue(packet_buffer_ptr buffer, int numSamples, int sampleRate);

		/**
		 * Starts the encoder thread
		 */
		void start();


	private:
		void run();

		void encodeNext();

		void onEncodedFramesReady(EncodedSamplesPtr samples);
	};

	typedef boost::shared_ptr<EncoderStage> EncoderStagePtr;
}

#endif /* ENCODERSTAGE_H_ */
