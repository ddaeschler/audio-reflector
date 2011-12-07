/*
 * ARClient.cpp
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#include "ARClient.h"

#include <boost/bind.hpp>
#include <stdexcept>
#include <iostream>

using boost::asio::ip::udp;
using namespace std;

namespace audioreflector
{
	const char* ARClient::SubscribeMsg = "\1";
	const char* ARClient::UnsubscribeMsg = "\2";

	ARClient::ARClient(const std::string& host, ushort port, int sampleRate)
		: _host(host), _port(port), _sampleRate(sampleRate),
		  _ioService(), _socket(_ioService),
		  _packetBuffer(new char[MTU]),
		  _netBuffer(sampleRate * 2 * 2)
	{

	}

	ARClient::~ARClient()
	{

	}

	void ARClient::start()
	{
		this->initPortaudio();

		udp::resolver resolver(_ioService);
		udp::resolver::query query(udp::v4(), _host, "");
		boost::system::error_code ec;

		boost::asio::ip::basic_resolver_iterator<boost::asio::ip::udp> iter = resolver.resolve(query, ec);
		if (ec != boost::system::errc::success) {
			throw std::runtime_error("Unable to resolve host " + _host);
		}

		_remoteEndpoint = *iter;

		_remoteEndpoint.port(_port);
		_networkThread.reset(new boost::thread(boost::bind(&ARClient::asioThreadRun, this)));
	}

	void ARClient::initPortaudio()
	{
		PaError err;

		_pausedForBufferRefill = true;

		/* Open an audio I/O stream. */
		err = Pa_OpenDefaultStream( &_outputStream,
									0,          /* no input */
									1,          /* one output channel */
									paInt16,  /* 16 bit audio */
									_sampleRate,
									paFramesPerBufferUnspecified,   /* frames per buffer, i.e. the number
													   of sample frames that PortAudio will
													   request from the callback. Many apps
													   may want to use
													   paFramesPerBufferUnspecified, which
													   tells PortAudio to pick the best,
													   possibly changing, buffer size.*/
									&ARClient::PaCallback, /* this is your callback function */
									static_cast<void*>(this) ); /*This is a pointer that will be passed to
													   	   	   your callback*/

		if( err != paNoError ) throw std::runtime_error("Could not initialize audio output");

		err = Pa_StartStream( _outputStream );

		if ( err != paNoError ) throw std::runtime_error("Could start audio output");
	}

	int ARClient::PaCallback(const void *inputBuffer, void *outputBuffer,
		                unsigned long framesPerBuffer,
		                const PaStreamCallbackTimeInfo* timeInfo,
		                PaStreamCallbackFlags statusFlags,
		                void *userData)
	{
		ARClient* client = static_cast<ARClient*>(userData);
		return client->paCallbackMethod(inputBuffer, outputBuffer, framesPerBuffer, timeInfo, statusFlags);
	}

	int ARClient::paCallbackMethod(const void *inputBuffer, void *outputBuffer,
					unsigned long framesPerBuffer,
					const PaStreamCallbackTimeInfo* timeInfo,
					PaStreamCallbackFlags statusFlags)
	{
		int bytesPerBuffer = framesPerBuffer * 2;

		boost::mutex::scoped_lock lock(_bufferLock);
		if (_pausedForBufferRefill) {
			memset(outputBuffer, 0, bytesPerBuffer);
			return paContinue;
		}

		//cout << "dist" << std::distance(_netBuffer.begin(), _netBuffer.end()) << endl;
		if (std::distance(_netBuffer.begin(), _netBuffer.end()) <= bytesPerBuffer) {
			_pausedForBufferRefill = true;
			memset(outputBuffer, 0, bytesPerBuffer);
			cout << "Buffer underrun" << endl;
			return paContinue;
		}

		std::copy(_netBuffer.begin(), _netBuffer.begin() + bytesPerBuffer, (char*)outputBuffer);
		_netBuffer.rotate(_netBuffer.begin() + bytesPerBuffer);
		_netBuffer.resize(_netBuffer.size() - bytesPerBuffer);

		return paContinue;
	}

	void ARClient::asioThreadRun()
	{
		_socket.open(udp::v4());

		//send the subscribe message and begin receive
		this->subscribeToServerStream();

		_ioService.run();
		cout << "asio died" << endl;
	}

	void ARClient::subscribeToServerStream()
	{
		_socket.async_send_to(
				boost::asio::buffer(ARClient::SubscribeMsg, 1), _remoteEndpoint,
				boost::bind(&ARClient::handleSend, this,
							boost::asio::placeholders::error,
							boost::asio::placeholders::bytes_transferred));
	}

	void ARClient::handleSend(
		  const boost::system::error_code& /*error*/,
		  std::size_t /*bytes_transferred*/)
	{
		this->beginRecv();
	}

	void ARClient::beginRecv()
	{
		_socket.async_receive_from(
			boost::asio::buffer(_packetBuffer.get(), MTU), _rcvEp,
			boost::bind(&ARClient::handleReceive, this,
			  boost::asio::placeholders::error,
			  boost::asio::placeholders::bytes_transferred));
	}

	void ARClient::handleReceive(const boost::system::error_code& error,
			      std::size_t bytes_transferred)
	{
		{
			boost::mutex::scoped_lock lock(_bufferLock);

			//copy received data to the circular buffer
			_netBuffer.insert(_netBuffer.end(), _packetBuffer.get(), _packetBuffer.get() + bytes_transferred);
			if (_pausedForBufferRefill && std::distance(_netBuffer.begin(), _netBuffer.end()) >= _sampleRate * 2 * 1.75) {
				_pausedForBufferRefill = false;

				cout << "Buffer refilled" << endl;
			}
		}

		this->beginRecv();


	}
}
