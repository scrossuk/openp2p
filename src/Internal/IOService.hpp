#ifndef P2P_INTERNAL_IOSERVICE_HPP
#define P2P_INTERNAL_IOSERVICE_HPP

#include <thread>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

namespace p2p {

	namespace {
		
		class IOServiceThread {
			public:
				IOServiceThread(boost::asio::io_service& io, bool wait = true)
					: internalIOService_(io),
					work_(new boost::asio::io_service::work(internalIOService_)),
					wait_(wait) { }
				
				void run() {
					internalIOService_.run();
				}
				
				void cancel() {
					work_.reset();
					
					if (!wait_) {
						internalIOService_.stop();
					}
				}
				
			private:
				boost::asio::io_service& internalIOService_;
				boost::scoped_ptr<boost::asio::io_service::work> work_;
				bool wait_;
				
		};
		
		class IOService {
			public:
				// wait = true allows operations using the io_service to be terminated gracefully.
				IOService(bool wait = true)
					: internalIOService_(1),
					ioThread_(internalIOService_, wait),
					thread_(&IOServiceThread::run, &ioThread_) { }
				
				~IOService() {
					ioThread_.cancel();
					thread_.join();
				}
				
				void post(boost::function<void ()> function) {
					internalIOService_.post(function);
				}
				
				operator boost::asio::io_service& () {
					return internalIOService_;
				}
				
			private:
				boost::asio::io_service internalIOService_;
				IOServiceThread ioThread_;
				std::thread thread_;
				
		};
		
	}
	
}

#endif
