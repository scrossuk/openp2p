#ifndef OPENP2P_RPC_SOCKET_HPP
#define OPENP2P_RPC_SOCKET_HPP

#include <OpenP2P/Buffer.hpp>

namespace OpenP2P {

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
