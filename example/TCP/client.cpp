#include <stdint.h>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>

int main(){
	OpenP2P::TCP::Stream tcpStream;
	std::cout << "---Connecting" << std::endl;

	OpenP2P::Future<bool> connect = tcpStream.connect(OpenP2P::TCP::Endpoint(boost::asio::ip::address_v4::loopback(), 45556));
	
	if(!connect.timedWait(5.0)){
		std::cout << "---Timed out" << std::endl;
		return 0;
	}

	if(connect.get()){
		std::cout << "---Successfully connected" << std::endl;

		OpenP2P::BinaryIOStream stream(tcpStream);
		for(unsigned int i = 0; i < 1000; i += 2){
			stream << uint32_t(i);

			uint32_t v;
			stream >> v;

			if(v != (i + 1)){
				std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << std::endl;
				return 1;
			}else{
				std::cout << "Received: " << v << std::endl;
			}
		}

		std::cout << "---Successfully completed transfer" << std::endl;
	}else{
		std::cout << "---Failed to connect" << std::endl;
	}

	return 0;
}
