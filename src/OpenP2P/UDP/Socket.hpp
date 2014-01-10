#ifndef OPENP2P_UDP_SOCKET_HPP
#define OPENP2P_UDP_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/IP/Endpoint.hpp>

namespace OpenP2P {

	namespace UDP {
	
		class Socket: public OpenP2P::Socket<IP::Endpoint>, boost::noncopyable {
			public:
				Socket();
				
				bool open();
				
				bool bind(unsigned short port);
				
				std::size_t send(const IP::Endpoint& endpoint, const uint8_t* data, std::size_t size, Timeout timeout);
				
				std::size_t receive(IP::Endpoint* endpoint, uint8_t* data, std::size_t size, Timeout timeout);
				
				void close();
				
			private:
				Mutex mutex_;
				boost::asio::ip::udp::socket internalSocket_;
				
		};
		
	}
	
}

#endif
