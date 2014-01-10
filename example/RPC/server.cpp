#include <stdint.h>
#include <string>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/UDP.hpp>

#include "TestIdGenerator.hpp"
#include "TestRPCSocket.hpp"

int main() {
	UDP::Socket socket(45557);
	TestRPCSocket rpcSocket(socket);
	TestIdGenerator idGenerator;
	
	RPC::Protocol<IP::Endpoint, uint32_t> protocol(rpcSocket, idGenerator);
	
	uint32_t rpcId;
	UDP::Endpoint endpoint;
	Buffer buffer;
	
	unsigned int count = 0;
	
	while (protocol.receiveRequest(endpoint, rpcId, buffer)) {
		BufferIterator iterator(buffer);
		BinaryIStream stream(iterator);
		std::string string(17, 0);
		stream >> string;
		
		std::cout << "Message(" << count << ") " << rpcId << ": " << string << std::endl;
		count++;
		
		Buffer buffer;
		BufferBuilder builder(buffer);
		BinaryOStream buildStream(builder);
		std::string message("Hello from server");
		buildStream << message;
		
		protocol.sendReply(endpoint, rpcId, buffer);
	}
	
	return 0;
}

