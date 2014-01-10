#ifndef OPENP2P_ROOTNETWORK_ENDPOINT_HPP
#define OPENP2P_ROOTNETWORK_ENDPOINT_HPP

#include <OpenP2P/IP/Endpoint.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		struct Endpoint {
			enum Type {
				Local = 0,
				UDPIPv4 = 1,
				UDPIPv6 = 2,
				TCPIPv4 = 3,
				TCPIPv6 = 4
			};
			
			union {
				IP::Endpoint ipEndpoint;
			};
		};
		
	}
	
}

#endif
