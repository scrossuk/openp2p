#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <OpenP2P/Dispatcher.hpp>

namespace OpenP2P{

	namespace{

		const unsigned int DefaultConcurrency = 2;

	}

	boost::asio::io_service& GlobalIOService(){
		unsigned int concurrency = boost::thread::hardware_concurrency();
		static Dispatcher globalDispatcher(concurrency == 0 ? DefaultConcurrency : concurrency);
		return globalDispatcher.getIOService();
	}

	void Schedule(boost::function<void ()> f){
		GlobalIOService().post(f);
	}

	Dispatcher::Dispatcher(unsigned int numThreads) :
		ioService_(numThreads){

		work_ = new boost::asio::io_service::work(ioService_);

		for(unsigned int i = 0; i < numThreads; ++i){
			threadGroup_.create_thread(boost::bind(&boost::asio::io_service::run, &ioService_));
		}
	}

	Dispatcher::~Dispatcher(){
		//ioService_.stop();
		delete work_;
		threadGroup_.join_all();
	}

	boost::asio::io_service& Dispatcher::getIOService(){
		return ioService_;
	}

}
