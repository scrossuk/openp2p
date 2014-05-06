#ifndef P2P_UDP_SOCKET_HPP
#define P2P_UDP_SOCKET_HPP

#include <memory>

#include <p2p/Transport.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/UDP/Endpoint.hpp>

namespace p2p {

	namespace UDP {
	
		class Socket: public p2p::Socket<UDP::Endpoint, Buffer> {
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
