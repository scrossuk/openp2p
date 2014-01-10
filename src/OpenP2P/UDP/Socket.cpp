#include <boost/asio.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Socket.hpp>
#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/TimeoutSequence.hpp>

#include <OpenP2P/IP/Endpoint.hpp>

#include <OpenP2P/UDP/Socket.hpp>

namespace OpenP2P {

	namespace UDP {
	
		namespace {
		
			void receiveCallback(Mutex* mutex, Condition* condition, boost::system::error_code* receiveResult,
								 std::size_t* receiveLength, const boost::system::error_code& ec, std::size_t transferred) {
				Lock lock(*mutex);
				*receiveResult = ec;
				*receiveLength = transferred;
				condition->notifyOne();
			}
			
			void sendCallback(Mutex* mutex, Condition* condition, boost::system::error_code* sendResult,
							  std::size_t* sendLength, const boost::system::error_code& ec, std::size_t transferred) {
				Lock lock(*mutex);
				*sendResult = ec;
				*sendLength = transferred;
				condition->notifyOne();
			}
			
		}
		
		Socket::Socket() : internalSocket_(GetIOService()) { }
		
		bool Socket::open() {
			Lock lock(mutex_);
			boost::system::error_code ec;
			
			// This ensures the socket uses both IPv4 and IPv6.
			boost::asio::ip::v6_only v6OnlyOption(false);
			internalSocket_.set_option(v6OnlyOption, ec);
			
			if (ec) {
				return false;
			}
			
			internalSocket_.open(boost::asio::ip::udp::v6(), ec);
			
			return !bool(ec);
		}
		
		bool Socket::bind(unsigned short port) {
			Lock lock(mutex_);
			
			boost::system::error_code ec;
			boost::asio::socket_base::reuse_address reuseAddressOption(true);
			internalSocket_.set_option(reuseAddressOption, ec);
			
			if (ec) {
				return false;
			}
			
			internalSocket_.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), port), ec);
			
			return !bool(ec);
		}
		
		std::size_t Socket::send(const IP::Endpoint& endpoint, const uint8_t* data, std::size_t size, Timeout timeout) {
			boost::asio::ip::udp::endpoint endpointImpl(IP::Address::ToImpl(endpoint.address), endpoint.port);
			
			boost::system::error_code sendResult;
			std::size_t sendLength;
			
			Condition condition;
			Lock lock(mutex_);
			
			internalSocket_.async_send_to(boost::asio::buffer(data, size), endpointImpl,
										  boost::bind(sendCallback, &mutex_, &condition, &sendResult, &sendLength, _1, _2));
										  
			TimeoutSequence sequence(timeout);
			
			while (condition.wait(lock, sequence.getTimeout())) {
				if (sendResult) {
					if (sendResult == boost::asio::error::operation_aborted) {
						// Other operations being cancelled can cause this operation
						// to be cancelled - need to restart it.
						internalSocket_.async_send_to(boost::asio::buffer(data, size), endpointImpl,
													  boost::bind(sendCallback, &mutex_, &condition, &sendResult, &sendLength, _1, _2));
					} else {
						return 0;
					}
				} else {
					return sendLength;
				}
			}
			
			internalSocket_.cancel();
			condition.wait(lock);
			return 0;
		}
		
		std::size_t Socket::receive(IP::Endpoint* endpoint, uint8_t* data, std::size_t size, Timeout timeout) {
			boost::asio::ip::udp::endpoint endpointImpl;
			
			boost::system::error_code receiveResult;
			std::size_t receiveLength;
			
			Condition condition;
			Lock lock(mutex_);
			
			internalSocket_.async_receive_from(boost::asio::buffer(data, size), endpointImpl,
											   boost::bind(receiveCallback, &mutex_, &condition, &receiveResult, &receiveLength, _1, _2));
											   
			TimeoutSequence sequence(timeout);
			
			while (condition.wait(lock, sequence.getTimeout())) {
				if (receiveResult) {
					if (receiveResult == boost::asio::error::operation_aborted) {
						// Other operations being cancelled can cause this operation
						// to be cancelled - need to restart it.
						internalSocket_.async_receive_from(boost::asio::buffer(data, size), endpointImpl,
														   boost::bind(receiveCallback, &mutex_, &condition, &receiveResult, &receiveLength, _1, _2));
					} else {
						return 0;
					}
				} else {
					endpoint->address = IP::Address::FromImpl(endpointImpl.address());
					endpoint->port = endpointImpl.port();
					
					return receiveLength;
				}
			}
			
			internalSocket_.cancel();
			condition.wait(lock);
			return 0;
		}
		
		void Socket::close() {
			Lock lock(mutex_);
			internalSocket_.close();
		}
		
	}
	
}

