#ifndef OPENP2P_CONDITION_HPP
#define OPENP2P_CONDITION_HPP

#include <boost/thread.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/Lock.hpp>

namespace OpenP2P{

	class Condition: public Cancellable{
		public:
			inline void notify(){
				internalCondition_.notify_one();
			}

			inline void notifyAll(){
				internalCondition_.notify_all();
			}

			inline void wait(Lock& lock){
				internalCondition_.wait(lock);
			}
			
			inline void timedWait(Lock& lock, double secs){
				internalCondition_.timed_wait(lock, boost::posix_time::milliseconds(secs * 1000));
			}

			inline void cancel(){
				notify();
			}

		private:
			boost::condition_variable internalCondition_;

	};

}

#endif
