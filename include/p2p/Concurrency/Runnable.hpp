#ifndef P2P_CONCURRENCY_RUNNABLE_HPP
#define P2P_CONCURRENCY_RUNNABLE_HPP

namespace p2p {

	namespace Concurrency {
		
		class Runnable {
			public:
				virtual void run() = 0;
				
				virtual void cancel() = 0;
				
		};
		
	}
	
	using Runnable = Concurrency::Runnable;
	
}

#endif
