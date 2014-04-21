#ifndef P2P_RPC_SOCKET_HPP
#define P2P_RPC_SOCKET_HPP

#include <p2p/Buffer.hpp>

namespace p2p {

	namespace RPC {
	
		template <typename EndpointType, typename IdType>
		class Socket {
			public:
				virtual void send(const EndpointType& endpoint, bool isRequest, const IdType& id, const Buffer& data) = 0;
				
				virtual bool receive(EndpointType& endpoint, bool& isRequest, IdType& id, Buffer& data) = 0;
				
				virtual void close() = 0;
				
		};
		
	}
	
}

#endif
