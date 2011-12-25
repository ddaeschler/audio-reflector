/*
 * SubscriberManager.h
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#ifndef SUBSCRIBERMANAGER_H_
#define SUBSCRIBERMANAGER_H_

#include "StreamSubscriber.h"
#include "EncodedSamples.h"

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <map>
#include <queue>

namespace audioreflector
{
	class SubscriberManager : public boost::noncopyable
	{
	private:
		boost::asio::io_service& _ioService;
		boost::asio::ip::udp::socket& _serverSocket;

		typedef std::map<boost::asio::ip::udp::endpoint, StreamSubscriberPtr> SubscriberMap;
		SubscriberMap _subscribers;

		std::queue<EncodedSamplesPtr> _waitingSamples;
		std::vector<boost::asio::ip::udp::endpoint> _pendingSubscribes;
		std::vector<boost::asio::ip::udp::endpoint> _pendingUnsubscribes;

		bool _isSending;
		SubscriberMap::iterator _currentClient;

	public:
		SubscriberManager(boost::asio::io_service& ioService,
				boost::asio::ip::udp::socket& serverSocket);

		virtual ~SubscriberManager();

		void newSubscriber(const boost::asio::ip::udp::endpoint& ep);

		void unsubscribe(const boost::asio::ip::udp::endpoint& ep);

		void updateSubscription(const boost::asio::ip::udp::endpoint& ep);

		void checkForTimedOutSubscriptions();

		void enqueueOrSend(EncodedSamplesPtr samples);

	private:
		void recordSubscription(const boost::asio::ip::udp::endpoint& ep);

		void serializedEnqueueOrSend(EncodedSamplesPtr samples);
		void beginBroadcastSamples(EncodedSamplesPtr samples);
		void onAfterSampleSentToClient();
		void sendSampleToClient();

		void serializedSubscribe(boost::asio::ip::udp::endpoint ep);
		void serializedUnsubscribe(boost::asio::ip::udp::endpoint ep);
		void serializedUpdate(boost::asio::ip::udp::endpoint ep);

		void performPendingSubscribes();
		void performPendingUnsubscribes();
	};

	typedef boost::shared_ptr<SubscriberManager> SubscriberManagerPtr;
}


#endif /* SUBSCRIBERMANAGER_H_ */
