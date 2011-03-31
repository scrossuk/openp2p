#ifndef OPENP2P_TCP_ENDPOINT_HPP
#define OPENP2P_TCP_ENDPOINT_HPP

#include <boost/asio.hpp>
#include <OpenP2P/BinaryStream.hpp>

namespace OpenP2P{

	namespace TCP{

		typedef boost::asio::ip::tcp::endpoint Endpoint;

	}

	BinaryStream& operator>>(BinaryStream&, TCP::Endpoint&);
	BinaryStream& operator<<(BinaryStream&, const TCP::Endpoint&);

}
#endif
