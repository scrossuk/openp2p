#ifndef OPENP2P_EVENT_SOURCE_HPP
#define OPENP2P_EVENT_SOURCE_HPP

namespace OpenP2P {

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
