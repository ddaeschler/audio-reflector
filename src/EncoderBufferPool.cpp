/*
 * EncoderBufferPool.cpp
 *
 *  Created on: May 25, 2013
 *      Author: ddaeschler
 */

#include "EncoderBufferPool.h"

#include <boost/bind.hpp>

namespace audioreflector {

EncoderBufferPool::EncoderBufferPool() {
	// TODO Auto-generated constructor stub

}

EncoderBufferPool::~EncoderBufferPool() {
	// TODO Auto-generated destructor stub
}

EncoderBufferPool& EncoderBufferPool::getInstance() {
	static EncoderBufferPool instance;
	return instance;
}

encoder_buffer_ptr EncoderBufferPool::alloc() {
	boost::mutex::scoped_lock lock(_bufferPoolLock);

	encoder_buffer_ptr buffer(_bufferPool.construct(),
			boost::bind(&EncoderBufferPool::freeBuffer, this, _1));

	return buffer;
}

void EncoderBufferPool::freeBuffer(encoder_buffer* buffer) {
	boost::mutex::scoped_lock lock(_bufferPoolLock);

	_bufferPool.destroy(buffer);
}



} /* namespace audioreflector */
