#ifndef OPENP2P_EVENT_WAIT_HPP
#define OPENP2P_EVENT_WAIT_HPP

#include <vector>

#include <OpenP2P/Event/Source.hpp>

namespace OpenP2P {

	namespace Event {
	
		void Wait(const std::vector<Source>& sources);
		
		inline void Wait(Source source) {
			Wait(std::vector<Source>(1, source));
		}
		
	}
	
}

#endif
