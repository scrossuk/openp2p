#ifndef OPENP2P_SOCKET_HPP
#define OPENP2P_SOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Cancellable.hpp>

namespace OpenP2P{

	template <typename EndpointType>
	class Socket: public Cancellable{
		public:
			virtual bool send(const EndpointType&, const Buffer&) = 0;


	};

}

#endif
