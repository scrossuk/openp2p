#ifndef OPENP2P_UDP_SOCKET_HPP
#define OPENP2P_UDP_SOCKET_HPP

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <OpenP2P/Block.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/Socket.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/UDP/Endpoint.hpp>

namespace OpenP2P{

	namespace UDP{

		class Socket: public OpenP2P::Socket<Endpoint>{
			public:
				Socket();
				Socket(unsigned short);

				~Socket();

				bool send(const Endpoint&, const Buffer&, WaitHandler = Block);

				bool receive(Endpoint&, Buffer&, WaitHandler = Block);

			private:
				void doReceive();

				void receiveCallback(const boost::system::error_code&, std::size_t);

				Signal dataReady_;
				Endpoint receiveEndpoint_;
				Buffer receiveBuffer_;
				uint8_t receiveData_[65536];

				boost::asio::ip::udp::socket socket_;
				bool terminating_;
				boost::mutex mutex_, receiveMutex_;

		};

	}

}

#endif
