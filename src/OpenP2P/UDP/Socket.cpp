#include <boost/asio.hpp>
#include <boost/scoped_array.hpp>

#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/UDP/Endpoint.hpp>
#include <OpenP2P/UDP/Socket.hpp>

namespace OpenP2P{

	namespace UDP{

		namespace{

			void receiveCallback(Mutex& mutex, Condition& condition, std::size_t& actualLength, const boost::system::error_code& ec, std::size_t length){
				Lock lock(mutex);
				actualLength = ec ? 0 : length;
				condition.notify();
			}

		}

		Socket::Socket() : internalSocket_(service_.getInternal()){
			internalSocket_.open(boost::asio::ip::udp::v4());
		}

		Socket::Socket(unsigned short port) : internalSocket_(service_.getInternal()){
			internalSocket_.open(boost::asio::ip::udp::v4());
			boost::asio::socket_base::reuse_address option(true);
			internalSocket_.set_option(option);
			internalSocket_.bind(Endpoint(boost::asio::ip::udp::v4(), port));
		}


		bool Socket::send(const Endpoint& endpoint, const Buffer& buffer){
			if(buffer.size() > 65536){
				return false;
			}

			boost::scoped_array<uint8_t> ptr(new uint8_t[65536]);
			BufferIterator iterator(buffer);
			BinaryStream binaryStream(iterator);
			binaryStream.read(ptr.get(), buffer.size());

			{
				Lock lock(mutex_);
				internalSocket_.send_to(boost::asio::buffer(ptr.get(), buffer.size()), endpoint);
			}

			return true;
		}

		bool Socket::receive(Endpoint& endpoint, Buffer& buffer){
			Condition condition;
			Lock lock(mutex_);

			boost::scoped_array<uint8_t> ptr(new uint8_t[65536]);

			std::size_t length;

			internalSocket_.async_receive_from(boost::asio::buffer(ptr.get(), 65536), endpoint, boost::bind(receiveCallback, boost::ref(mutex_), boost::ref(condition), boost::ref(length), _1, _2));

			condition.wait(lock);

			buffer = Buffer(ptr.get(), length);

			return length > 0;
		}

		void Socket::cancel(){
			Lock lock(mutex_);
			internalSocket_.cancel();
		}

		void Socket::close(){
			Lock lock(mutex_);
			internalSocket_.close();
		}

	}

}

