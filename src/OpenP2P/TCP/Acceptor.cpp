#include <boost/asio.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/IOService.hpp>

#include <OpenP2P/TCP/Acceptor.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void acceptCallback(Promise<bool> acceptResult, const boost::system::error_code& ec){
				acceptResult.setValue(!bool(ec));
			}

		}

		Acceptor::Acceptor(unsigned short port) : internalAcceptor_(GetIOService()){
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
			boost::system::error_code ec;

			internalAcceptor_.open(endpoint.protocol(), ec);

			boost::asio::socket_base::reuse_address option(true);
			internalAcceptor_.set_option(option, ec);

			internalAcceptor_.bind(endpoint, ec);

			internalAcceptor_.listen(boost::asio::ip::tcp::socket::max_connections, ec);
		}

		Future<bool> Acceptor::accept(Stream& stream){
			Promise<bool> acceptResult;
			
			internalAcceptor_.async_accept(stream.getInternal(), boost::bind(acceptCallback, acceptResult, _1));

			return acceptResult;
		}
		
		void Acceptor::cancel(){
			internalAcceptor_.cancel();
		}

	}

}

