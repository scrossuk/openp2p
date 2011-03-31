#include <stdint.h>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>

int main(){
	OpenP2P::TCP::Stream tcpStream;
	OpenP2P::Timeout timeout(5.0, tcpStream);
	bool success = tcpStream.connect(OpenP2P::TCP::Endpoint(boost::asio::ip::address_v4::loopback(), 45556));

	std::cout << "Starting" << std::endl;

	if(success){
		OpenP2P::BinaryStream stream(tcpStream);
		for(unsigned int i = 0; i < 1000; i += 2){
			stream << uint32_t(i);

			uint32_t v;
			stream >> v;

			if(v != (i + 1)){
				std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << std::endl;
				return 1;
			}
		}
	}else{
		std::cout << "Failed to connect" << std::endl;
	}

	return 0;
}

