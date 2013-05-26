/*
 * ARTypes.h
 *
 *  Created on: Dec 6, 2011
 *      Author: ddaeschler
 */

#ifndef ARTYPES_H_
#define ARTYPES_H_

#include <boost/shared_array.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>

#include <iostream>

namespace audioreflector
{
	typedef unsigned short ar_ushort;

	const int MTU = 1200;
	const int BIT_DEPTH = 16;
	const int BIT_DEPTH_IN_BYTES = 2;
	const int HEADER_SZ = 8;

	enum ClientSubscriptionStatus
	{
		CSS_SUBSCRIBED = 1,
		CSS_UNSUBSCRIBED = 2,
		CSS_UPDATESUBSC = 3
	};
}


#endif /* ARTYPES_H_ */
