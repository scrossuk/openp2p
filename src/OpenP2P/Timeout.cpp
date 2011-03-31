#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/Timeout.hpp>

namespace OpenP2P{

	namespace{

		void timeoutCallback(Cancellable& cancellable, const boost::system::error_code& ec){
			if(!ec){
				cancellable.cancel();
			}
		}

	}

	Timeout::Timeout(double secs, Cancellable& cancellable) : internalTimer_(service_.getInternal()){
		internalTimer_.expires_from_now(boost::posix_time::milliseconds(secs * 1000.0));
		internalTimer_.async_wait(boost::bind(timeoutCallback, boost::ref(cancellable), _1));
	}

	Timeout::~Timeout(){
		internalTimer_.cancel();
	}

}

