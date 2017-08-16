#include <memory>
#include <stdexcept>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <p2p/Concurrency/Signal.hpp>

#include <p2p/TCP/Acceptor.hpp>
#include <p2p/TCP/Stream.hpp>

#include <p2p/Event/IOService.hpp>

namespace p2p {

	namespace TCP {
	
		namespace {
		
			void acceptCallback(Signal* signal, bool* acceptResult, const boost::system::error_code& ec) {
				*acceptResult = !bool(ec);
				signal->activate();
			}
			
		}
		
		struct AcceptorImpl {
			boost::asio::ip::tcp::acceptor acceptor;
			
			inline AcceptorImpl()
				: acceptor(Event::GetIOService()) { }
		};
		
		Acceptor::Acceptor(uint16_t port) : impl_(new AcceptorImpl()) {
			boost::system::error_code ec;
			
			impl_->acceptor.open(boost::asio::ip::tcp::v6(), ec);
			
			if (ec) {
				throw std::runtime_error("open() failed.");
			}
			
			// This ensures the acceptor uses both IPv4 and IPv6.
			boost::asio::ip::v6_only v6OnlyOption(false);
			impl_->acceptor.set_option(v6OnlyOption, ec);
			
			if (ec) {
				throw std::runtime_error("Set IPv4 and IPv6 failed.");
			}
			
			boost::asio::socket_base::reuse_address reuseAddressOption(true);
			impl_->acceptor.set_option(reuseAddressOption, ec);
			
			if (ec) {
				throw std::runtime_error("Set reuse address failed.");
			}
			
			boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v6(), port);
			
			impl_->acceptor.bind(endpoint, ec);
			
			if (ec) {
				throw std::runtime_error("bind() failed.");
			}
			
			impl_->acceptor.listen(boost::asio::ip::tcp::socket::max_connections, ec);
			
			if (ec) {
				throw std::runtime_error("listen() failed.");
			}
		}
		
		Acceptor::~Acceptor() { }
		
		// TODO: make this non-blocking!
		bool Acceptor::accept(Stream& stream) {
			bool acceptResult = false;
			
			Signal signal;
			
			impl_->acceptor.async_accept(stream.getInternal(), boost::bind(acceptCallback, &signal, &acceptResult, _1));
			
			signal.wait();
			
			return acceptResult;
		}
		
	}
	
}

