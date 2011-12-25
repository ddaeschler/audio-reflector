/*
 * ARServer.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#include "ARServer.h"
#include "PacketBufferPool.h"

#include <boost/bind.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>

using boost::asio::ip::udp;
using namespace std;

namespace audioreflector
{

	ARServer::ARServer(ushort subscriptionPort, int bitRate, IEncoderPtr encoder)
	:	_subscriptionPort(subscriptionPort), _bitRate(bitRate),
	 	_ioService(),
	 	_socket(_ioService, udp::endpoint(udp::v4(), subscriptionPort)),
	 	_encoderStage(new EncoderStage(encoder, boost::bind(&ARServer::onEncodeComplete, this, _1)))
	{
		PacketBufferPool::getInstance();
	}

	ARServer::~ARServer()
	{

	}

	void ARServer::start()
	{
		//we need to fire up portaudio
		this->initPortaudio();

		//we then need to fire up asio to throw udp at the subscriber
		this->initAsio();
	}

	void ARServer::initAsio()
	{
		_networkThread.reset(new boost::thread(boost::bind(&ARServer::asioThreadRun, this)));
	}

	void ARServer::asioThreadRun()
	{
		this->beginReceive();
		_ioService.run();
	}

	void ARServer::beginReceive()
	{
		//we dont worry about allocating from the pool here, because
		//these messages will be few and far between
		packet_buffer_ptr buffer(new packet_buffer());

		_socket.async_receive_from(
			boost::asio::buffer(buffer->contents, packet_buffer::BUF_SZ), _remoteEndpoint,
			boost::bind(&ARServer::handleReceive, this,
			  boost::asio::placeholders::error,
			  boost::asio::placeholders::bytes_transferred,
			  buffer));
	}

	void ARServer::handleReceive(const boost::system::error_code& error,
		      std::size_t bytes_transferred,
		      packet_buffer_ptr buffer)
	{
		udp::endpoint epCopy = _remoteEndpoint;
		this->beginReceive();

		if (! error) {
			if (CSS_SUBSCRIBED == (ClientSubscriptionStatus) buffer->contents[0]) {
				this->subscribeClient(epCopy);
			} else {
				this->unsubscribeClient(epCopy);
			}
		}
	}

	void ARServer::subscribeClient(boost::asio::ip::udp::endpoint ep)
	{
		boost::mutex::scoped_lock lock(_subscriberLock);

		cout << "New client connection: " << ep << endl;
		_subscribers[ep] = StreamSubscriberPtr(new StreamSubscriber(ep, _socket));
	}

	void ARServer::unsubscribeClient(boost::asio::ip::udp::endpoint ep)
	{
		boost::mutex::scoped_lock lock(_subscriberLock);

		cout << "End client connection: " << ep << endl;
		_subscribers.erase(ep);
	}

	int ARServer::PaCallback(const void *inputBuffer, void *outputBuffer,
	                unsigned long framesPerBuffer,
	                const PaStreamCallbackTimeInfo* timeInfo,
	                PaStreamCallbackFlags statusFlags,
	                void *userData)
	{
		ARServer* server = static_cast<ARServer*>(userData);
		return server->paCallbackMethod(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
	}

	int ARServer::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
	                unsigned long framesPerBuffer,
	                const PaStreamCallbackTimeInfo* timeInfo,
	                PaStreamCallbackFlags statusFlags)
	{
		boost::mutex::scoped_lock lock(_subscriberLock);
		if (_subscribers.size() > 0) {
			//broadcast to all clients
			//packetize the given data using our selected MTU
			size_t numBytes = framesPerBuffer * BIT_DEPTH_IN_BYTES;

			//calculate the number of buffers we need
			int reqdBuffers = numBytes / MTU;
			if (numBytes % MTU > 0) {
				reqdBuffers++;
			}

			char* inBufferAsCharBuffer = (char*)inputBuffer;

			for (int i = 0; i < reqdBuffers; ++i) {
				size_t copiedSoFar = i * MTU;
				size_t amtToCopy;
				if (MTU < numBytes - copiedSoFar) {
					amtToCopy = MTU;
				} else {
					amtToCopy = numBytes - copiedSoFar;
				}

				packet_buffer_ptr buffer(PacketBufferPool::getInstance().alloc());
				memcpy(buffer->contents, inBufferAsCharBuffer + copiedSoFar, amtToCopy);


			}
		}

		return paContinue;
	}

	void ARServer::initPortaudio()
	{
		PaError err;

		/* Open an audio I/O stream. */
		err = Pa_OpenDefaultStream( &_inputStream,
									1,          /* 1 input channel*/
									0,          /* no output */
									paInt16,  /* 16 bit audio */
									_bitRate,
									paFramesPerBufferUnspecified,        /* frames per buffer, i.e. the number
													   of sample frames that PortAudio will
													   request from the callback. Many apps
													   may want to use
													   paFramesPerBufferUnspecified, which
													   tells PortAudio to pick the best,
													   possibly changing, buffer size.*/
									&ARServer::PaCallback, /* this is your callback function */
									static_cast<void*>(this) ); /*This is a pointer that will be passed to
													   your callback*/

		if( err != paNoError ) throw std::runtime_error("Could not initialize audio input");

		this->beginRecording();
	}

	void ARServer::beginRecording()
	{
		if (Pa_StartStream(_inputStream) != paNoError) {
			throw std::runtime_error("Could not start audio input");
		}
	}

	void ARServer::onEncodeComplete(EncodedSamplesPtr samples)
	{

	}
}



