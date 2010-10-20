#include <boost/asio.hpp>
#include "Buffer.hpp"
#include "BufferIterator.hpp"
#include "DataReader.hpp"
#include "Endpoint.hpp"
#include "Types.hpp"

using namespace boost::asio;

namespace openp2p{

	namespace Core{

		enum IPVersion{
			IPVersion4 = 0,
			IPVersion6 = 1
		};

		Endpoint Endpoint::Unserialize(BufferIterator& iterator){
			Uint8 ipVersion;
			Uint8 addressData[16];
			Uint16 port;
			ip::address address;

			DataReader reader(iterator);
			reader >> ipVersion;
			reader >> port;

			if(ipVersion == IPVersion4){
				iterator.Read(addressData, 4);
				boost::array<unsigned char, 4> array;
				for(unsigned int i = 0; i < 4; i++){
					array[i] = addressData[i];
				}
				address = ip::address_v4(array);
			}else{
				iterator.Read(addressData, 16);
				boost::array<unsigned char, 16> array;
				for(unsigned int i = 0; i < 16; i++){
					array[i] = addressData[i];
				}
				address = ip::address_v6(array);
			}

			return Endpoint(address, port);
		}

		Endpoint::Endpoint(ip::address address, Uint16 port) : mAddress(address){
			mPort = port;
		}

		Endpoint::Endpoint(ip::address_v4 address, Uint16 port) : mAddress(address){
			mPort = port;
		}

		Endpoint::Endpoint(ip::address_v6 address, Uint16 port) : mAddress(address){
			mPort = port;
		}

		ip::address Endpoint::Address() const{
			return mAddress;
		}

		Uint16 Endpoint::Port() const{
			return mPort;
		}

		Buffer Endpoint::Serialize() const{
			Uint8 addressVersion;
			const Uint8 * addressData;
			unsigned int addressSize;

			if(mAddress.is_v4()){
				ip::address_v4 address = mAddress.to_v4();
				addressData = (const Uint8 *) address.to_bytes().data();
				addressSize = 4;
				addressVersion = IPVersion4;
			}else{
				ip::address_v6 address = mAddress.to_v6();
				addressData = (const Uint8 *) address.to_bytes().data();
				addressSize = 16;
				addressVersion = IPVersion6;
			}

			return Buffer(addressVersion) + Buffer(mPort) + Buffer(addressData, addressSize);
		}

	}

}

