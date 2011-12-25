/*
 * WavPackEncoder.h
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#ifndef WAVPACKENCODER_H_
#define WAVPACKENCODER_H_

#include "IEncoder.h"

#include <wavpack/wavpack.h>

namespace audioreflector
{
	class WavPackEncoder : public IEncoder
	{
	private:
		WavpackContext* _wpContext;
		WavpackConfig* _config;
		int _sampleRate;
		FramesReadyCallback _framesReady;

		int32_t* _tmpBuffer;
		int _tmpBufferSize;

	public:
		WavPackEncoder(int sampleRate);
		virtual ~WavPackEncoder();

		virtual void setFramesReadyCallback(FramesReadyCallback callBack);
		virtual void encode(packet_buffer_ptr buffer, int numSamples);

	private:
		static int RawOutput(void *id, void *data, int32_t bcount);
		int onOutputAvailable(void* data, int32_t bcount);
	};
}


#endif /* WAVPACKENCODER_H_ */
