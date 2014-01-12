#ifndef OPENP2P_EVENT_WAIT_HPP
#define OPENP2P_EVENT_WAIT_HPP

#include <vector>

namespace OpenP2P {

	namespace Event {
	
		class Source;
		
		void Wait(const std::vector<Source>& sources);
		
		inline void Wait(Source source) {
			Wait(std::vector<Source>(1, source));
		}
		
	}
	
}

#endif
