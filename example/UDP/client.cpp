#include <stdio.h>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>
#include <p2p/Transport.hpp>
#include <p2p/UDP.hpp>

using namespace p2p;

int main() {
	printf("UDP Client Started.\n");
	
	UDP::Socket socket;
	
	{
		Buffer buffer;
		BufferBuilder builder(buffer);
		BinaryOStream binaryStream(builder);
		Binary::WriteUint16(binaryStream, 0);
		socket.send(UDP::Endpoint(IP::V6Address::Localhost(), 45557), buffer);
		
		printf("Sent: 0\n");
	}
	
	uint16_t expectedValue = 1;
	
	while (true) {
		UDP::Endpoint endpoint;
		Buffer data;
		
		while (!socket.receive(endpoint, data)) {
			if (!socket.isValid()) {
				printf("Socket became invalid.\n");
				return -1;
			}
			
			Event::Wait({ socket.eventSource() });
		}
		
		BufferIterator iterator(data);
		BinaryIStream blockingReader(iterator);
		const uint16_t i = Binary::ReadUint16(blockingReader);
		
		printf("Received: %u from '%s'.\n",
			(unsigned) i, endpoint.toString().c_str());
		
		if (i != expectedValue) {
			printf("Incorrect data received: %u.\n", (unsigned) i);
			return -1;
		}
		
		if (i >= 9999) {
			printf("Completed successfully.\n");
			return 0;
		}
		
		Buffer buffer;
		BufferBuilder builder(buffer);
		BinaryOStream blockingWriter(builder);
		Binary::WriteUint16(blockingWriter, i + 1);
		socket.send(endpoint, buffer);
		
		printf("Sent: %u.\n", (unsigned) (i + 1));
		
		expectedValue += 2;
	}
}

