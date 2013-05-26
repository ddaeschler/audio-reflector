/*
 * SubscriberManager.cpp
 *
 *  Created on: Dec 24, 2011
 *      Author: ddaeschler
 */

#include "SubscriberManager.h"
#include "PacketizedSamples.h"

#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>
#include <iostream>

using namespace boost::posix_time;

namespace audioreflector
{
	const int SubscriberManager::SUBSCRIPTION_CHECK_INTERVAL = 10;

	SubscriberManager::SubscriberManager(boost::asio::io_service& ioService,
			boost::asio::ip::udp::socket& serverSocket)
	: _ioService(ioService), _serverSocket(serverSocket),
	  _subscriptionCheckTimer(_ioService),
	  _isSending(false)
	{
		//set a timer to remove subscribers that have not checked in during the timeout period
		this->resetSubscriptionCheckTimer();
	}

	SubscriberManager::~SubscriberManager()
	{

	}

	void SubscriberManager::newSubscriber(const boost::asio::ip::udp::endpoint& ep)
	{
		_ioService.post(boost::bind(&SubscriberManager::serializedSubscribe, this, ep));
	}

	void SubscriberManager::recordSubscription(const boost::asio::ip::udp::endpoint& ep)
    {
        _subscribers[ep] = StreamSubscriberPtr(new StreamSubscriber(ep, _serverSocket,
        		boost::bind(&SubscriberManager::onAfterSampleSentToClient, this)));
    }

    void SubscriberManager::serializedSubscribe(boost::asio::ip::udp::endpoint ep)
    {
        if(_isSending){
            _pendingSubscribes.push_back(ep);
        }else{
            recordSubscription(ep);
		}
	}

	void SubscriberManager::unsubscribe(const boost::asio::ip::udp::endpoint& ep)
	{
		_ioService.post(boost::bind(&SubscriberManager::serializedUnsubscribe, this, ep));
	}

	void SubscriberManager::serializedUnsubscribe(boost::asio::ip::udp::endpoint ep)
	{
		if (_isSending) {
			_pendingUnsubscribes.push_back(ep);
		} else {
			_subscribers.erase(ep);
		}
	}

	void SubscriberManager::updateSubscription(const boost::asio::ip::udp::endpoint& ep)
	{
		_ioService.post(boost::bind(&SubscriberManager::serializedUpdate, this, ep));
	}

	void SubscriberManager::serializedUpdate(boost::asio::ip::udp::endpoint ep)
	{
		SubscriberMap::iterator found = _subscribers.find(ep);
		if (found != _subscribers.end()) {
			found->second->updateSubscription();
		}
	}

	void SubscriberManager::checkForTimedOutSubscriptions()
	{
		SubscriberMap::iterator end = _subscribers.end();
		for (SubscriberMap::iterator i = _subscribers.begin(); i != end; ++i) {
			if (i->second->isExpired()) {
				std::cerr << "Subscription timeout expired: " << i->first << std::endl;
				this->unsubscribe(i->first);
			}
		}


	}

	void SubscriberManager::enqueueOrSend(PacketizedSamplesPtr samples)
	{
		_ioService.post(boost::bind(&SubscriberManager::serializedEnqueueOrSend, this, samples));
	}

	void SubscriberManager::serializedEnqueueOrSend(PacketizedSamplesPtr samples)
	{
		if (_isSending) {
			_waitingSamples.push(samples);

		} else {
			_waitingSamples.push(samples);
			this->beginBroadcastSamples();
		}
	}

	void SubscriberManager::beginBroadcastSamples()
	{
		_isSending = true;
		_currentClient = _subscribers.begin();

		this->sendSampleToClient();
	}

	void SubscriberManager::sendSampleToClient()
	{
		if (_currentClient != _subscribers.end()) {
			PacketizedSamplesPtr samples = _waitingSamples.front();
			_currentClient->second->sendData(samples->Samples, samples->Size);

		} else {
			this->performPendingUnsubscribes();
			this->performPendingSubscribes();
			_waitingSamples.pop();

			if (_waitingSamples.size() == 0) {
				_isSending = false;

			} else {
				//continue sending while the queue is full
				this->beginBroadcastSamples();
			}
		}
	}

	void SubscriberManager::performPendingUnsubscribes()
	{
		if (_pendingUnsubscribes.size() > 0) {
			BOOST_FOREACH (const boost::asio::ip::udp::endpoint& ep, _pendingUnsubscribes) {
				_subscribers.erase(ep);
			}

			_pendingUnsubscribes.clear();
		}
	}

	void SubscriberManager::performPendingSubscribes()
	{
		if (_pendingSubscribes.size() > 0) {
			BOOST_FOREACH (const boost::asio::ip::udp::endpoint& ep, _pendingSubscribes) {
				this->recordSubscription(ep);
			}

			_pendingSubscribes.clear();
		}
	}

	void SubscriberManager::onAfterSampleSentToClient()
	{
		++_currentClient;
		this->sendSampleToClient();
	}

	void SubscriberManager::resetSubscriptionCheckTimer()
	{
		_subscriptionCheckTimer.expires_from_now(seconds(SUBSCRIPTION_CHECK_INTERVAL));
		_subscriptionCheckTimer.async_wait(boost::bind(&SubscriberManager::onCheckForExpirations, this,
				boost::asio::placeholders::error));
	}

	void SubscriberManager::onCheckForExpirations(const boost::system::error_code& error)
	{
		if (! error) {
			this->checkForTimedOutSubscriptions();
			this->resetSubscriptionCheckTimer();
		}
	}

}
