/*
 * WavPackDecoder.h
 *
 *  Created on: Dec 26, 2011
 *      Author: ddaeschler
 */

#ifndef WAVPACKDECODER_H_
#define WAVPACKDECODER_H_

#include "IDecoder.h"
#include "ARTypes.h"

#include <wavpack/wavpack.h>

#include <boost/shared_array.hpp>

namespace audioreflector
{
	class WavPackDecoder : public IDecoder
	{
	private:
		static const int DECODE_BUFFER_SZ;

		boost::shared_array<int32_t> _decodeBuffer;
		boost::shared_array<char> _decodedPcm;
		WavpackStreamReader _streamReader;
		boost::shared_array<char> _errMsgStorage;
		WavpackContext* _context;

		const char* _data;
		std::size_t _dataSize;
		uint32_t _pos;

	public:
		WavPackDecoder();
		virtual ~WavPackDecoder();

		virtual decoded_samples decode(const char* data, std::size_t bytes);

		int32_t readBytes(void *data, int32_t bcount);
		uint32_t getLength();
		uint32_t getPos();
		int pushBackByte(char c);
	};
}


#endif /* WAVPACKDECODER_H_ */
