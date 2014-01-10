#ifndef OPENP2P_SIGNAL_HPP
#define OPENP2P_SIGNAL_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Timeout.hpp>

namespace OpenP2P {

	class Signal: boost::noncopyable {
		public:
			Signal();
			
			void activate();
			
			void reset();
			
			bool isActivated();
			
			bool wait(Timeout timeout = Timeout::Infinite());
			
			void cancel();
			
		private:
			Mutex mutex_;
			Condition cond_;
			bool isActivated_;
			
	};
	
}

#endif
