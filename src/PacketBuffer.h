/*
 * PacketBuffer.h
 *
 *  Created on: May 25, 2013
 *      Author: ddaeschler
 */

#ifndef PACKETBUFFER_H_
#define PACKETBUFFER_H_

#include "ARTypes.h"

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>


namespace audioreflector
{
	/**
	 * Buffer that holds outbound packet data
	 */
	struct packet_buffer : public boost::noncopyable
	{
		public:
			static const int BUF_SZ = MTU;
			static const int BUF_MAX_DATA = MTU - HEADER_SZ;
			char contents[BUF_SZ];

		public:
			packet_buffer() {
			}

			~packet_buffer() {
			}
	};

	typedef boost::shared_ptr<packet_buffer> packet_buffer_ptr;
}

#endif /* PACKETBUFFER_H_ */
