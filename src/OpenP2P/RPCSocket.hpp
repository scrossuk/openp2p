#ifndef OPENP2P_RPCSOCKET_HPP
#define OPENP2P_RPCSOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Cancellable.hpp>

namespace OpenP2P {

	template <typename EndpointType, typename IdType>
	class RPCSocket: public Cancellable{
		public:
			virtual void send(const EndpointType& endpoint, bool isRequest, const IdType& id, const Buffer& data) = 0;
			
			virtual bool receive(EndpointType& endpoint, bool& isRequest, IdType& id, Buffer& data) = 0;

			virtual void close() = 0;

	};

}

#endif
