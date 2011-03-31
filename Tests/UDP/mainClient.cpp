#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>
#include <iostream>

using namespace OpenP2P;

int main(){
	std::cout << "Start client" << std::endl;

	UDP::Socket socket;

	{
		BufferBuilder builder;
		BinaryStream binaryStream(builder);
		binaryStream << uint16_t(0);

		socket.send(UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 45557), builder.getBuffer());
	}
	
	std::cout << "Sent: 0" << std::endl;
	
	uint16_t v = 1;
	
	while(true){
		UDP::Endpoint endpoint;
		Buffer data;
		
		if(!socket.receive(endpoint, data)){
			std::cout << "Server failed to respond in time" << std::endl;
			break;
		}
		
		uint16_t i;
		
		{
			BufferIterator iterator(data);
			BinaryStream binaryStream(iterator);
			binaryStream >> i;
		}
		
		std::cout << "Received: " << i << " from " << endpoint << std::endl;
		
		if(i != v){
			std::cout << "Incorrect data received: " << i << std::endl;
			break;
		}
		
		if(i < 10000){
			v += 2;
			std::cout << "Sent: " << (i + 1) << std::endl;
			BufferBuilder builder;
			BinaryStream binaryStream(builder);
			binaryStream << uint16_t(i + 1);
			socket.send(endpoint, builder.getBuffer());
		}
		
		if(i >= 9999){
			std::cout << "Completed Successfully" << std::endl;
			break;
		}
	}

	return 0;
}

