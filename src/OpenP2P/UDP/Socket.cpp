#include <condition_variable>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Generator.hpp>
#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>
#include <OpenP2P/UDP/Socket.hpp>

namespace OpenP2P {

	namespace UDP {
		
		constexpr size_t MAX_DATAGRAM_SIZE = 65536;
		
		struct SocketImpl {
			boost::asio::ip::udp::socket socket;
			std::mutex mutex;
			std::condition_variable condition;
			Event::Generator eventGenerator;
			bool isActiveReceive, isActiveSend, hasReceiveData;
			Buffer sendBuffer;
			boost::asio::ip::udp::endpoint receiveEndpoint;
			Buffer receiveBuffer;
			
			inline SocketImpl(boost::asio::io_service& pIOService)
				: socket(pIOService), isActiveReceive(false),
				isActiveSend(false), hasReceiveData(false) { }
			
			inline void close() {
				boost::system::error_code ec;
				socket.close(ec);
			}
		};
		
		namespace {
		
			void receiveCallback(SocketImpl* impl, const boost::system::error_code& ec, size_t transferred) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->isActiveReceive = false;
				impl->hasReceiveData = true;
				impl->receiveBuffer.resize(transferred);
				if (ec) impl->close();
				impl->condition.notify_all();
			}
			
			void sendCallback(SocketImpl* impl, const boost::system::error_code& ec, size_t transferred) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->isActiveSend = false;
				if (ec || transferred != impl->sendBuffer.size()) {
					impl->close();
				}
				impl->condition.notify_all();
			}
			
		}
		
		Socket::Socket() : impl_(new SocketImpl(GetIOService())) {
			boost::system::error_code ec;
			
			impl_->socket.open(boost::asio::ip::udp::v6(), ec);
			
			if (ec) {
				throw std::runtime_error("Socket open() failed.");
			}
			
			// Ensure the socket uses both IPv4 and IPv6.
			boost::asio::ip::v6_only v6OnlyOption(false);
			impl_->socket.set_option(v6OnlyOption, ec);
			
			if (ec) {
				throw std::runtime_error("Set 'v6_only' to 'false' failed.");
			}
		}
		
		Socket::Socket(uint16_t port) : impl_(new SocketImpl(GetIOService())) {
			boost::system::error_code ec;
			
			impl_->socket.open(boost::asio::ip::udp::v6(), ec);
			
			if (ec) {
				throw std::runtime_error("Socket open() failed.");
			}
			
			// Ensure the address/port are re-used.
			boost::asio::socket_base::reuse_address reuseAddressOption(true);
			impl_->socket.set_option(reuseAddressOption, ec);
			
			if (ec) {
				throw std::runtime_error("Set 'reuse_address' to 'true' failed.");
			}
			
			// Ensure the socket uses both IPv4 and IPv6.
			boost::asio::ip::v6_only v6OnlyOption(false);
			impl_->socket.set_option(v6OnlyOption, ec);
			
			if (ec) {
				throw std::runtime_error("Set 'v6_only' to 'false' failed.");
			}
			
			impl_->socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), port), ec);
			
			if (ec) {
				throw std::runtime_error("Socket bind() failed.");
			}
		}
		
		Socket::~Socket() {
			impl_->close();
			std::unique_lock<std::mutex> lock(impl_->mutex);
			while (impl_->isActiveReceive || impl_->isActiveSend) {
				impl_->condition.wait(lock);
			}
		}
		
		bool Socket::isValid() const {
			return impl_->socket.is_open();
		}
		
		Event::Source Socket::eventSource() const {
			return impl_->eventGenerator.eventSource();
		}
		
		bool Socket::send(const UDP::Endpoint& endpoint, const Buffer& buffer) {
			boost::asio::ip::udp::endpoint endpointImpl(IP::Address::ToImpl(endpoint.address), endpoint.port);
			
			std::lock_guard<std::mutex> lock(impl_->mutex);
			
			if (impl_->isActiveSend) return false;
			
			impl_->sendBuffer = buffer;
			impl_->socket.async_send_to(boost::asio::buffer(impl_->sendBuffer.data(), impl_->sendBuffer.size()),
					endpointImpl, boost::bind(sendCallback, impl_.get(), _1, _2));
			
			impl_->isActiveSend = true;
			
			return true;
		}
		
		bool Socket::receive(UDP::Endpoint& endpoint, Buffer& buffer) {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			
			const bool hasData = impl_->hasReceiveData;
			
			if (hasData) {
				endpoint.address = IP::Address::FromImpl(impl_->receiveEndpoint.address());
				endpoint.port = impl_->receiveEndpoint.port();
				buffer = std::move(impl_->receiveBuffer);
				impl_->hasReceiveData = false;
			}
			
			if (!impl_->isActiveReceive) {
				impl_->receiveBuffer.resize(MAX_DATAGRAM_SIZE);
				impl_->socket.async_receive_from(boost::asio::buffer(impl_->receiveBuffer.data(), impl_->receiveBuffer.size()),
					impl_->receiveEndpoint, boost::bind(receiveCallback, impl_.get(), _1, _2));
				impl_->isActiveReceive = true;
			}
			
			return hasData;
		}
		
	}
	
}

