#ifndef P2P_CONCURRENCY_THREAD_HPP
#define P2P_CONCURRENCY_THREAD_HPP

#include <memory>

#include <p2p/Concurrency/Concurrency_api.hpp>
#include <p2p/Concurrency/Runnable.hpp>

namespace p2p {
	
	namespace Concurrency {
		
		class CONCURRENCY_OPENP2P_API Thread {
			public:
				Thread();
				Thread(Runnable& runnable);
				Thread(Thread&&);
				Thread& operator=(Thread&&);
				
				~Thread();
				
			private:
				// Non-copyable.
				Thread(const Thread&) = delete;
				Thread& operator=(Thread) = delete;
				
				std::unique_ptr<struct ThreadImpl> impl_;
				
		};
		
	}
	
	using Thread = Concurrency::Thread;
	
}

#endif
