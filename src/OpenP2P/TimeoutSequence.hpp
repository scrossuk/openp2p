#ifndef OPENP2P_TIMEOUTSEQUENCE_HPP
#define OPENP2P_TIMEOUTSEQUENCE_HPP

#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/Timer.hpp>

namespace OpenP2P {

	class TimeoutSequence {
		public:
			inline TimeoutSequence(Timeout timeout)
				: timeout_(timeout),
				  startTime_(timer_.getTime()) { }
				  
			inline Timeout getTimeout() {
				return Timeout::Seconds(timeout_.seconds() - (timer_.getTime() - startTime_));
			}
			
			inline bool hasExpired() {
				return getTimeout().hasExpired();
			}
			
		private:
			Timeout timeout_;
			Timer timer_;
			double startTime_;
			
	};
	
}

#endif
