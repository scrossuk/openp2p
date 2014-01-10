#ifndef OPENP2P_IOSERVICE_HPP
#define OPENP2P_IOSERVICE_HPP

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include <OpenP2P/Runnable.hpp>
#include <OpenP2P/Thread.hpp>

namespace OpenP2P {

	class IOServiceThread: public Runnable {
		public:
			IOServiceThread(boost::asio::io_service& io, bool wait = true);
			
			void run();
			
			void cancel();
			
		private:
			boost::asio::io_service& internalIOService_;
			boost::scoped_ptr<boost::asio::io_service::work> work_;
			bool wait_;
			
	};
	
	class IOService {
		public:
			//wait = true allows operations using the io_service to be terminated gracefully
			IOService(bool wait = true);
			
			void post(boost::function<void ()> function);
			
			operator boost::asio::io_service& ();
			
		private:
			boost::asio::io_service internalIOService_;
			IOServiceThread ioThread_;
			Thread thread_;
			
	};
	
	IOService& GetIOService();
	
}

#endif
