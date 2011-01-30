#ifndef OPENP2P_DISPATCHER_HPP
#define OPENP2P_DISPATCHER_HPP

#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

namespace OpenP2P{

	boost::asio::io_service& GlobalIOService();

	void Schedule(boost::function<void ()>);

	class Dispatcher{
		public:
			Dispatcher(unsigned int);
			~Dispatcher();

			boost::asio::io_service& getIOService();

		private:
			boost::asio::io_service ioService_;

			boost::asio::io_service::work * work_;

			boost::thread_group threadGroup_;

	};

}

#endif
