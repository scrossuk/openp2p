#ifndef OPENP2P_UDP_SOCKET_HPP
#define OPENP2P_UDP_SOCKET_HPP

#include <memory>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>

namespace OpenP2P {

	namespace UDP {
	
		class Socket: public OpenP2P::Socket<UDP::Endpoint, Buffer> {
			public:
				Socket();
				Socket(uint16_t port);
				~Socket();
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool send(const UDP::Endpoint& endpoint, const Buffer& buffer);
				
				bool receive(UDP::Endpoint& endpoint, Buffer& buffer);
				
			private:
				std::unique_ptr<struct SocketImpl> impl_;
				
		};
		
	}
	
}

#endif
