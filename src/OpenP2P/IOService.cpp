#include <boost/asio.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Thread.hpp>

namespace OpenP2P {

	IOServiceThread::IOServiceThread(boost::asio::io_service& io, bool wait) : internalIOService_(io), work_(new boost::asio::io_service::work(internalIOService_)), wait_(wait) { }
	
	void IOServiceThread::run() {
		internalIOService_.run();
	}
	
	void IOServiceThread::cancel() {
		work_.reset();
		
		if(!wait_) {
			internalIOService_.stop();
		}
	}
	
	IOService::IOService(bool wait) : internalIOService_(1), ioThread_(internalIOService_, wait), thread_(ioThread_) { }
	
	IOService::operator boost::asio::io_service&() {
		return internalIOService_;
	}
	
}
