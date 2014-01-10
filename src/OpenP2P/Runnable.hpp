#ifndef OPENP2P_RUNNABLE_HPP
#define OPENP2P_RUNNABLE_HPP

namespace OpenP2P {

	class Runnable {
		public:
			virtual void run() = 0;
			
			virtual void cancel() = 0;
			
	};
	
}

#endif
