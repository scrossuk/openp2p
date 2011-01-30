#ifndef OPENP2P_TCP_CONNECT_HPP
#define OPENP2P_TCP_CONNECT_HPP

#include <list>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P{

	namespace TCP{

		class Stream;

		boost::optional<Stream> Connect(const Endpoint&, WaitHandler = Block);

		boost::optional<Stream> Connect(const std::list<Endpoint>&, WaitHandler = Block);

	}

}

#endif
