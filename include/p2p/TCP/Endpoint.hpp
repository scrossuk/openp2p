#ifndef P2P_TCP_ENDPOINT_HPP
#define P2P_TCP_ENDPOINT_HPP

#include <stdint.h>

#include <p2p/IP/Address.hpp>

namespace p2p {
	
	namespace TCP {
	
		struct Endpoint {
			IP::Address address;
			uint16_t port;
			
			inline Endpoint()
				: address(), port(0) { }
				
			inline Endpoint(const IP::Address& pAddress, uint16_t pPort)
				: address(pAddress), port(pPort) { }
			
			inline bool operator<(const Endpoint& other) const {
				return (address != other.address) ? (address < other.address) : (port < other.port);
			}
		};
		
	}
	
}

#endif
