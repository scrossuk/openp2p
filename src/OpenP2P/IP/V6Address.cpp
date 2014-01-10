#include <boost/asio.hpp>
#include <OpenP2P/IP/V6Address.hpp>

namespace OpenP2P {

	namespace IP {
	
		V6AddressImplType V6Address::ToImpl(const V6Address& address) {
			return V6AddressImplType(address.data);
		}
		
		V6Address V6Address::FromImpl(const V6AddressImplType& addressImpl) {
			V6Address address;
			address.data = addressImpl.to_bytes();
			return address;
		}
		
		V6Address V6Address::Any() {
			return V6Address::FromImpl(boost::asio::ip::address_v6::any());
		}
		
		V6Address V6Address::Localhost() {
			return V6Address::FromImpl(boost::asio::ip::address_v6::loopback());
		}
		
	}
	
}

