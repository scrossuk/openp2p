#ifndef OPENP2P_TCP_STREAM_HPP
#define OPENP2P_TCP_STREAM_HPP

#include <stdint.h>

#include <memory>

#include <boost/asio.hpp>

#include <OpenP2P/Stream.hpp>

#include <OpenP2P/Event/Source.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P {

	namespace TCP {
	
		class Stream: public OpenP2P::IOStream {
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
