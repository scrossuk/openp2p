#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <OpenP2P/Runnable.hpp>
#include <OpenP2P/Thread.hpp>

namespace OpenP2P {

	namespace {
	
		void threadFunc(Runnable& runnable) {
			runnable.run();
		}
		
	}
	
	Thread::Thread(Runnable& runnable) : runnable_(runnable), internalThread_(boost::bind(threadFunc, boost::ref(runnable))) { }
	
	Thread::~Thread() {
		runnable_.cancel();
		internalThread_.join();
	}
	
}

