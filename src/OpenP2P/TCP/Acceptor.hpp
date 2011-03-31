#ifndef OPENP2P_TCP_ACCEPTOR_HPP
#define OPENP2P_TCP_ACCEPTOR_HPP

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		class Acceptor: public Cancellable, boost::noncopyable{
			public:
				Acceptor(unsigned short port);

				bool accept(Stream& stream);

				void cancel();

			private:
				IOService service_;
				Mutex mutex_;
				boost::asio::ip::tcp::acceptor internalAcceptor_;

		};

	}

}

#endif
