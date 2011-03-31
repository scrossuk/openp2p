#ifndef OPENP2P_ROOTNETWORK_IDSOCKET_HPP
#define OPENP2P_ROOTNETWORK_IDSOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/IdSocket.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		class IdSocket: public OpenP2P::IdSocket<Endpoint, Id>{
			public:
				IdSocket(Socket<Endpoint>& socket);

				void send(const Endpoint& endpoint, const Id& id, const Buffer& data);
				
				bool receive(Endpoint& endpoint, Id& id, Buffer& data);

				void cancel();

			private:
				Socket<Endpoint>& socket_;

		};

	}

}

#endif
