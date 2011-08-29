#ifndef OPENP2P_TCP_STREAM_HPP
#define OPENP2P_TCP_STREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <vector>

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Stream.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>

namespace OpenP2P{

	namespace TCP{

		class Stream: public OpenP2P::IOStream, boost::noncopyable{
			public:
				Stream();

				Future<bool> connect(const Endpoint& endpoint);

				bool connect(const std::vector<Endpoint>& endpointList);

				boost::asio::ip::tcp::socket& getInternal();
				
				EventHandle readEvent();
				
				std::size_t readSome(uint8_t * data, std::size_t dataSize);
				
				EventHandle writeEvent();

				std::size_t writeSome(const uint8_t * data, std::size_t dataSize);
				
				void close();

			private:
				boost::asio::ip::tcp::socket internalSocket_;
				

		};

	}

}

#endif
