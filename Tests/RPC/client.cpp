#include <stdint.h>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>

#include "TestIdGenerator.hpp"
#include "TestRPCSocket.hpp"

int main(){
	UDP::Socket socket;
	TestRPCSocket rpcSocket(socket);
	TestIdGenerator idGenerator;

	RPCProtocol<UDP::Endpoint, uint32_t> rpcProtocol(rpcSocket, idGenerator);

	BufferBuilder builder;
	BinaryStream buildStream(builder);
	std::string message("Hello from client");
	buildStream << message;

	boost::optional<Buffer> reply = rpcProtocol.execute(UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 45557), builder.getBuffer());

	if(reply){
		BufferIterator iterator(*reply);
		BinaryStream stream(iterator);
		std::string string(17, 0);
		stream >> string;

		std::cout << "Got reply: " << string << std::endl;
	}else{
		std::cout << "Failed to get reply" << std::endl;
	}

	return 0;
}

