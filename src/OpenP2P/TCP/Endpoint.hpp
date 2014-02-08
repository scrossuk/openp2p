#ifndef OPENP2P_TCP_ENDPOINT_HPP
#define OPENP2P_TCP_ENDPOINT_HPP

#include <stdint.h>

#include <OpenP2P/IP/Address.hpp>

namespace OpenP2P {
	
	namespace TCP {
	
		struct Endpoint {
			IP::Address address;
			uint16_t port;
			
			inline Endpoint()
				: address(), port(0) { }
				
			inline Endpoint(const IP::Address& pAddress, uint16_t pPort)
				: address(pAddress), port(pPort) { }
		};
		
	}
	
}

#endif
