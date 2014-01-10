#include <iostream>
#include <OpenP2P.hpp>
#include <boost/asio.hpp>

void callback(const boost::system::error_code&) {
	std::cout << "Async Timer" << std::endl;
}

int main() {
	OpenP2P::IOService ioService;
	boost::asio::deadline_timer asyncTimer(ioService);
	boost::asio::deadline_timer timer(ioService);
	
	asyncTimer.expires_from_now(boost::posix_time::seconds(5));
	timer.expires_from_now(boost::posix_time::seconds(10));
	
	asyncTimer.async_wait(callback);
	timer.wait();
	
	std::cout << "Sync Timer" << std::endl;
	
	//Will terminate after 5 seconds, calling the callback, as a result of the automatic destruction of the IOService
	
	return 0;
}

