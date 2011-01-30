#ifndef OPENP2P_TCP_ACCEPT_HPP
#define OPENP2P_TCP_ACCEPT_HPP

#include <list>
#include <boost/asio.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	namespace TCP{

		class Stream;

		boost::optional<Stream> Accept(unsigned short, WaitHandler = Block);

	}

}

#endif
