#include <boost/bind.hpp>
#include <boost/thread.hpp>

#include <p2p/Concurrency/Runnable.hpp>
#include <p2p/Concurrency/Thread.hpp>

namespace p2p {

	namespace Concurrency {
		
		namespace {
			
			void threadFunc(Runnable* runnable) {
				runnable->run();
			}
			
		}
		
		struct ThreadImpl {
			Runnable& runnable;
			boost::thread internalThread;
			
			ThreadImpl(Runnable& pRunnable)
				: runnable(pRunnable),
				internalThread(boost::bind(threadFunc, &runnable)) { }
		};
		
		Thread::Thread()
			: impl_(nullptr) { }
		
		Thread::Thread(Runnable& runnable) : impl_(new ThreadImpl(runnable)) { }
		
		Thread::Thread(Thread&& other)
			: impl_(nullptr) {
			std::swap(impl_, other.impl_);
		}
		
		Thread& Thread::operator=(Thread&& other) {
			std::swap(impl_, other.impl_);
			return *this;
		}
		
		Thread::~Thread() {
			if (impl_.get() != nullptr) {
				impl_->runnable.cancel();
				impl_->internalThread.join();
			}
		}
		
	}
	
}

