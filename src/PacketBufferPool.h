/*
 * PacketBufferPool.h
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#ifndef PACKETBUFFERPOOL_H_
#define PACKETBUFFERPOOL_H_

#include "ARTypes.h"

#include <boost/pool/object_pool.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/pool/object_pool.hpp>


namespace audioreflector
{

	class PacketBufferPool
	{
	private:
		boost::object_pool<packet_buffer> _bufferPool;
		boost::mutex _bufferPoolLock;

		void freePacketBuffer(packet_buffer* buffer);

		PacketBufferPool();
		virtual ~PacketBufferPool();

	public:
		static PacketBufferPool& getInstance();

		packet_buffer_ptr alloc();
	};

}

#endif /* PACKETBUFFERPOOL_H_ */
