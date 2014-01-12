#include "Source.hpp"

namespace OpenP2P {

	namespace Event {
	
		Source::Source() : impl_(nullptr) { }
		
		Source::Source(SourceImpl* implPtr) : impl_(implPtr) { }
		
		SourceImpl* Source::impl() {
			return impl_;
		}
		
	}
	
}

