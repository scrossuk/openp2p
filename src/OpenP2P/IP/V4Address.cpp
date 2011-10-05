#include <boost/asio.hpp>
#include <OpenP2P/IP/V4Address.hpp>

namespace OpenP2P{

	namespace IP{
	
		V4AddressImplType V4Address::ToImpl(const V4Address& address){
			return V4AddressImplType(address.data);
		}
			
		V4Address V4Address::FromImpl(const V4AddressImplType& addressImpl){
			V4Address address;
			address.data = addressImpl.to_bytes();
			return address;
		}
			
		V4Address V4Address::Any(){
			return V4Address::FromImpl(boost::asio::ip::address_v4::any());
		}
			
		V4Address V4Address::Broadcast(){
			return V4Address::FromImpl(boost::asio::ip::address_v4::broadcast());
		}
			
		V4Address V4Address::Localhost(){
			return V4Address::FromImpl(boost::asio::ip::address_v4::loopback());
		}
			
		V4Address V4Address::Netmask(const V4Address& address){
			return V4Address::FromImpl(boost::asio::ip::address_v4::netmask(V4Address::ToImpl(address)));
		}
		
	}
	
}

