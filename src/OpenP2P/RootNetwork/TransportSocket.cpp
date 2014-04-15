#include <assert.h>

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/TransportSocket.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		TransportSocket::TransportSocket(Socket<UDP::Endpoint, Buffer>& udpSocket)
			: udpSocket_(udpSocket) { }
			
		bool TransportSocket::isValid() const {
			return udpSocket_.isValid();
		}
		
		Event::Source TransportSocket::eventSource() const {
			return udpSocket_.eventSource();
		}
		
		bool TransportSocket::receive(Endpoint& endpoint, Buffer& buffer) {
			UDP::Endpoint udpEndpoint;
			
			// TODO: add support for other endpoints.
			if (!udpSocket_.receive(udpEndpoint, buffer)) {
				return false;
			}
			
			endpoint.kind = Endpoint::UDPIPV6;
			endpoint.udpEndpoint = udpEndpoint;
			return true;
		}
		
		bool TransportSocket::send(const Endpoint& endpoint, const Buffer& buffer) {
			assert(endpoint.kind == Endpoint::UDPIPV6);
			// TODO: add support for other endpoints.
			return udpSocket_.send(endpoint.udpEndpoint, buffer);
		}
		
	}
	
}

