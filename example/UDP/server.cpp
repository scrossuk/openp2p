#include <stdio.h>

#include <map>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>
#include <p2p/Transport.hpp>
#include <p2p/UDP.hpp>

using namespace p2p;

int main() {
	UDP::Socket socket(45557);
	
	std::map<UDP::Endpoint, uint16_t> expectedValueMap;
	
	printf("UDP Server Started.\n");
	
	while (socket.isValid()) {
		UDP::Endpoint endpoint;
		Buffer data;
		
		if (!socket.receive(endpoint, data)) {
			Event::Wait({ socket.eventSource() });
			continue;
		}
		
		uint16_t& expectedValue = expectedValueMap[endpoint];
		
		BufferIterator iterator(data);
		BinaryIStream blockingReader(iterator);
		const uint16_t i = Binary::ReadUint16(blockingReader);
		
		printf("Received: %u from '%s'.\n",
			(unsigned) i, endpoint.toString().c_str());
		
		if (i != expectedValue) {
			printf("Incorrect data received: %u from endpoint '%s'.\n",
				(unsigned) i, endpoint.toString().c_str());
			expectedValue = 0;
			continue;
		}
		
		if (i >= 9999) {
			printf("Completed successfully for endpoint '%s'.\n",
				endpoint.toString().c_str());
			expectedValue = 0;
			continue;
		}
		
		Buffer buffer;
		BufferBuilder builder(buffer);
		BinaryOStream blockingWriter(builder);
		Binary::WriteUint16(blockingWriter, i + 1);
		socket.send(endpoint, buffer);
		
		printf("Sent: %u.\n", (unsigned) (i + 1));
		
		expectedValue += 2;
	}
	
	return 0;
}

