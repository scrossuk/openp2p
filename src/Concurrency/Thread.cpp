#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <p2p/Concurrency/Runnable.hpp>
#include <p2p/Concurrency/Thread.hpp>

namespace p2p {

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

