#ifndef OPENP2P_EVENT_WAIT_HPP
#define OPENP2P_EVENT_WAIT_HPP

#include <vector>

namespace OpenP2P {

	namespace Event {
	
		class Source;
		
		void Wait(const std::vector<Source>& sources);
		
	}
	
}

#endif
