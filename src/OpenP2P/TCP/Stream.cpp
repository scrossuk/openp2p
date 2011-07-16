#include <stdint.h>
#include <cstddef>
#include <cstring>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Promise.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void connectCallback(Promise<bool> connectResult, const boost::system::error_code& ec){
				connectResult.setValue(!bool(ec));
			}
			
			//void multiConnectCallback(Promise<bool> connectResult, std::vector<Endpoint> 

			void readCallback(Promise<std::size_t> readResult, const boost::system::error_code& ec, std::size_t len){
				readResult.setValue(ec ? 0 : len);
			}

		}

		Stream::Stream() : internalSocket_(service_){ }

		Future<bool> Stream::connect(const Endpoint& endpoint){
			Promise<bool> connectResult(false);

			Lock lock(mutex_);
			internalSocket_.close();
			internalSocket_.async_connect(endpoint, boost::bind(connectCallback, connectResult, _1));

			return connectResult;
		}

		bool Stream::connect(const std::vector<Endpoint>& endpointList){
			for(std::vector<Endpoint>::const_iterator i = endpointList.begin(); i != endpointList.end(); ++i){
				if(connect(*i).get()){
					return true;
				}
			}
			return false;
		}

		boost::asio::ip::tcp::socket& Stream::getInternal(){
			return internalSocket_;
		}

		Future<std::size_t> Stream::writeSome(const uint8_t * data, std::size_t length){
			Lock lock(mutex_);
			boost::system::error_code ec;
			return internalSocket_.write_some(boost::asio::buffer(data, length), ec);
		}

		Future<std::size_t> Stream::readSome(uint8_t * data, std::size_t length){
			Promise<std::size_t> readResult(0);

			Lock lock(mutex_);
			internalSocket_.async_read_some(boost::asio::buffer(data, length),
						boost::bind(readCallback, readResult, _1, _2));

			return readResult;
		}

		void Stream::close(){
			Lock lock(mutex_);
			internalSocket_.close();
		}

	}

}

