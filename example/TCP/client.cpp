#include <stdint.h>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/TCP.hpp>

int main(){
	using namespace OpenP2P;

	TCP::Stream tcpStream;
	std::cout << "---Connecting" << std::endl;

	if(!tcpStream.connect(OpenP2P::TCP::Endpoint(boost::asio::ip::address_v4::loopback(), 45556), Timeout::Seconds(5.0))){
		std::cout << "---Failed to connect" << std::endl;
		return 0;
	}

	std::cout << "---Successfully connected" << std::endl;

	BinaryIOStream stream(tcpStream);
	for(unsigned int i = 0; i < 1000; i += 2){
		if(!Binary::WriteUint32(stream, i)){
			std::cout << "---Failed to write to stream" << std::endl;
			return 0;
		}

		uint32_t v;
		
		if(!Binary::ReadUint32(stream, &v)){
			std::cout << "---Failed to read from stream" << std::endl;
			return 0;
		}

		if(v != (i + 1)){
			std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << std::endl;
			return 0;
		}else{
			std::cout << "Received: " << v << std::endl;
		}
	}

	std::cout << "---Successfully completed transfer" << std::endl;

	return 0;
}

