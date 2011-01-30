#ifndef OPENP2P_SOCKET_HPP
#define OPENP2P_SOCKET_HPP

#include <OpenP2P/Block.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	template <typename EndpointType>
	class Socket{
		public:
			virtual bool send(const EndpointType&, const Buffer&, WaitHandler = Block) = 0;

			virtual bool receive(EndpointType&, Buffer&, WaitHandler = Block) = 0;

	};

}

#endif
