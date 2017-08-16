#ifndef P2P_ROOTNETWORK_TRANSPORTSOCKET_HPP
#define P2P_ROOTNETWORK_TRANSPORTSOCKET_HPP

#include <p2p/Root/Root_api.hpp>
#include <p2p/Transport.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>

#include <p2p/UDP/Endpoint.hpp>

#include <p2p/Root/Endpoint.hpp>

namespace p2p {

	namespace Root {
		
		/**
		 * \brief Transport Socket
		 * 
		 * Sends/receives root network packets (as raw data)
		 * through the appropriate underlying transports (e.g.
		 * UDP or TCP).
		 */
		class ROOT_OPENP2P_API TransportSocket: public Socket<Endpoint, Buffer> {
			public:
				TransportSocket(Socket<UDP::Endpoint, Buffer>& udpSocket);
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				bool receive(Endpoint& endpoint, Buffer& buffer);
				
				bool send(const Endpoint& endpoint, const Buffer& buffer);
				
			private:
				TransportSocket(const TransportSocket&) = delete;
				TransportSocket& operator=(TransportSocket) = delete;
				
				Socket<UDP::Endpoint, Buffer>& udpSocket_;
				
		};
		
	}
	
}

#endif
