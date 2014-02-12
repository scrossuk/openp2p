#ifndef OPENP2P_ROOTNETWORK_ENDPOINT_HPP
#define OPENP2P_ROOTNETWORK_ENDPOINT_HPP

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/UDP/Endpoint.hpp>

#include <OpenP2P/Stream/BinaryStream.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		struct Endpoint {
			enum Kind {
				LOCAL = 0,
				UDPIPV4 = 1,
				UDPIPV6 = 2,
				TCPIPV4 = 3,
				TCPIPV6 = 4
			} kind;
			
			UDP::Endpoint udpEndpoint;
			TCP::Endpoint tcpEndpoint;
			
			Endpoint();
			Endpoint(const UDP::Endpoint& udpEndpoint);
			
			static Endpoint Read(BlockingReader& reader);
			
			void writeTo(BlockingWriter& writer) const;
		};
		
	}
	
}

#endif
