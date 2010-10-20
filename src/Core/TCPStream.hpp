#ifndef OPENP2P_CORE_TCPSTREAM_HPP
#define OPENP2P_CORE_TCPSTREAM_HPP

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Stream.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;

		class TCPStream: public Stream{
			public:
				TCPStream(boost::asio::ip::tcp::socket *, Stream *);
				~TCPStream();

				void Data(Buffer);

			private:
				void * mInternal;

		};

		inline Stream * buildTCPStream(boost::asio::ip::tcp::socket * socket, Stream * stream){
			return new TCPStream(socket, stream);
		}

		inline StreamBuilder TCPStreamBuilder(boost::asio::ip::tcp::socket * socket){
			return boost::bind(buildTCPStream, socket, _1);
		}

	}

}

#endif
