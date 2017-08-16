#ifndef P2P_EVENT_SOURCE_HPP
#define P2P_EVENT_SOURCE_HPP

#include <p2p/Event/Event_api.hpp>

namespace p2p {

	namespace Event {
	
		class SourceImpl;
		
		class EVENT_OPENP2P_API Source {
			public:
				Source();
				Source(SourceImpl*);
				
				SourceImpl* impl();
				
			private:
				SourceImpl* impl_;
				
		};
		
	}
	
}

#endif
