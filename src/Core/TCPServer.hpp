#ifndef OPENP2P_CORE_TCPSERVER_HPP
#define OPENP2P_CORE_TCPSERVER_HPP

#include <boost/function.hpp>
#include "Stream.hpp"

namespace openp2p{

	namespace Core{

		class TCPServer{
			public:
				TCPServer(unsigned short, boost::function<void (StreamBuilder)>);

				~TCPServer();

			private:
				void * mInternal;

		};

	}

}

#endif
