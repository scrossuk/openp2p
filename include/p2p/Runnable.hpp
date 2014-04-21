#ifndef P2P_RUNNABLE_HPP
#define P2P_RUNNABLE_HPP

namespace p2p {

	class Runnable {
		public:
			virtual void run() = 0;
			
			virtual void cancel() = 0;
			
	};
	
}

#endif
