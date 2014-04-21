#ifndef P2P_EVENT_SOURCEIMPL_HPP
#define P2P_EVENT_SOURCEIMPL_HPP

namespace p2p {

	namespace Event {
	
		class Receiver {
			public:
				virtual void notify() = 0;
				
		};
		
		class SourceImpl {
			public:
				virtual void addReceiver(Receiver& receiver) = 0;
				
				virtual void removeReceiver(Receiver& receiver) = 0;
				
		};
		
	}
	
}

#endif
