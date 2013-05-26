/*
 * WavPackEncoder.cpp
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#include "WavPackEncoder.h"

#include "EncoderBufferPool.h"

#include <boost/lexical_cast.hpp>
#include <stdexcept>
#include <iostream>
#include <vector>


namespace audioreflector
{
	WavPackEncoder::WavPackEncoder(int sampleRate)
		: _sampleRate(sampleRate), _tmpBuffer(0), _tmpBufferSize(0)
	{
		//typedef int (*WavpackBlockOutput)(void *id, void *data, int32_t bcount);
		_wpContext = WavpackOpenFileOutput(&WavPackEncoder::RawOutput, this, 0);

	    if (!_wpContext) {
	    	throw std::runtime_error("Unable to allocate wavpack context");
		}

	    _config = new WavpackConfig;
	    memset(_config, 0, sizeof(WavpackConfig));
	    _config->bytes_per_sample = BIT_DEPTH_IN_BYTES;
	    _config->bits_per_sample = BIT_DEPTH;
	    _config->sample_rate = sampleRate;
	    _config->channel_mask = 4;
	    _config->num_channels = 1;

	    _config->block_samples = WavPackEncoder::SAMPLES_PER_BLOCK;

	    WavpackSetConfiguration(_wpContext, _config, -1);
	    WavpackPackInit(_wpContext);
	}

	WavPackEncoder::~WavPackEncoder()
	{
	}

	void WavPackEncoder::setFramesReadyCallback(FramesReadyCallback callBack)
	{
		_framesReady = callBack;
	}

	void WavPackEncoder::encode(encoder_buffer_ptr buffer, int numSamples)
	{
		//upconvert the samples to 32 bit integers
		if (! _tmpBuffer || _tmpBufferSize < numSamples) {
			delete[] _tmpBuffer;
			_tmpBuffer = new int32_t[numSamples];
			_tmpBufferSize = numSamples;
		}

		for (int i = 0; i < numSamples; i++) {
			switch (BIT_DEPTH) {
				case 8:
					_tmpBuffer[i] = ((int8_t*)buffer->contents)[i];
					break;
				case 16:
					_tmpBuffer[i] = ((int16_t*)buffer->contents)[i];
					break;
			}
		}

		if (WavpackPackSamples(_wpContext, _tmpBuffer, numSamples) == false) {
			throw std::runtime_error("Unable to compress the given " +
					boost::lexical_cast<std::string>(numSamples) + " samples: " +
					WavpackGetErrorMessage(_wpContext));
		}
	}

	int WavPackEncoder::RawOutput(void *id, void *data, int32_t bcount)
	{
		if (id == 0) return (int) true;

		WavPackEncoder* encoder = (WavPackEncoder*)id;
		return encoder->onOutputAvailable(data, bcount);
	}

	int WavPackEncoder::onOutputAvailable(void* data, int32_t bcount)
	{
		std::cout << bcount << std::endl;

		if (bcount > encoder_buffer::BUF_SZ) {
			throw std::runtime_error("Wavpack output would overflow the encoder buffer");
		}

		encoder_buffer_ptr buffer(EncoderBufferPool::getInstance().alloc());
		memcpy(buffer->contents, data, bcount);

		EncodedSamplesPtr samples(new EncodedSamples);
		samples->SampleRate = _sampleRate;
		samples->EncodedSize = bcount;
		samples->Samples = buffer;

		_framesReady(samples);

		return (int) true;
	}
}
