#ifndef P2P_ROOTNETWORK_ENDPOINT_HPP
#define P2P_ROOTNETWORK_ENDPOINT_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/TCP/Endpoint.hpp>
#include <p2p/UDP/Endpoint.hpp>

#include <p2p/Transport.hpp>

namespace p2p {

	namespace Root {
	
		struct ROOT_OPENP2P_API Endpoint {
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
			
			bool operator<(const Endpoint& endpoint) const;
			
			void writeTo(BlockingWriter& writer) const;
			
			std::string toString() const;
		};
		
	}
	
}

#endif
