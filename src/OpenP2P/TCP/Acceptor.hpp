#ifndef OPENP2P_TCP_ACCEPTOR_HPP
#define OPENP2P_TCP_ACCEPTOR_HPP

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Future.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		class Acceptor: boost::noncopyable{
			public:
				Acceptor(unsigned short port);

				Future<bool> accept(Stream& stream);

				void cancel();

			private:
				boost::asio::ip::tcp::acceptor internalAcceptor_;

		};

	}

}

#endif
