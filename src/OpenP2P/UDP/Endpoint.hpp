#ifndef OPENP2P_UDP_ENDPOINT_HPP
#define OPENP2P_UDP_ENDPOINT_HPP

#include <boost/asio.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>

namespace OpenP2P{

	namespace UDP{

		typedef boost::asio::ip::udp::endpoint Endpoint;

	}

	IStream& operator>>(IStream&, UDP::Endpoint&);
	OStream& operator<<(OStream&, const UDP::Endpoint&);

}
#endif
