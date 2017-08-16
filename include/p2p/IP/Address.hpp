#ifndef P2P_IP_ADDRESS_HPP
#define P2P_IP_ADDRESS_HPP

#include <string>

#include <p2p/IP/IP_api.hpp>
#include <p2p/IP/Version.hpp>
#include <p2p/IP/V4Address.hpp>
#include <p2p/IP/V6Address.hpp>

namespace p2p {

	namespace IP {
	
		typedef boost::asio::ip::address AddressImplType;
		
		struct IP_OPENP2P_API Address {
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
