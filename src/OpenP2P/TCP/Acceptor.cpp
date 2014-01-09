#include <boost/asio.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Timeout.hpp>

#include <OpenP2P/TCP/Acceptor.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void acceptCallback(Signal* signal, bool * acceptResult, const boost::system::error_code& ec){
				*acceptResult = !bool(ec);
				signal->activate();
			}

		}

		Acceptor::Acceptor() : internalAcceptor_(GetIOService()){ }
		
		bool Acceptor::listen(unsigned short port){
			internalAcceptor_.close();
			
			boost::system::error_code ec;
			
			// This ensures the acceptor uses both IPv4 and IPv6.
			boost::asio::ip::v6_only v6OnlyOption(false);
			internalAcceptor_.set_option(v6OnlyOption, ec);
			
			if(ec) return false;
			
			internalAcceptor_.open(boost::asio::ip::tcp::v6(), ec);
			
			if(ec) return false;

			boost::asio::socket_base::reuse_address reuseAddressOption(true);
			internalAcceptor_.set_option(reuseAddressOption, ec);
			
			if(ec) return false;
			
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v6(), port);

			internalAcceptor_.bind(endpoint, ec);
			
			if(ec) return false;

			internalAcceptor_.listen(boost::asio::ip::tcp::socket::max_connections, ec);
			
			return !bool(ec);
		}

		bool Acceptor::accept(Stream& stream, Timeout timeout){
			bool acceptResult = false;
			
			Signal signal;
			
			internalAcceptor_.async_accept(stream.getInternal(), boost::bind(acceptCallback, &signal, &acceptResult, _1));
			
			if(signal.wait(timeout)){
				return acceptResult;
			}else{
				internalAcceptor_.cancel();
				signal.wait();
				return false;
			}
		}

	}

}

