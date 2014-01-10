#include <boost/asio.hpp>
#include <boost/function.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Thread.hpp>

namespace OpenP2P {

	IOServiceThread::IOServiceThread(boost::asio::io_service& io, bool wait) : internalIOService_(io), work_(new boost::asio::io_service::work(internalIOService_)), wait_(wait) { }
	
	void IOServiceThread::run() {
		internalIOService_.run();
	}
	
	void IOServiceThread::cancel() {
		work_.reset();
		
		if (!wait_) {
			internalIOService_.stop();
		}
	}
	
	IOService::IOService(bool wait) : internalIOService_(1), ioThread_(internalIOService_, wait), thread_(ioThread_) { }
	
	void IOService::post(boost::function<void ()> function) {
		internalIOService_.post(function);
	}
	
	IOService::operator boost::asio::io_service& () {
		return internalIOService_;
	}
	
	IOService& GetIOService() {
		static IOService service(true);
		return service;
	}
	
}
