/*
 * ARTypes.h
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#ifndef ARTYPES_H_
#define ARTYPES_H_

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>

namespace audioreflector
{
	typedef unsigned short ushort;

	const int MTU = 1200;
	const int BIT_DEPTH = 16;
	const int BIT_DEPTH_IN_BYTES = 2;

	enum ClientSubscriptionStatus
	{
		CSS_SUBSCRIBED = 1,
		CSS_UNSUBSCRIBED = 2
	};

	struct packet_buffer : public boost::noncopyable
	{
		public:
			static const int BUF_SZ = MTU;
			char contents[BUF_SZ];

		public:
			packet_buffer() {
			}

			~packet_buffer() {
			}
	};

	typedef boost::shared_ptr<packet_buffer> packet_buffer_ptr;
}


#endif /* ARTYPES_H_ */
