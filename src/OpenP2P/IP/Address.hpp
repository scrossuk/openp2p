#ifndef OPENP2P_IP_ADDRESS_HPP
#define OPENP2P_IP_ADDRESS_HPP

#include <OpenP2P/IP/Version.hpp>
#include <OpenP2P/IP/V4Address.hpp>
#include <OpenP2P/IP/V6Address.hpp>

namespace OpenP2P{

	namespace IP{
	
		typedef boost::asio::ip::address AddressImplType;
		
		struct Address{
			Version version;
			
			union{
				V4Address v4Address;
				V6Address v6Address;
			};
			
			static AddressImplType ToImpl(const Address&);
			
			static Address FromImpl(const AddressImplType&);
		};
		
	}
	
}

#endif
