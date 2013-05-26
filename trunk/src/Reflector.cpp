#include "portaudio.h"

#include "ARTypes.h"
#include "ARServer.h"
#include "ARClient.h"
#include "IEncoder.h"
#include "WavPackEncoder.h"
#include "IDecoder.h"
#include "WavPackDecoder.h"

#include <boost/lexical_cast.hpp>

#include <iostream>
#include <string>
#include <stdexcept>


using namespace std;

const string& VER = "0.1";


void BeginServerMode();
void BeginClientMode();


int main (int argc, char* argv[])
{
	cout << "AudioReflector " << VER << endl;
	Pa_Initialize();

	cout << "Server or Client mode? [s/c]: ";
	string serverOrClient;
	getline(cin, serverOrClient);

	if (serverOrClient == "s") {
		BeginServerMode();

	} else {
		BeginClientMode();
	}

	return 0;
}

void BeginServerMode()
{
	using namespace audioreflector;

	string line;

	cout << "UDP Subscription Port [8889]: ";
	ar_ushort subscPort;
	getline(cin, line);
	if (line == "") {
		subscPort = 8889;
	} else {
		subscPort = boost::lexical_cast<ar_ushort>(line);
	}

	cout << "Sample Rate [44100]: ";
	int sampleRate;
	getline(cin, line);
	if (line == "") {
		sampleRate = 44100;
	} else {
		sampleRate = boost::lexical_cast<int>(line);
	}

	cout << "Encoder (n:none, wl:lossless) [wl]: ";
	IEncoderPtr encoder;
	getline(cin, line);
	if (line == "" || line == "wl") {
		encoder.reset(new WavPackEncoder(sampleRate));
	} else {

	}

	ARServer* server = new ARServer(subscPort, sampleRate, encoder);
	server->start();

	string input;
	cin >> input;
}

void BeginClientMode()
{
	using namespace audioreflector;

	string line;

	cout << "Reflector Host: ";
	string reflectorHost;
	getline(cin, reflectorHost);

	cout << "UDP Subscription Port [8889]: ";
	audioreflector::ar_ushort subscPort;
	getline(cin, line);
	if (line == "") {
		subscPort = 8889;
	} else {
		subscPort = boost::lexical_cast<audioreflector::ar_ushort>(line);
	}

	cout << "Sample Rate [44100]: ";
	int sampleRate;
	getline(cin, line);
	if (line == "") {
		sampleRate = 44100;
	} else {
		sampleRate = boost::lexical_cast<int>(line);
	}


	cout << "Encoding (n:none, wl:lossless) [wl]: ";
	IDecoderPtr decoder;
	getline(cin, line);
	if (line == "" || line == "wl") {
		decoder.reset(new WavPackDecoder());
	} else {

	}

	try
	{
		ARClient* client = new ARClient(reflectorHost, subscPort, sampleRate,
				decoder);
		client->start();

		string input;
		cin >> input;

		client->stop();
	}
	catch (const std::runtime_error& e)
	{
		cout << "Error: " << e.what() << endl;
	}
}
