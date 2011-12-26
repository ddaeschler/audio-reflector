/*
 * WavPackDecoder.cpp
 *
 *  Created on: Dec 26, 2011
 *      Author: ddaeschler
 */

#include "WavPackDecoder.h"

#include <cstring>

namespace audioreflector
{
	const int WavPackDecoder::DECODE_BUFFER_SZ = 2048;


	int32_t wpd_read_bytes_shim(void *id, void *data, int32_t bcount)
	{
		WavPackDecoder* d = (WavPackDecoder*)id;
		return d->readBytes(data, bcount);
	}

	uint32_t wpd_get_pos_shim(void *id)
	{
		WavPackDecoder* d = (WavPackDecoder*)id;
		return d->getPos();
	}

	int wpd_set_pos_abs_shim(void *id, uint32_t pos)
	{

	}

	int wpd_set_pos_rel_shim(void *id, int32_t delta, int mode)
	{

	}

	int wpd_push_back_byte_shim(void *id, int c)
	{
		WavPackDecoder* d = (WavPackDecoder*)id;
		return d->pushBackByte(c);
	}

	uint32_t wpd_get_length_shim(void *id)
	{
		WavPackDecoder* d = (WavPackDecoder*)id;
		return d->getLength();
	}

	int wpd_can_seek_shim(void *id)
	{
		return (int)false;
	}

	int32_t wpd_write_bytes_shim(void *id, void *data, int32_t bcount)
	{

	}

	WavPackDecoder::WavPackDecoder()
	: _decodeBuffer(new int32_t[DECODE_BUFFER_SZ]),
	  _decodedPcm(new char[DECODE_BUFFER_SZ * BIT_DEPTH_IN_BYTES]),
	  _errMsgStorage(new char[1024]),
	  _context(0),
	  _data(0),
	  _dataSize(0),
	  _pos(0)
	{
		_streamReader.can_seek = wpd_can_seek_shim;
		_streamReader.get_length = wpd_get_length_shim;
		_streamReader.get_pos = wpd_get_pos_shim;
		_streamReader.push_back_byte = wpd_push_back_byte_shim;
		_streamReader.read_bytes = wpd_read_bytes_shim;
		_streamReader.set_pos_abs = wpd_set_pos_abs_shim;
		_streamReader.set_pos_rel = wpd_set_pos_rel_shim;
		_streamReader.write_bytes = wpd_write_bytes_shim;
	}

	WavPackDecoder::~WavPackDecoder()
	{

	}

	decoded_samples WavPackDecoder::decode(const char* data, std::size_t bytes)
	{
		_data = data;
		_dataSize = bytes;
		_pos = 0;

		if (! _context) {
			_context = WavpackOpenFileInputEx(&_streamReader, this, 0, _errMsgStorage.get(), OPEN_STREAMING, 0);
			if (! _context) {
				throw std::runtime_error(std::string("Could not set up input stream: ") + _errMsgStorage.get());
			}
		}

		uint32_t sampleCount = WavpackUnpackSamples(_context, _decodeBuffer.get(), DECODE_BUFFER_SZ);
		uint32_t sampleCountFinal = sampleCount;

		int32_t* p = _decodeBuffer.get();
		char* outBytes = _decodedPcm.get();

		//unpack the samples back to original bit depth
		switch (BIT_DEPTH) {
		case 16:
			while (sampleCount > 0) {
				*((int16_t *)outBytes) = (int16_t)(*p);
				outBytes += sizeof (int16_t);
				p++;
				sampleCount--;
			}
			break;

		case 8:
			while (sampleCount > 0) {
				*outBytes++ = (char)(*p);
				p++;
				sampleCount--;
			}
			break;
		}

		decoded_samples ret;
		ret.NumSamples = sampleCountFinal;
		ret.SampleData = _decodedPcm.get();

		return ret;
	}

	int32_t WavPackDecoder::readBytes(void *data, int32_t bcount)
	{
		int remain = _dataSize - _pos;
		int amt = bcount < remain ? bcount : remain;

		memcpy(data, _data + _pos, amt);

		_pos += amt;

		return amt;
	}

	uint32_t WavPackDecoder::getLength()
	{
		return _dataSize;
	}

	uint32_t WavPackDecoder::getPos()
	{
		return _pos;
	}

	int WavPackDecoder::pushBackByte(char c)
	{
		if (_pos > 0) {
			--_pos;
		}

		return (int)c;
	}
}
