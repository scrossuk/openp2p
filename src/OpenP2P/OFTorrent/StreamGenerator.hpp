#ifndef OPENP2P_OFTORRENT_STREAMGENERATOR_HPP
#define OPENP2P_OFTORRENT_STREAMGENERATOR_HPP

#include <OpenP2P/Stream.hpp>

namespace OpenP2P {

	namespace OFTorrent {
	
		class OutputStreamGenerator {
			public:
				virtual OStream& getNextOutputStream() = 0;
				
		};
		
	}
	
}

#endif
