#include <stdint.h>
#include <string>
#include <iostream>
#include <p2p.hpp>
#include <p2p/UDP.hpp>

#include "TestIdGenerator.hpp"
#include "TestRPCSocket.hpp"

int main() {
	UDP::Socket socket;
	TestRPCSocket rpcSocket(socket);
	TestIdGenerator idGenerator;
	
	RPC::Protocol<UDP::Endpoint, uint32_t> rpcProtocol(rpcSocket, idGenerator);
	
	Buffer buffer;
	BufferBuilder builder(buffer);
	BinaryOStream buildStream(builder);
	std::string message("Hello from client");
	buildStream << message;
	
	RPC::Call<UDP::Endpoint, uint32_t> call(rpcProtocol, UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 45557), buffer);
	
	boost::optional<Buffer> reply = call.execute();
	
	if (reply) {
		BufferIterator iterator(*reply);
		BinaryIStream stream(iterator);
		std::string string(17, 0);
		stream >> string;
		
		std::cout << "Got reply: " << string << std::endl;
	} else {
		std::cout << "Failed to get reply" << std::endl;
	}
	
	return 0;
}

