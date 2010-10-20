#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Dispatcher.hpp"

namespace openp2p{

	namespace Core{

		const unsigned int DefaultConcurrency = 2;

		boost::asio::io_service& GetGlobalIOService(){
			unsigned int concurrency = boost::thread::hardware_concurrency();
			static Dispatcher globalDispatcher(concurrency == 0 ? DefaultConcurrency : concurrency);
			return globalDispatcher.IOService();
		}

		Dispatcher::Dispatcher(unsigned int numThreads) :
			mIOService(numThreads){

			mWork = new boost::asio::io_service::work(mIOService);

			for(unsigned int i = 0; i < numThreads; ++i){
				mThreads.create_thread(boost::bind(&boost::asio::io_service::run, &mIOService));
			}
		}

		Dispatcher::~Dispatcher(){
			//mIOService.stop();
			delete mWork;
			mThreads.join_all();
		}

		boost::asio::io_service& Dispatcher::IOService(){
			return mIOService;
		}
	}

}
