#ifndef OPENP2P_IP_V6ADDRESS_HPP
#define OPENP2P_IP_V6ADDRESS_HPP

#include <stdint.h>

#include <array>
#include <string>

#include <boost/asio.hpp>

namespace OpenP2P {

	namespace IP {
	
		typedef boost::asio::ip::address_v6 V6AddressImplType;
		
		struct V6Address {
			std::array<uint8_t, 16> data;
			
			static V6AddressImplType ToImpl(const V6Address&);
			
			static V6Address FromImpl(const V6AddressImplType&);
			
			static V6Address Any();
			
			static V6Address Localhost();
			
			bool operator==(const V6Address& other) const;
			bool operator<(const V6Address& other) const;
			
			std::string toString() const;
			
		};
		
	}
	
}

#endif
