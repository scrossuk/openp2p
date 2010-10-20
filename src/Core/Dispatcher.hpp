#ifndef OPENP2P_CORE_DISPATCHER_HPP
#define OPENP2P_CORE_DISPATCHER_HPP

#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace openp2p{

	namespace Core{

		boost::asio::io_service& GetGlobalIOService();

		class Dispatcher{
			public:
				Dispatcher(unsigned int);
				~Dispatcher();

				boost::asio::io_service& IOService();

			private:
				boost::asio::io_service mIOService;

				boost::asio::io_service::work * mWork;

				boost::thread_group mThreads;

		};

	}

}

#endif
