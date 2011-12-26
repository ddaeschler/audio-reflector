/*
 * IDecoder.h
 *
 *  Created on: Dec 26, 2011
 *      Author: ddaeschler
 */

#ifndef IDECODER_H_
#define IDECODER_H_

#include <boost/shared_ptr.hpp>

namespace audioreflector
{
	struct decoded_samples
	{
		const char* SampleData;
		int NumSamples;
	};

	class IDecoder
	{
	public:
		virtual ~IDecoder();

		virtual decoded_samples decode(const char* data, std::size_t bytes) = 0;
	};

	typedef boost::shared_ptr<IDecoder> IDecoderPtr;
}


#endif /* IDECODER_H_ */
