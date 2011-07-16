#include <iostream>
#include <string>
#include <sstream>
#include <OpenP2P.hpp>
#include <OpenP2P/Future.hpp>
#include <OpenP2P/Promise.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

using namespace OpenP2P;

class PromiseThread: public Runnable{
	public:
		PromiseThread(){ }
			
		void run(){
			promise1_.setValue("First test");
			OpenP2P::IOService ioService;
			boost::asio::deadline_timer timer(ioService);
			timer.expires_from_now(boost::posix_time::seconds(3));
			timer.wait();
			promise2_.setValue("Second test");
		}
		
		Promise<std::string> promise1_;
		Promise<std::string> promise2_;
		
	private:
		Promise<std::string> promise_;
	
};

std::size_t transform1(std::string string){
	return string.size();
}

std::string transform2(std::size_t size){
	std::ostringstream stream;
	stream << size;
	return stream.str();
}

std::string transform3(const std::string& string){
	std::ostringstream stream;
	stream << "[" << string << "]";
	return stream.str();
}

std::string joinTransform(std::pair<std::string, std::string> pair){
	return std::string("(") + pair.first + std::string("|") + pair.second + std::string(")");
}

int main(){
	PromiseThread promiseThread;
	Thread thread(promiseThread);
	
	// Compose three times.
	std::cout << "1" << std::endl;
	Future<std::string> * interFuture = new Future<std::string>(Future<std::string>(promiseThread.promise1_).compose<std::size_t>(transform1).compose<std::string>(transform2));
	
	std::cout << "2" << std::endl;
	Future<std::string> future = Future<std::string>(promiseThread.promise2_).compose<std::string>(transform3);
	std::cout << "Is promise1 ready: " << (interFuture->isReady() ? "Yes" : "No") << std::endl;
	std::cout << "Is promise2 ready: " << (future.isReady() ? "Yes" : "No") << std::endl;
	
	Future<std::string> copyFuture(*interFuture);
	
	std::cout << "3" << std::endl;
	Future<std::string> joinFuture = Future<std::string>(promiseThread.promise2_).lateJoin(*interFuture).compose<std::string>(joinTransform).lateJoin(future).compose<std::string>(joinTransform);
	
	std::cout << "4" << std::endl;
	delete interFuture;
	
	bool success = future.timedWait(2.0);
	std::cout << "Finished timed wait..." << (success ? "Done" : "Not ready yet") << std::endl;
	
	future.wait();
	
	std::cout << "Got promise value: " << future.get() << std::endl;
	
	std::cout << "Join future: " << joinFuture.get() << std::endl;
	
	std::cout << "Copy future: " << copyFuture.get() << std::endl;
	
	return 0;
}

