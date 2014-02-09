#ifndef OPENP2P_ROOTNETWORK_ENDPOINT_HPP
#define OPENP2P_ROOTNETWORK_ENDPOINT_HPP

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/UDP/Endpoint.hpp>

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
			
			inline static Endpoint UDP(const UDP::Endpoint& udpEndpoint) {
				Endpoint endpoint;
				
				// TODO: detect this.
				endpoint.kind = UDPIPV6;
				
				endpoint.udpEndpoint = udpEndpoint;
				return endpoint;
			}
			
			inline Endpoint()
				: kind(LOCAL) { }
			
			inline Endpoint(const UDP::Endpoint& pUdpEndpoint)
				// TODO: detect this.
				: kind(UDPIPV6), udpEndpoint(pUdpEndpoint) { }
		};
		
	}
	
}

#endif
