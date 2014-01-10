#include <memory>
#include <stdexcept>

#include <boost/asio.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Signal.hpp>

#include <OpenP2P/TCP/Acceptor.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P {

	namespace TCP {
	
		namespace {
		
			void acceptCallback(Signal* signal, bool* acceptResult, const boost::system::error_code& ec) {
				*acceptResult = !bool(ec);
				signal->activate();
			}
			
		}
		
		struct AcceptorImpl {
			boost::asio::ip::tcp::acceptor acceptor;
			
			inline AcceptorImpl(boost::asio::io_service& pIOService)
				: acceptor(pIOService) { }
		};
		
		Acceptor::Acceptor(uint16_t port) : impl_(new AcceptorImpl(GetIOService())) {
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
		
		bool Acceptor::accept(Stream& stream) {
			bool acceptResult = false;
			
			Signal signal;
			
			impl_->acceptor.async_accept(stream.getInternal(), boost::bind(acceptCallback, &signal, &acceptResult, _1));
			
			if (signal.wait()) {
				return acceptResult;
			} else {
				impl_->acceptor.cancel();
				signal.wait();
				return false;
			}
		}
		
	}
	
}

