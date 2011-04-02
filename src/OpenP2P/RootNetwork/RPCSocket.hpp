#ifndef OPENP2P_ROOTNETWORK_RPCSOCKET_HPP
#define OPENP2P_ROOTNETWORK_RPCSOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/RPCSocket.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		class RPCSocket: public OpenP2P::RPCSocket<Endpoint, Id>{
			public:
				RPCSocket(Socket<Endpoint>& socket);

				void send(const Endpoint& endpoint, bool isRequest, const Id& id, const Buffer& data);

				bool receive(Endpoint& endpoint, bool& isRequest, Id& id, Buffer& data);

				void cancel();

                void close();

			private:
				Socket<Endpoint>& socket_;

		};

	}

}

#endif
