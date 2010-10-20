#ifndef OPENP2P_CORE_ENDPOINT_HPP
#define OPENP2P_CORE_ENDPOINT_HPP

#include <boost/asio.hpp>
#include "Types.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;
		class BufferIterator;

		class Endpoint{
			public:
				static Endpoint Unserialize(BufferIterator&);

				Endpoint(boost::asio::ip::address, unsigned short);
				Endpoint(boost::asio::ip::address_v4, unsigned short);
				Endpoint(boost::asio::ip::address_v6, unsigned short);

				boost::asio::ip::address Address() const;

				Uint16 Port() const;

				Buffer Serialize() const;

			private:
				boost::asio::ip::address mAddress;

				Uint16 mPort;

		};

	}

}

#endif
