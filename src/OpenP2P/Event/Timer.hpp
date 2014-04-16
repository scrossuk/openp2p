#ifndef OPENP2P_EVENT_TIMER_HPP
#define OPENP2P_EVENT_TIMER_HPP

#include <memory>

namespace OpenP2P {

	namespace Event {
	
		class Source;
		
		class Timer {
			public:
				Timer();
				~Timer();
				
				// Moveable.
				Timer(Timer&& other);
				Timer& operator=(Timer&&);
				
				void setMilliseconds(double milliseconds);
				
				void schedule();
				
				bool hasExpired() const;
				
				Source eventSource() const;
				
			private:
				// Non-copyable.
				Timer(const Timer&) = delete;
				Timer& operator=(const Timer&) = delete;
				
				std::unique_ptr<struct TimerImpl> impl_;
				
		};
		
	}
	
}

#endif
