#ifndef OPENP2P_IOSERVICE_HPP
#define OPENP2P_IOSERVICE_HPP

#include <boost/asio.hpp>

#include <OpenP2P/Runnable.hpp>
#include <OpenP2P/Thread.hpp>

namespace OpenP2P{

	class IOServiceThread: public Runnable{
		public:
			IOServiceThread(boost::asio::io_service& io);

			void run();

			void cancel();

		private:
			boost::asio::io_service& internalIOService_;
			boost::asio::io_service::work work_;

	};

	class IOService{
		public:
			IOService();

			boost::asio::io_service& getInternal();

		private:
			boost::asio::io_service internalIOService_;
			IOServiceThread ioThread_;
			Thread thread_;

	};

}

#endif
