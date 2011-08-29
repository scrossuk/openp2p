#include <stdint.h>
#include <cstddef>
#include <cstring>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Future.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Promise.hpp>

#include <OpenP2P/TCP/Endpoint.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void connectCallback(Promise<bool> connectResult, const boost::system::error_code& ec){
				connectResult.setValue(!bool(ec));
			}

		}

		Stream::Stream() : internalSocket_(GetIOService()){ }

		Future<bool> Stream::connect(const Endpoint& endpoint){
			Promise<bool> connectResult;

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
		
		EventHandle Stream::readEvent(){
			// TODO: return an event handle that is only active when data is available.
			return EventHandle::True;
		}
		
		std::size_t Stream::readSome(uint8_t * data, std::size_t dataSize){
			boost::system::error_code ec;
			return internalSocket_.read_some(boost::asio::buffer(data, dataSize), ec);
		}
		
		EventHandle Stream::writeEvent(){
			return EventHandle::True;
		}
		
		std::size_t Stream::writeSome(const uint8_t * data, std::size_t dataSize){
			boost::system::error_code ec;
			return internalSocket_.write_some(boost::asio::buffer(data, dataSize), ec);
		}

		void Stream::close(){
			internalSocket_.close();
		}

	}

}

