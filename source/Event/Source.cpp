#include <p2p/Event/Source.hpp>

namespace p2p {

	namespace Event {
	
		Source::Source() : impl_(nullptr) { }
		
		Source::Source(SourceImpl* implPtr) : impl_(implPtr) { }
		
		SourceImpl* Source::impl() {
			return impl_;
		}
		
	}
	
}

