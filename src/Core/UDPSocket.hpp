#ifndef OPENP2P_CORE_UDPSOCKET_HPP
#define OPENP2P_CORE_UDPSOCKET_HPP

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "Socket.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;
		class Endpoint;

		class UDPSocket: public Socket{
			public:
				UDPSocket(unsigned short, boost::function<void (Endpoint, Buffer)>);
				~UDPSocket();

				void Send(Endpoint, Buffer);

			private:
				void * mInternalSocket;

		};

		inline Socket * buildUDPSocket(unsigned short port, boost::function<void (Endpoint, Buffer)> callback){
			return new UDPSocket(port, callback);
		}

		inline SocketBuilder UDPSocketBuilder(unsigned short port){
			return boost::bind(buildUDPSocket, port, _1);
		}

	}

}

#endif
