#ifndef OPENP2P_ROOTNETWORK_TRANSPORTSOCKET_HPP
#define OPENP2P_ROOTNETWORK_TRANSPORTSOCKET_HPP

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>

namespace OpenP2P {

	namespace RootNetwork {
		
		/**
		 * \brief Transport Socket
		 * 
		 * Sends/receives root network packets (as raw data)
		 * through the appropriate underlying transports (e.g.
		 * UDP or TCP).
		 */
		class TransportSocket: public Socket<Endpoint, Buffer> {
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
