#ifndef OPENP2P_CONDITION_HPP
#define OPENP2P_CONDITION_HPP

#include <boost/thread.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/Lock.hpp>

namespace OpenP2P{

	class Condition: public Cancellable{
		public:
			inline void notify(){
				internalCondition_.notify_all();
			}

			inline void wait(Lock& lock){
				internalCondition_.wait(lock.getInternal());
			}

			inline void cancel(){
				notify();
			}

		private:
			boost::condition_variable internalCondition_;

	};

}

#endif
