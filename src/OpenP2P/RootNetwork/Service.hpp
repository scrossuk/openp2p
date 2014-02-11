#ifndef OPENP2P_ROOTNETWORK_SERVICE_HPP
#define OPENP2P_ROOTNETWORK_SERVICE_HPP

#include <stdint.h>

#include <queue>

#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class Service {
			public:
				Service(Socket<Endpoint, Packet>& socket);
				~Service();
				
				NodeId identifyEndpoint(const Endpoint& endpoint);
				
				void processRequests();
				
			private:
				// Non-copyable.
				Service(const Service&) = delete;
				Service& operator=(Service) = delete;
				
				Socket<Endpoint, Packet>& socket_;
				uint32_t nextRoutine_;
				std::queue<Packet> packetQueue_;
				
		};
		
	}
	
}

#endif
