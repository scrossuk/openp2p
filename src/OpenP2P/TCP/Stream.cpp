#include <stdint.h>
#include <cstddef>
#include <cstring>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/TimeoutSequence.hpp>

#include <OpenP2P/IP/Address.hpp>
#include <OpenP2P/IP/Endpoint.hpp>

#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			void connectCallback(Signal * signal, bool * connectResult, const boost::system::error_code& ec){
				*connectResult = !bool(ec);
				signal->activate();
			}
			
			void writeCallback(Signal * signal, bool * writeResult, const boost::system::error_code& ec, std::size_t transferred){
				*writeResult = !bool(ec);
				signal->activate();
			}

		}

		Stream::Stream() : internalSocket_(GetIOService()){ }

		bool Stream::connect(const IP::Endpoint& endpoint, Timeout timeout){
			bool connectResult = false;
			Signal signal;
		
			boost::asio::ip::tcp::endpoint endpointImpl(IP::Address::ToImpl(endpoint.address), endpoint.port);

			internalSocket_.close();
			internalSocket_.async_connect(endpointImpl, boost::bind(connectCallback, &signal, &connectResult, _1));
			
			if(signal.wait(timeout)){
				return connectResult;
			}else{
				internalSocket_.cancel();
				signal.wait();
				return false;
			}
		}

		bool Stream::connect(const std::vector<IP::Endpoint>& endpointList, Timeout timeout){
			TimeoutSequence sequence(timeout);
			for(std::vector<IP::Endpoint>::const_iterator i = endpointList.begin(); i != endpointList.end(); ++i){
				if(connect(*i, sequence.getTimeout())){
					return true;
				}
			}
			return false;
		}

		boost::asio::ip::tcp::socket& Stream::getInternal(){
			return internalSocket_;
		}
		
		std::size_t Stream::waitForData(Timeout timeout){
			return 0;
		}
		
		bool Stream::read(uint8_t * data, std::size_t size, Timeout timeout){
			//boost::system::error_code ec;
			//return internalSocket_.read_some(boost::asio::buffer(data, size), ec);
			
			return false;
		}
		
		std::size_t Stream::waitForSpace(Timeout){
			boost::asio::socket_base::send_buffer_size option;
			internalSocket_.get_option(option);
			return option.value();
		}
		
		bool Stream::write(const uint8_t * data, std::size_t size, Timeout timeout){
			Signal signal;
			bool writeResult = false;
				
			boost::asio::async_write(internalSocket_, boost::asio::buffer(data, size),
				boost::bind(writeCallback, &signal, &writeResult, _1, _2));
					
			if(signal.wait(timeout)){
				return writeResult;
			}else{
				internalSocket_.cancel();
				signal.wait();
				return false;
			}
		}

		void Stream::close(){
			internalSocket_.close();
		}

	}

}

