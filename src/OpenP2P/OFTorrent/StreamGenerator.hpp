#ifndef OPENP2P_OFTORRENT_STREAMGENERATOR_HPP
#define OPENP2P_OFTORRENT_STREAMGENERATOR_HPP

#include <OpenP2P/Stream.hpp>

namespace OpenP2P{

	namespace OFTorrent{

		class OStreamGenerator{
			public:
				virtual OStream& getOStream() = 0;

		};

	}

}

#endif
