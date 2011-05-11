#ifndef OPENP2P_UDP_ENDPOINT_HPP
#define OPENP2P_UDP_ENDPOINT_HPP

#include <boost/asio.hpp>
#include <OpenP2P/BinaryStream.hpp>

namespace OpenP2P{

	namespace UDP{

		typedef boost::asio::ip::udp::endpoint Endpoint;

	}

	BinaryIStream& operator>>(BinaryIStream&, UDP::Endpoint&);
	BinaryOStream& operator<<(BinaryOStream&, const UDP::Endpoint&);

}

#endif
