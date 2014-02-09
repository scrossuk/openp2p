#ifndef OPENP2P_ROOTNETWORK_SERVICE_HPP
#define OPENP2P_ROOTNETWORK_SERVICE_HPP

#include <stdint.h>

#include <queue>

#include <OpenP2P/Socket.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class Service {
			public:
				Service(Socket<Endpoint, Packet>& socket, PrivateIdentity& identity);
				~Service();
				
				NodeId identifyEndpoint(const Endpoint& endpoint);
				
				void processRequests();
				
			private:
				Socket<Endpoint, Packet>& socket_;
				PrivateIdentity& identity_;
				uint32_t nextRoutine_;
				std::queue<Packet> packetQueue_;
				
		};
		
	}
	
}

#endif
