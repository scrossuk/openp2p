#ifndef P2P_TCP_STREAM_HPP
#define P2P_TCP_STREAM_HPP

#include <stdint.h>

#include <memory>

#include <boost/asio.hpp>

#include <p2p/TCP/TCP_api.hpp>
#include <p2p/Transport/Stream.hpp>
#include <p2p/Event/Source.hpp>
#include <p2p/TCP/Endpoint.hpp>

namespace p2p {

	namespace TCP {
	
		class TCP_OPENP2P_API Stream: public p2p::IOStream {
			public:
				Stream();
				~Stream();
				
				bool connect(const TCP::Endpoint& endpoint);
				
				bool connect(const std::vector<TCP::Endpoint>& endpointList);
				
				boost::asio::ip::tcp::socket& getInternal();
				
				bool isValid() const;
				
				Event::Source eventSource() const;
				
				size_t read(uint8_t* data, size_t size);
				
				size_t write(const uint8_t* data, size_t size);
				
			private:
				// Non-copyable.
				Stream(const Stream&) = delete;
				Stream& operator=(Stream) = delete;
				
				std::unique_ptr<struct StreamImpl> impl_;
				
				
		};
		
	}
	
}

#endif
