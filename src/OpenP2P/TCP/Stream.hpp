#ifndef OPENP2P_TCP_STREAM_HPP
#define OPENP2P_TCP_STREAM_HPP

#include <stdint.h>
#include <cstddef>
#include <list>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <OpenP2P/BasicStream.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	namespace TCP{

		class Stream: public BasicStream{
			public:
				Stream(boost::shared_ptr<boost::asio::ip::tcp::socket>);
				Stream(const Stream&);

				std::size_t writeSome(const uint8_t *, std::size_t, WaitHandler = Block);

				std::size_t readSome(uint8_t *, std::size_t, WaitHandler = Block);

			private:
				boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;

		};

	}

}

#endif
