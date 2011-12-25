/*
 * PacketBufferPool.cpp
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#include "PacketBufferPool.h"

#include <boost/bind.hpp>

namespace audioreflector
{
	PacketBufferPool::PacketBufferPool()
	{

	}

	PacketBufferPool::~PacketBufferPool()
	{

	}

	PacketBufferPool& PacketBufferPool::getInstance()
	{
		static PacketBufferPool instance;
		return instance;
	}

	packet_buffer_ptr PacketBufferPool::alloc()
	{
		boost::mutex::scoped_lock lock(_bufferPoolLock);

		packet_buffer_ptr buffer(_bufferPool.construct(),
				boost::bind(&PacketBufferPool::freePacketBuffer, this, _1));

		return buffer;
	}

	void PacketBufferPool::freePacketBuffer(packet_buffer* buffer)
	{
		boost::mutex::scoped_lock lock(_bufferPoolLock);

		_bufferPool.destroy(buffer);
	}
}


