#include <boost/asio.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Acceptor.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void acceptCallback(Mutex& mutex, Condition& condition, bool& success, const boost::system::error_code& ec){
				Lock lock(mutex);
				success = !bool(ec);
				condition.notify();
			}

		}

		Acceptor::Acceptor(unsigned short port) : internalAcceptor_(service_){
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
			boost::system::error_code ec;

			internalAcceptor_.open(endpoint.protocol(), ec);

			boost::asio::socket_base::reuse_address option(true);
			internalAcceptor_.set_option(option, ec);

			internalAcceptor_.bind(endpoint, ec);

			internalAcceptor_.listen(boost::asio::ip::tcp::socket::max_connections, ec);
		}

		bool Acceptor::accept(Stream& stream){
			Condition condition;
			Lock lock(mutex_);

			bool success = false;
			internalAcceptor_.async_accept(stream.getInternal(), boost::bind(acceptCallback, boost::ref(mutex_), boost::ref(condition), boost::ref(success), _1));

			condition.wait(lock);

			return success;
		}

		void Acceptor::cancel(){
			Lock lock(mutex_);
			internalAcceptor_.cancel();
		}

	}

}

