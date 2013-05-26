/*
 * EncoderBuffer.h
 *
 *  Created on: May 25, 2013
 *      Author: ddaeschler
 */

#ifndef ENCODERBUFFER_H_
#define ENCODERBUFFER_H_

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace audioreflector
{
	/**
	 * Buffer used for the results of an audio encoder run
	 */
	struct encoder_buffer : public boost::noncopyable
	{
		public:
			static const int BUF_SZ = 65536;
			char contents[BUF_SZ];

		public:
			encoder_buffer() {
			}

			~encoder_buffer() {
			}
	};

	typedef boost::shared_ptr<encoder_buffer> encoder_buffer_ptr;
}

#endif /* ENCODERBUFFER_H_ */
