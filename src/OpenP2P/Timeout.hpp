#ifndef OPENP2P_TIMEOUT_HPP
#define OPENP2P_TIMEOUT_HPP

#include <boost/asio.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/IOService.hpp>

namespace OpenP2P{

	class Timeout: boost::noncopyable{
		public:
			Timeout(double secs, Cancellable& cancellable);

			~Timeout();

		private:
			IOService service_;
			boost::asio::deadline_timer internalTimer_;
	};

}

#endif
