#ifndef OPENP2P_IP_V4ADDRESS_HPP
#define OPENP2P_IP_V4ADDRESS_HPP

#include <stdint.h>

#include <array>
#include <string>

#include <boost/asio.hpp>

namespace OpenP2P {

	namespace IP {
	
		typedef boost::asio::ip::address_v4 V4AddressImplType;
		
		struct V4Address {
			std::array<uint8_t, 4> data;
			
			static V4AddressImplType ToImpl(const V4Address&);
			
			static V4Address FromImpl(const V4AddressImplType&);
			
			static V4Address Any();
			
			static V4Address Broadcast();
			
			static V4Address Localhost();
			
			static V4Address Netmask(const V4Address&);
			
			bool operator==(const V4Address& other) const;
			bool operator<(const V4Address& other) const;
			
			std::string toString() const;
			
		};
		
	}
	
}

#endif
