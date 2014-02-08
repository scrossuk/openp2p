#ifndef OPENP2P_IP_ADDRESS_HPP
#define OPENP2P_IP_ADDRESS_HPP

#include <string>

#include <OpenP2P/IP/Version.hpp>
#include <OpenP2P/IP/V4Address.hpp>
#include <OpenP2P/IP/V6Address.hpp>

namespace OpenP2P {

	namespace IP {
	
		typedef boost::asio::ip::address AddressImplType;
		
		struct Address {
			Version version;
			
			union {
				V4Address v4Address;
				V6Address v6Address;
			};
			
			static AddressImplType ToImpl(const Address&);
			
			static Address FromImpl(const AddressImplType&);
			
			Address();
			Address(const V4Address& pAddress);
			Address(const V6Address& pAddress);
			
			bool operator==(const Address& other) const;
			bool operator!=(const Address& other) const;
			bool operator<(const Address& other) const;
			
			std::string toString() const;
			
		};
		
	}
	
}

#endif
