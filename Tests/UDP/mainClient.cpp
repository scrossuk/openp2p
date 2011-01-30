#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>
#include <iostream>

using namespace OpenP2P;

int main(){
	std::cout << "Start client" << std::endl;

	UDP::Socket socket;
	socket.send(UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 45557), MakeBuffer<uint16_t>(0));
	
	std::cout << "Sent: 0" << std::endl;
	
	uint16_t v = 1;
	
	while(true){
		UDP::Endpoint endpoint;
		Buffer data;
		
		if(!socket.receive(endpoint, data, Timeout(5.0))){
			std::cout << "Server failed to respond in time" << std::endl;
			break;
		}
		
		BufferIterator iterator(data);
		uint16_t i;
		
		iterator >> i;
		
		std::cout << "Received: " << i << " from " << endpoint << std::endl;
		
		if(i != v){
			std::cout << "Incorrect data received: " << i << std::endl;
			break;
		}
		
		if(i < 10000){
			v += 2;
			std::cout << "Sent: " << (i + 1) << std::endl;
			socket.send(endpoint, MakeBuffer<uint16_t>(i + 1));
		}
		
		if(i >= 9999){
			std::cout << "Completed Successfully" << std::endl;
			break;
		}
	}

	return 0;
}

