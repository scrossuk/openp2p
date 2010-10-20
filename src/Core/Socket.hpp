#ifndef OPENP2P_CORE_SOCKET_HPP
#define OPENP2P_CORE_SOCKET_HPP

#include <boost/function.hpp>
#include "SharedCount.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;
		class Endpoint;

		class Socket: public SharedCount{
			public:
				virtual void Send(Endpoint, Buffer) = 0;

				virtual ~Socket(){ }

		};

		typedef boost::function<Socket* (boost::function<void (Endpoint, Buffer)>)> SocketBuilder;

	}

}

#endif
