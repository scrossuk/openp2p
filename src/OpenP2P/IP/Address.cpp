#include <OpenP2P/String.hpp>

#include <OpenP2P/IP/Address.hpp>

namespace OpenP2P {

	namespace IP {
	
		AddressImplType Address::ToImpl(const Address& address) {
			switch (address.version) {
				case v4:
					return V4Address::ToImpl(address.v4Address);
					
				case v6:
					return V6Address::ToImpl(address.v6Address);
			}
			
			return AddressImplType();
		}
		
		Address Address::FromImpl(const AddressImplType& addressImpl) {
			if (addressImpl.is_v4()) {
				Address address;
				address.version = v4;
				address.v4Address = V4Address::FromImpl(addressImpl.to_v4());
				return address;
			} else {
				Address address;
				address.version = v6;
				address.v6Address = V6Address::FromImpl(addressImpl.to_v6());
				return address;
			}
		}
		
		Address::Address() { }
		
		Address::Address(const V4Address& pAddress)
			: version(v4), v4Address(pAddress) { }
			
		Address::Address(const V6Address& pAddress)
			: version(v6), v6Address(pAddress) { }
		
		bool Address::operator==(const Address& other) const {
			return version == other.version &&
				((version == v4) ?
					(v4Address == other.v4Address) :
					(v6Address == other.v6Address)
				);
		}
		
		bool Address::operator!=(const Address& other) const {
			return !(*this == other);
		}
		
		bool Address::operator<(const Address& other) const {
			return (version != other.version) ? (version < other.version) :
				((version == v4) ?
					(v4Address < other.v4Address) :
					(v6Address < other.v6Address)
				);
		}
		
		std::string Address::toString() const {
			if (version == v4) {
				return STR("IPv4Address(%s)",
					v4Address.toString().c_str());
			} else {
				return STR("IPv6Address(%s)",
					v6Address.toString().c_str());
			}
		}
			
	}
	
}

