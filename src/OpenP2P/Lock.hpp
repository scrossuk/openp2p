#ifndef OPENP2P_LOCK_HPP
#define OPENP2P_LOCK_HPP

#include <boost/thread.hpp>

namespace OpenP2P {

	typedef boost::unique_lock<boost::mutex> Lock;
	
}

#endif
