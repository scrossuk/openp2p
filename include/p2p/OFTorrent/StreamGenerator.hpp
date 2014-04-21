#ifndef P2P_OFTORRENT_STREAMGENERATOR_HPP
#define P2P_OFTORRENT_STREAMGENERATOR_HPP

#include <p2p/Stream.hpp>

namespace p2p {

	namespace OFTorrent {
	
		class OutputStreamGenerator {
			public:
				virtual OStream& getNextOutputStream() = 0;
				
		};
		
	}
	
}

#endif
