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
	const int StreamSubscriber::SUBSCRIPTION_TIMEOUT = 30;

	StreamSubscriber::StreamSubscriber(boost::asio::ip::udp::endpoint endPoint,
				boost::asio::ip::udp::socket& socket,
				boost::function<void()> sendCompleteCallback)
	:	_ep(endPoint), _socket(socket),
	 	_lastResubscribe(boost::posix_time::second_clock::local_time()),
	 	_sendCompleteCallback(sendCompleteCallback)
	{

	}

	StreamSubscriber::~StreamSubscriber()
	{

	}

	void StreamSubscriber::updateSubscription()
	{
		_lastResubscribe = boost::posix_time::second_clock::local_time();
	}

	const boost::asio::ip::udp::endpoint& StreamSubscriber::getEndpoint() const
	{
		return	_ep;
	}

	bool StreamSubscriber::isExpired()
	{
		boost::posix_time::time_duration diff = boost::posix_time::second_clock::local_time() - _lastResubscribe;
		return diff.seconds() >= SUBSCRIPTION_TIMEOUT;
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
		_sendCompleteCallback();
	}
}


