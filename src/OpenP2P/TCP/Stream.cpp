#include <stdint.h>
#include <cstddef>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Wait.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>
#include <OpenP2P/TCP/Stream.hpp>

namespace OpenP2P{

	namespace TCP{

		namespace{

			class WriteOperation: public WaitObject, boost::noncopyable{
				public:
					WriteOperation(boost::shared_ptr<boost::asio::ip::tcp::socket> socket, const uint8_t * data, std::size_t length) :
						socket_(socket), data_(data), length_(length), size_(0){ }

					std::size_t size() const{
						return size_;
					}

					CancelFunction asyncWait(WaitCallback callback){
						socket_->async_write_some(boost::asio::buffer(data_, length_),
							boost::bind(&WriteOperation::callback, this, callback, _1, _2));
						return boost::bind(&WriteOperation::cancel, this);
					}

					void callback(WaitCallback callback, const boost::system::error_code& ec, std::size_t len){
						if(ec){
							size_ = 0;
						}else{
							size_ = len;
						}
						callback();
					}

					void cancel(){
						socket_->cancel();
					}

				private:
					boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
					const uint8_t * data_;
					std::size_t length_;
					std::size_t size_;

			};

			class ReadOperation: public WaitObject, boost::noncopyable{
				public:
					ReadOperation(boost::shared_ptr<boost::asio::ip::tcp::socket> socket, uint8_t * data, std::size_t length) :
						socket_(socket), data_(data), length_(length), size_(0){ }

					std::size_t size() const{
						return size_;
					}

					CancelFunction asyncWait(WaitCallback callback){
						socket_->async_read_some(boost::asio::buffer(data_, length_),
							boost::bind(&ReadOperation::callback, this, callback, _1, _2));
						return boost::bind(&ReadOperation::cancel, this);
					}

					void callback(WaitCallback callback, const boost::system::error_code& ec, std::size_t len){
						if(ec){
							size_ = 0;
						}else{
							size_ = len;
						}
						callback();
					}

					void cancel(){
						socket_->cancel();
					}

				private:
					boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
					uint8_t * data_;
					std::size_t length_;
					std::size_t size_;

			};

		}

		Stream::Stream(boost::shared_ptr<boost::asio::ip::tcp::socket> socket) : socket_(socket){ }

		Stream::Stream(const Stream& stream) : socket_(stream.socket_){ }

		std::size_t Stream::writeSome(const uint8_t * data, std::size_t length, WaitHandler handler){
			WriteOperation writeOp(socket_, data, length);

			Wait(writeOp, handler);

			return writeOp.size();
		}

		std::size_t Stream::readSome(uint8_t * data, std::size_t length, WaitHandler handler){
			ReadOperation readOp(socket_, data, length);

			Wait(readOp, handler);

			return readOp.size();
		}

	}

}

