/*
 * StreamSubscriber.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#include "StreamSubscriber.h"

#include <boost/bind.hpp>
#include <iostream>

namespace audioreflector
{
	StreamSubscriber::StreamSubscriber(boost::asio::ip::udp::endpoint endPoint,
				boost::asio::ip::udp::socket& socket)
	:	_ep(endPoint), _socket(socket)
	{

	}

	StreamSubscriber::~StreamSubscriber()
	{

	}

	void StreamSubscriber::sendData(packet_buffer_ptr buffer, int amtToCopy)
	{
		_socket.async_send_to(
				boost::asio::buffer(buffer->contents, amtToCopy), _ep,
				boost::bind(&StreamSubscriber::handleSend, this,
				            boost::asio::placeholders::error,
				            boost::asio::placeholders::bytes_transferred,
				            buffer));
	}

	void StreamSubscriber::handleSend(
		      const boost::system::error_code& error,
		      std::size_t /*bytes_transferred*/,
		      packet_buffer_ptr buffer)
	{

	}
}


