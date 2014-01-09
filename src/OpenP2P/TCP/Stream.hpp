#ifndef OPENP2P_TCP_STREAM_HPP
#define OPENP2P_TCP_STREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <vector>

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Stream.hpp>
#include <OpenP2P/Timeout.hpp>

#include <OpenP2P/IP/Endpoint.hpp>

namespace OpenP2P{

	namespace TCP{

		class Stream: public OpenP2P::IOStream, boost::noncopyable{
			public:
				Stream();

				bool connect(const IP::Endpoint& endpoint, Timeout timeout = Timeout::Infinite());

				bool connect(const std::vector<IP::Endpoint>& endpointList, Timeout timeout = Timeout::Infinite());

				boost::asio::ip::tcp::socket& getInternal();
				
				std::size_t waitForData(Timeout timeout);
				
				bool read(uint8_t * data, std::size_t size, Timeout timeout);
				
				std::size_t waitForSpace(Timeout timeout);

				bool write(const uint8_t * data, std::size_t size, Timeout timeout);
				
				void close();

			private:
				boost::asio::ip::tcp::socket internalSocket_;
				

		};

	}

}

#endif
