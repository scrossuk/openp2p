#ifndef OPENP2P_TCP_ENDPOINT_HPP
#define OPENP2P_TCP_ENDPOINT_HPP

#include <boost/asio.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>

namespace OpenP2P{

	namespace TCP{

		typedef boost::asio::ip::tcp::endpoint Endpoint;

	}

	IStream& operator>>(IStream&, TCP::Endpoint&);
	OStream& operator<<(OStream&, const TCP::Endpoint&);

}
#endif
