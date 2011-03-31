#include <boost/asio.hpp>

#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Thread.hpp>

namespace OpenP2P{

	IOServiceThread::IOServiceThread(boost::asio::io_service& io) : internalIOService_(io), work_(internalIOService_){ }

	void IOServiceThread::run(){
		internalIOService_.run();
	}

	void IOServiceThread::cancel(){
		internalIOService_.stop();
	}

	IOService::IOService() : internalIOService_(1), ioThread_(internalIOService_), thread_(ioThread_){ }

	boost::asio::io_service& IOService::getInternal(){
		return internalIOService_;
	}

}
