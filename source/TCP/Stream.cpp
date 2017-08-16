#include <stdint.h>

#include <condition_variable>
#include <mutex>
#include <vector>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Source.hpp>
#include <p2p/TCP/Endpoint.hpp>
#include <p2p/TCP/Stream.hpp>
#include <p2p/Event/IOService.hpp>

namespace p2p {

	namespace TCP {
	
		struct StreamImpl {
			boost::asio::ip::tcp::socket socket;
			std::mutex mutex;
			std::condition_variable condition;
			Event::Generator eventGenerator;
			bool isActiveConnect, isActiveRead, isActiveWrite;
			std::vector<uint8_t> readBuffer, writeBuffer;
			
			inline StreamImpl()
				: socket(Event::GetIOService()), isActiveConnect(false),
				isActiveRead(false), isActiveWrite(false) { }
			
			inline void close() {
				boost::system::error_code ec;
				socket.close(ec);
			}
		};
		
		namespace {
		
			const size_t STREAM_BUFFER_SIZE = 4096;
			
			void connectCallback(StreamImpl* impl, const boost::system::error_code& ec) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->isActiveConnect = false;
				if (ec) impl->close();
				impl->condition.notify_all();
			}
			
			void readCallback(StreamImpl* impl, const boost::system::error_code& ec, size_t transferred) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->isActiveRead = false;
				if (ec) impl->close();
				impl->condition.notify_all();
				impl->readBuffer.resize(transferred);
			}
			
			void writeCallback(StreamImpl* impl, const boost::system::error_code& ec, size_t transferred) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->isActiveWrite = false;
				if (ec) impl->close();
				impl->condition.notify_all();
				
				// TODO: replace with circular buffer.
				impl->writeBuffer.erase(impl->writeBuffer.begin(), impl->writeBuffer.begin() + transferred);
			}
			
		}
		
		Stream::Stream() : impl_(new StreamImpl()) { }
		
		Stream::~Stream() {
			impl_->close();
			std::unique_lock<std::mutex> lock(impl_->mutex);
			while (impl_->isActiveRead || impl_->isActiveWrite) {
				impl_->condition.wait(lock);
			}
		}
		
		bool Stream::connect(const TCP::Endpoint& endpoint) {
			std::unique_lock<std::mutex> lock(impl_->mutex);
			
			impl_->socket.close();
			
			boost::asio::ip::tcp::endpoint endpointImpl(IP::Address::ToImpl(endpoint.address), endpoint.port);
			impl_->socket.async_connect(endpointImpl, boost::bind(connectCallback, impl_.get(), _1));
			
			impl_->isActiveConnect = true;
			
			while (impl_->isActiveConnect) {
				impl_->condition.wait(lock);
			}
			
			return impl_->socket.is_open();
		}
		
		bool Stream::connect(const std::vector<TCP::Endpoint>& endpointList) {
			for (const auto& endpoint: endpointList) {
				if (connect(endpoint)) {
					return true;
				}
			}
			
			return false;
		}
		
		boost::asio::ip::tcp::socket& Stream::getInternal() {
			return impl_->socket;
		}
		
		bool Stream::isValid() const {
			return impl_->socket.is_open();
		}
		
		Event::Source Stream::eventSource() const {
			return impl_->eventGenerator.eventSource();
		}
		
		size_t Stream::read(uint8_t* data, size_t size) {
			if (size == 0) return 0;
			
			std::lock_guard<std::mutex> lock(impl_->mutex);
			
			if (impl_->isActiveRead) return 0;
			
			auto& readBuffer = impl_->readBuffer;
			
			const size_t readSize = std::min<size_t>(size, readBuffer.size());
			memcpy(data, readBuffer.data(), readSize);
			
			// TODO: replace with circular buffer.
			readBuffer.erase(readBuffer.begin(), readBuffer.begin() + readSize);
			
			if (readBuffer.empty()) {
				readBuffer.resize(STREAM_BUFFER_SIZE);
				
				impl_->socket.async_read_some(boost::asio::buffer(readBuffer.data(), readBuffer.size()),
					boost::bind(readCallback, impl_.get(), _1, _2));
				
				impl_->isActiveRead = true;
			}
			
			return readSize;
		}
		
		size_t Stream::write(const uint8_t* data, size_t size) {
			if (size == 0) return 0;
			
			std::lock_guard<std::mutex> lock(impl_->mutex);
			if (impl_->isActiveWrite) return 0;
			
			auto& writeBuffer = impl_->writeBuffer;
			
			assert(writeBuffer.size() <= STREAM_BUFFER_SIZE);
			const size_t writeCapacity = STREAM_BUFFER_SIZE - writeBuffer.size();
			const size_t writeSize = std::min<size_t>(size, writeCapacity);
			writeBuffer.insert(writeBuffer.end(), data, data + writeSize);
			
			if (!writeBuffer.empty()) {
				impl_->socket.async_write_some(boost::asio::buffer(writeBuffer.data(), writeBuffer.size()),
					boost::bind(writeCallback, impl_.get(), _1, _2));
				
				impl_->isActiveWrite = true;
			}
			
			return writeSize;
		}
		
	}
	
}

