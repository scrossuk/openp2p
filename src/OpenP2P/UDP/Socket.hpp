#ifndef OPENP2P_UDP_SOCKET_HPP
#define OPENP2P_UDP_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>

namespace OpenP2P{

	namespace UDP{

		class Socket: public OpenP2P::Socket<Endpoint>, boost::noncopyable{
			public:
				Socket();
				Socket(unsigned short);

				bool send(const Endpoint&, const Buffer&);

				bool receive(Endpoint&, Buffer&);

				void cancel();

				void close();

			private:
				IOService service_;
				boost::asio::ip::udp::socket internalSocket_;
				Mutex mutex_;

		};

	}

}

#endif
