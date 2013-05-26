#pragma once

#include "ARTypes.h"
#include "StreamSubscriber.h"
#include "EncoderStage.h"
#include "SubscriberManager.h"

#include "portaudio.h"

#include <boost/asio.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/shared_ptr.hpp>

#include <boost/pool/object_pool.hpp>

#include <map>

namespace audioreflector
{
	class ARServer
	{
	private:
		ar_ushort _subscriptionPort;
		int _sampleRate;
		boost::asio::io_service _ioService;
		boost::asio::ip::udp::socket _socket;
		boost::asio::ip::udp::endpoint _remoteEndpoint;

		PaStream* _inputStream;

		boost::shared_ptr<boost::thread> _networkThread;

		EncoderStagePtr _encoderStage;
		SubscriberManagerPtr _subscriberMgr;

	public:
		ARServer(ar_ushort subscriptionPort, int sampleRate, IEncoderPtr encoder);
		virtual ~ARServer();

		void start();

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

		void initPortaudio();
		void beginRecording();
		void initAsio();
		void asioThreadRun();

		void beginReceive();
		void handleReceive(const boost::system::error_code& error,
				      std::size_t bytes_transferred,
				      packet_buffer_ptr buffer);

		void subscribeClient(boost::asio::ip::udp::endpoint ep);
		void unsubscribeClient(boost::asio::ip::udp::endpoint ep);
		void updateClientSubscription(boost::asio::ip::udp::endpoint ep);

		void onEncodeComplete(EncodedSamplesPtr samples);
	};
}
