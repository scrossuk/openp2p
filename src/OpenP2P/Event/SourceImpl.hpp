#ifndef OPENP2P_EVENT_SOURCEIMPL_HPP
#define OPENP2P_EVENT_SOURCEIMPL_HPP

namespace OpenP2P {

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
