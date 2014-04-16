#ifndef OPENP2P_SIGNAL_HPP
#define OPENP2P_SIGNAL_HPP

#include <condition_variable>
#include <mutex>

namespace OpenP2P {

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
