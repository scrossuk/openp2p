#ifndef OPENP2P_CORE_TCPCLIENT_HPP
#define OPENP2P_CORE_TCPCLIENT_HPP

#include <boost/function.hpp>
#include "Stream.hpp"

namespace openp2p{

	namespace Core{

		class Endpoint;

		class TCPClient{
			public:
				TCPClient(Endpoint, boost::function<void (StreamBuilder)>);

				~TCPClient();

			private:
				void * mInternal;

		};

	}

}

#endif
