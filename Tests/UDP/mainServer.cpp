#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>
#include <iostream>
#include <map>

using namespace OpenP2P;

int main(){
	UDP::Socket socket(45557);
	
	std::map<UDP::Endpoint, uint16_t> v_map;
	
	std::cout << "Start server" << std::endl;
	
	while(true){
		UDP::Endpoint endpoint;
		Buffer data;
		
		if(!socket.receive(endpoint, data)){
			std::cout << "receive failed" << std::endl;
			continue;
		}
		
		uint16_t& v = v_map[endpoint];
		
		BufferIterator iterator(data);
		uint16_t i;
		
		iterator >> i;
		
		std::cout << "Received: " << i << " from " << endpoint << std::endl;
		
		if(i != v){
			std::cout << "Incorrect data received: " << i << " for endpoint " << endpoint << std::endl;
			v = 0;
			continue;
		}
		
		if(i < 10000){
			v += 2;
			std::cout << "Sent: " << (i + 1) << std::endl;
			socket.send(endpoint, MakeBuffer<uint16_t>(i + 1));
		}
		
		if(i >= 9999){
			std::cout << "Completed Successfully for endpoint " << endpoint << std::endl;
			v = 0;
		}
	}

	return 0;
}

