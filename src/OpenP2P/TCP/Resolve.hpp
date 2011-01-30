#ifndef OPENP2P_TCP_RESOLVE_HPP
#define OPENP2P_TCP_RESOLVE_HPP

#include <list>
#include <string>
#include <boost/asio.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P{

	namespace TCP{

		std::list<Endpoint> Resolve(const std::string&, const std::string&, WaitHandler = Block);

	}

}

#endif
