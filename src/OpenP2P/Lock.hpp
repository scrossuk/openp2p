#ifndef OPENP2P_LOCK_HPP
#define OPENP2P_LOCK_HPP

#include <boost/thread.hpp>

#include <OpenP2P/Mutex.hpp>

namespace OpenP2P{

	class Lock{
		public:
			inline Lock(Mutex& mutex) : internalLock_(mutex){ }

			inline operator boost::unique_lock<boost::mutex>&(){
				return internalLock_;
			}

		private:
			boost::unique_lock<boost::mutex> internalLock_;

	};

}

#endif
