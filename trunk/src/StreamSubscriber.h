/*
 * StreamSubscriber.h
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#ifndef STREAMSUBSCRIBER_H_
#define STREAMSUBSCRIBER_H_

#include "ARTypes.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>

#include <vector>

namespace audioreflector
{

	class StreamSubscriber
	{
	private:
		boost::asio::ip::udp::endpoint _ep;
		boost::asio::ip::udp::socket& _socket;

	public:
		StreamSubscriber(boost::asio::ip::udp::endpoint endPoint,
				boost::asio::ip::udp::socket& socket);

		virtual ~StreamSubscriber();

		void sendData(packet_buffer_ptr buffer, int amtToCopy);

	private:
		void handleSend(const boost::system::error_code& /*error*/,
				      std::size_t /*bytes_transferred*/,
				      packet_buffer_ptr buffer);
	};

	typedef boost::shared_ptr<StreamSubscriber> StreamSubscriberPtr;
}


#endif /* STREAMSUBSCRIBER_H_ */
