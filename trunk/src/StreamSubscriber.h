/*
 * StreamSubscriber.h
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#ifndef STREAMSUBSCRIBER_H_
#define STREAMSUBSCRIBER_H_

#include "ARTypes.h"
#include "PacketBuffer.h"

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/function.hpp>

#include <vector>

namespace audioreflector
{

	class StreamSubscriber
	{
	private:
		static const int SUBSCRIPTION_TIMEOUT;

		boost::asio::ip::udp::endpoint _ep;
		boost::asio::ip::udp::socket& _socket;
		boost::posix_time::ptime _lastResubscribe;

		boost::function<void()> _sendCompleteCallback;

	public:
		StreamSubscriber(boost::asio::ip::udp::endpoint endPoint,
				boost::asio::ip::udp::socket& socket,
				boost::function<void()> sendCompleteCallback);

		virtual ~StreamSubscriber();

		/**
		 * Sends data to this subscriber
		 */
		void sendData(packet_buffer_ptr buffer, int amtToCopy);

		/**
		 * Whether or not this client has updated it's subscription
		 * within the timeout period
		 */
		bool isExpired();

		/**
		 * Updates the subscription time for this client
		 */
		void updateSubscription();

		/**
		 * Returns the udp endpoint of this subscriber
		 */
		const boost::asio::ip::udp::endpoint& getEndpoint() const;

	private:
		void handleSend(const boost::system::error_code& /*error*/,
				      std::size_t /*bytes_transferred*/,
				      packet_buffer_ptr buffer);
	};

	typedef boost::shared_ptr<StreamSubscriber> StreamSubscriberPtr;
}


#endif /* STREAMSUBSCRIBER_H_ */
