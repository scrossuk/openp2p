#ifndef P2P_EVENT_SIGNAL_HPP
#define P2P_EVENT_SIGNAL_HPP

#include <memory>

#include <p2p/Event/Event_api.hpp>
#include <p2p/Event/Source.hpp>

namespace p2p {

	namespace Event {
		
		class EVENT_OPENP2P_API Signal {
			public:
				Signal();
				~Signal();
				
				// Moveable.
				Signal(Signal&& other);
				Signal& operator=(Signal&&);
				
				void activate();
				
				void reset();
				
				bool isActive() const;
				
				Source eventSource() const;
				
			private:
				// Non-copyable.
				Signal(const Signal&) = delete;
				Signal& operator=(const Signal&) = delete;
				
				std::unique_ptr<struct SignalImpl> impl_;
				
		};
		
	}
	
}

#endif
