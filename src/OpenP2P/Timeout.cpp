#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/Timeout.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	namespace{

		void TimeoutCallback(WaitCallback callback, boost::shared_ptr<boost::asio::deadline_timer> timer, const boost::system::error_code& error){
			callback();
		}

		void TimeoutCancel(boost::shared_ptr<boost::asio::deadline_timer> timer){
			timer->cancel();
		}

		CancelFunction TimeoutStart(double secs, WaitCallback callback){
			boost::shared_ptr<boost::asio::deadline_timer> timer(new boost::asio::deadline_timer(GlobalIOService()));
			timer->expires_from_now(boost::posix_time::milliseconds((unsigned int) (secs * 1000.0)));
			timer->async_wait(boost::bind(TimeoutCallback, callback, timer, _1));
			return boost::bind(TimeoutCancel, timer);
		}

	}

	WaitHandler Timeout(double secs){
		return WaitHandler(boost::bind(TimeoutStart, secs, _1));
	}

}
