#ifndef OPENP2P_UDP_ENDPOINT_HPP
#define OPENP2P_UDP_ENDPOINT_HPP

#include <stdint.h>

#include <string>

#include <OpenP2P/String.hpp>
#include <OpenP2P/IP/Address.hpp>

namespace OpenP2P {
	
	namespace UDP {
	
		struct Endpoint {
			IP::Address address;
			uint16_t port;
			
			inline Endpoint()
				: address(), port(0) { }
				
			inline Endpoint(const IP::Address& pAddress, uint16_t pPort)
				: address(pAddress), port(pPort) { }
			
			inline std::string toString() const {
				return STR("UDP::Endpoint(address = %s, port = %u)",
					address.toString().c_str(), (unsigned) port);
			}
			
			inline bool operator<(const Endpoint& other) const {
				return (address != other.address) ? (address < other.address) : (port < other.port);
			}
		};
		
	}
	
}

#endif
