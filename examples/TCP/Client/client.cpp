#include <stdint.h>
#include <iostream>

#include <p2p/TCP.hpp>
#include <p2p/Transport.hpp>

int main() {
	using namespace p2p;
	
	TCP::Stream tcpStream;
	std::cout << "---Connecting" << std::endl;
	
	if (!tcpStream.connect(TCP::Endpoint(IP::V4Address::Localhost(), 45556))) {
		std::cout << "---Failed to connect" << std::endl;
		return 0;
	}
	
	std::cout << "---Successfully connected" << std::endl;
	
	BinaryIOStream stream(tcpStream);
	
	for (unsigned int i = 0; i < 1000; i += 2) {
		Binary::WriteUint32(stream.output(), i);
		
		std::cout << "Sent: " << i << std::endl;
		
		const uint32_t v = Binary::ReadUint32(stream.input());
		
		if (v != (i + 1)) {
			std::cout << "Wrong number: " << v << ", Expected: " << (i + 1) << std::endl;
			return 0;
		} else {
			std::cout << "Received: " << v << std::endl;
		}
	}
	
	std::cout << "---Successfully completed transfer" << std::endl;
	
	return 0;
}

