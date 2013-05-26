/*
 * EncoderBufferPool.h
 *
 *  Created on: May 25, 2013
 *      Author: ddaeschler
 */

#ifndef ENCODERBUFFERPOOL_H_
#define ENCODERBUFFERPOOL_H_

#include "ARTypes.h"
#include "EncoderBuffer.h"

#include <boost/pool/object_pool.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

namespace audioreflector {

	class EncoderBufferPool
	{
	public:
		static EncoderBufferPool& getInstance();

		encoder_buffer_ptr alloc();

	private:
		boost::object_pool<encoder_buffer> _bufferPool;
		boost::mutex _bufferPoolLock;

		void freeBuffer(encoder_buffer* buffer);

		EncoderBufferPool();
		virtual ~EncoderBufferPool();
	};

} /* namespace audioreflector */
#endif /* ENCODERBUFFERPOOL_H_ */
