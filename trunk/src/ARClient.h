/*
 * ARClient.h
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#ifndef ARCLIENT_H_
#define ARCLIENT_H_

#include "ARTypes.h"
#include "IDecoder.h"
#include "portaudio.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <vector>

namespace audioreflector
{
	class ARClient
	{
	private:
		static const char* SubscribeMsg;
		static const char* UnsubscribeMsg;
		static const char* RenewMsg;
		static const int SUBSCRIPTION_RENEWAL_INTERVAL;

		std::string _host;
		ar_ushort _port;
		int _sampleRate;
		IDecoderPtr _decoder;

		boost::asio::io_service _ioService;
		boost::asio::ip::udp::socket _socket;
		boost::asio::ip::udp::endpoint _remoteEndpoint;
		boost::asio::ip::udp::endpoint _rcvEp;

		boost::asio::deadline_timer _subscriptionRenewalTimer;

		PaStream* _outputStream;

		boost::shared_ptr<boost::thread> _networkThread;
		boost::shared_array<char> _packetBuffer;

		boost::circular_buffer<char> _netBuffer;
		boost::mutex _bufferLock;

		bool _pausedForBufferRefill;
		bool _doStop;

		boost::posix_time::ptime _lastBufferDisplay;

		std::vector<char> _builderBuffer;
		int _currentBuildSize;

	public:
		ARClient(const std::string& host, ar_ushort port, int sampleRate,
				IDecoderPtr decoder);
		virtual ~ARClient();

		void start();
		void stop();

	private:
		static int PaCallback(const void *inputBuffer, void *outputBuffer,
		                unsigned long framesPerBuffer,
		                const PaStreamCallbackTimeInfo* timeInfo,
		                PaStreamCallbackFlags statusFlags,
		                void *userData);

		int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
				unsigned long framesPerBuffer,
				const PaStreamCallbackTimeInfo* timeInfo,
				PaStreamCallbackFlags statusFlags);

		void subscribeToServerStream();
		void unsubscribeToServerStream();
		void handleSend(const boost::system::error_code& error, std::size_t bytes_transferred);
		void beginRecv();
		void handleReceive(const boost::system::error_code& error, std::size_t bytes_transferred);

		void initPortaudio();
		void asioThreadRun();
		void resetSubscriptionRenewalTimer();
		void onRenewSubscription(const boost::system::error_code& error);
	};
}

#endif /* ARCLIENT_H_ */
