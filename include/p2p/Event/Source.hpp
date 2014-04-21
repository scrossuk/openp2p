#ifndef P2P_EVENT_SOURCE_HPP
#define P2P_EVENT_SOURCE_HPP

namespace p2p {

	namespace Event {
	
		class SourceImpl;
		
		class Source {
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
