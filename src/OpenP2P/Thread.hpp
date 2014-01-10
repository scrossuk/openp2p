#ifndef OPENP2P_THREAD_HPP
#define OPENP2P_THREAD_HPP

#include <boost/thread.hpp>

#include <OpenP2P/Runnable.hpp>

namespace OpenP2P {

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
