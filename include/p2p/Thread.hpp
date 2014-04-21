#ifndef P2P_THREAD_HPP
#define P2P_THREAD_HPP

#include <boost/thread.hpp>

#include <p2p/Runnable.hpp>

namespace p2p {

	class Thread {
		public:
			Thread(Runnable& runnable);
			
			~Thread();
			
		private:
			Runnable& runnable_;
			boost::thread internalThread_;
			
	};
	
}

#endif
