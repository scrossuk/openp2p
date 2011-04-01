#include <stdint.h>
#include <cstddef>
#include <cstring>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void connectCallback(Mutex& mutex, Condition& condition, bool& success, const boost::system::error_code& ec){
				Lock lock(mutex);
				success = !bool(ec);
				condition.notify();
			}

			void readCallback(Mutex& mutex, Condition& condition, std::size_t& actualReadLength, const boost::system::error_code& ec, std::size_t len){
				Lock lock(mutex);
				actualReadLength = ec ? 0 : len;
				condition.notify();
			}

		}

		Stream::Stream() : internalSocket_(service_){ }

		bool Stream::connect(const Endpoint& endpoint){
			Condition condition;
			Lock lock(mutex_);
			internalSocket_.close();

			bool success = false;

			internalSocket_.async_connect(endpoint, boost::bind(connectCallback, boost::ref(mutex_), boost::ref(condition), boost::ref(success), _1));

			condition.wait(lock);

			return success;
		}

		bool Stream::connect(const std::vector<Endpoint>& endpointList){
			for(std::vector<Endpoint>::const_iterator i = endpointList.begin(); i != endpointList.end(); ++i){
				if(connect(*i)){
					return true;
				}
			}
			return false;
		}

		boost::asio::ip::tcp::socket& Stream::getInternal(){
			return internalSocket_;
		}

		std::size_t Stream::writeSome(const uint8_t * data, std::size_t length){
			Lock lock(mutex_);
			boost::system::error_code ec;
			return internalSocket_.write_some(boost::asio::buffer(data, length), ec);
		}

		std::size_t Stream::readSome(uint8_t * data, std::size_t length){
			Condition condition;
			Lock lock(mutex_);

			std::size_t actualReadLength = 0;

			internalSocket_.async_read_some(boost::asio::buffer(data, length),
						boost::bind(readCallback, boost::ref(mutex_), boost::ref(condition), boost::ref(actualReadLength), _1, _2));

			condition.wait(lock);

			return actualReadLength;
		}

		void Stream::cancel(){
			Lock lock(mutex_);
			internalSocket_.cancel();
		}

		void Stream::close(){
			Lock lock(mutex_);
			internalSocket_.close();
		}

	}

}

