#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/Socket.hpp>
#include <OpenP2P/Wait.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/UDP/Endpoint.hpp>
#include <OpenP2P/UDP/Socket.hpp>

namespace OpenP2P{

	namespace UDP{

		Socket::Socket() : socket_(GlobalIOService()), terminating_(false){
			socket_.open(boost::asio::ip::udp::v4());

			doReceive();
		}

		Socket::Socket(unsigned short port) : socket_(GlobalIOService()), terminating_(false){
			socket_.open(boost::asio::ip::udp::v4());
			boost::asio::socket_base::reuse_address option(true);
			socket_.set_option(option);
			socket_.bind(Endpoint(boost::asio::ip::udp::v4(), port));

			doReceive();
		}

		Socket::~Socket(){
			{
				boost::lock_guard<boost::mutex> lock(mutex_);
				terminating_ = true;
				socket_.close();
			}
			Wait(dataReady_);
		}

		bool Socket::send(const Endpoint& endpoint, const Buffer& buffer, WaitHandler handler){
			if(buffer.size() > 65536){
				return false;
			}

			uint8_t data[65536];
			BufferIterator iterator(buffer);
			iterator.read(data, buffer.size());

			{
				boost::lock_guard<boost::mutex> lock(mutex_);
				socket_.send_to(boost::asio::buffer(data, buffer.size()), endpoint);
			}

			return true;
		}

		bool Socket::receive(Endpoint& endpoint, Buffer& buffer, WaitHandler handler){
			boost::lock_guard<boost::mutex> receiveLock(receiveMutex_);

			bool s = (Wait(dataReady_, handler) == 0);

			if(s){
				boost::lock_guard<boost::mutex> lock(mutex_);
				endpoint = receiveEndpoint_;
				buffer = receiveBuffer_;
				dataReady_.reset();
				doReceive();
			}

			return s;
		}

		void Socket::doReceive(){
			socket_.async_receive_from(boost::asio::buffer(receiveData_, 65536), receiveEndpoint_, boost::bind(&Socket::receiveCallback, this, _1, _2));
		}

		void Socket::receiveCallback(const boost::system::error_code& error, std::size_t length){
			boost::lock_guard<boost::mutex> lock(mutex_);
			if(!error){
				receiveBuffer_ = Buffer(receiveData_, length);
				dataReady_.activate();
			}else if(terminating_){
				dataReady_.activate();
			}else{
				doReceive();
			}
		}

	}

}

