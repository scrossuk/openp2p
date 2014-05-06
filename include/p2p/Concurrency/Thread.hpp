#ifndef P2P_CONCURRENCY_THREAD_HPP
#define P2P_CONCURRENCY_THREAD_HPP

#include <boost/thread.hpp>

#include <p2p/Concurrency/Runnable.hpp>

namespace p2p {
	
	namespace Concurrency {
		
		class Thread {
			public:
				Thread(Runnable& runnable);
				~Thread();
				
			private:
				Runnable& runnable_;
				boost::thread internalThread_;
				
		};
		
	}
	
	using Thread = Concurrency::Thread;
	
}

#endif
