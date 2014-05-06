#ifndef P2P_CONCURRENCY_SIGNAL_HPP
#define P2P_CONCURRENCY_SIGNAL_HPP

#include <condition_variable>
#include <mutex>

namespace p2p {

	class Signal {
		public:
			Signal();
			
			void activate();
			
			void reset();
			
			bool isActivated() const;
			
			void wait();
			
			void cancel();
			
		private:
			// Non-copyable.
			Signal(const Signal&) = delete;
			Signal& operator=(const Signal&) = delete;
			
			mutable std::mutex mutex_;
			std::condition_variable condition_;
			bool isActivated_;
			
	};
	
}

#endif
