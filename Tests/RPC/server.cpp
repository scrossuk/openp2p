#include <stdint.h>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>

#include "TestIdGenerator.hpp"
#include "TestRPCSocket.hpp"

int main(){
	UDP::Socket socket(45557);
	TestRPCSocket rpcSocket(socket);
	TestIdGenerator idGenerator;

	RPCProtocol<UDP::Endpoint, uint32_t> rpcProtocol(rpcSocket, idGenerator);

	uint32_t rpcId;
	UDP::Endpoint endpoint;
	Buffer buffer;

	unsigned int count = 0;

	while(rpcProtocol.receiveRequest(rpcId, endpoint, buffer)){
		BufferIterator iterator(buffer);
		BinaryStream stream(iterator);
		std::string string(17, 0);
		stream >> string;

		std::cout << "Message(" << count << ") " << rpcId << ": " << string << std::endl;
		count++;

		BufferBuilder builder;
		BinaryStream buildStream(builder);
		std::string message("Hello from server");
		buildStream << message;

		rpcProtocol.sendReply(rpcId, endpoint, builder.getBuffer());
	}

	return 0;
}

