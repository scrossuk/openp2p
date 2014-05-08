#ifndef P2P_EVENT_IOSERVICE_HPP
#define P2P_EVENT_IOSERVICE_HPP

#include <thread>

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

namespace p2p {

	namespace Event {
		
		class IOServiceThread {
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
				// wait = true allows operations using the io_service to be terminated gracefully.
				IOService(bool wait = true);
				
				~IOService();
				
				void post(boost::function<void ()> function);
				
				operator boost::asio::io_service& ();
				
			private:
				boost::asio::io_service internalIOService_;
				IOServiceThread ioThread_;
				std::thread thread_;
				
		};
		
		IOService& GetIOService();
		
	}
	
}

#endif
