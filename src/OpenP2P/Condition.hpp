#ifndef OPENP2P_CONDITION_HPP
#define OPENP2P_CONDITION_HPP

#include <boost/thread.hpp>

#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Timeout.hpp>

namespace OpenP2P {

	class Condition {
		public:
			inline void notifyOne() {
				internalCondition_.notify_one();
			}
			
			inline void notifyAll() {
				internalCondition_.notify_all();
			}
			
			inline bool wait(Lock& lock, Timeout timeout = Timeout::Infinite()) {
				if (timeout.isInfinite()) {
					internalCondition_.wait(lock);
					return true;
				} else {
					return internalCondition_.timed_wait(lock, boost::posix_time::milliseconds(timeout.seconds() * 1000));
				}
			}
			
		private:
			boost::condition_variable internalCondition_;
			
	};
	
}

#endif
