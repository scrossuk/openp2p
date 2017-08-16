#ifndef P2P_INTERNAL_IOSERVICE_HPP
#define P2P_INTERNAL_IOSERVICE_HPP

#include <thread>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include <p2p/Event/IOService.hpp>

namespace p2p {

	namespace Event {
		
		IOServiceThread::IOServiceThread(boost::asio::io_service& io, bool wait)
			: internalIOService_(io),
			work_(new boost::asio::io_service::work(internalIOService_)),
			wait_(wait) { }
		
		void IOServiceThread::run() {
			internalIOService_.run();
		}
		
		void IOServiceThread::cancel() {
			work_.reset();
			
			if (!wait_) {
				internalIOService_.stop();
			}
		}
		
		// wait = true allows operations using the io_service to be terminated gracefully.
		IOService::IOService(bool wait)
			: internalIOService_(1),
			ioThread_(internalIOService_, wait),
			thread_(&IOServiceThread::run, &ioThread_) { }
		
		IOService::~IOService() {
			ioThread_.cancel();
			thread_.join();
		}
		
		void IOService::post(boost::function<void ()> function) {
			internalIOService_.post(function);
		}
		
		IOService::operator boost::asio::io_service& () {
			return internalIOService_;
		}
		
		IOService& GetIOService() {
			static IOService ioService;
			return ioService;
		}
		
	}
	
}

#endif
