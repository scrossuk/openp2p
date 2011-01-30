#include <iostream>
#include <OpenP2P.hpp>
#include <boost/thread.hpp>

const unsigned int NUM_TIMERS = 1000;

unsigned int counter = 0;
boost::mutex mutex;
OpenP2P::Signal endSignal;

void f(){
	boost::lock_guard<boost::mutex> lock(mutex);
	counter++;
	if((counter % (NUM_TIMERS / 100)) == 0){
		std::cout << "Done " << counter << std::endl;
	}
	
	if(counter == NUM_TIMERS){
		std::cout << "Activate end signal" << std::endl;
		//endSignal.activate();
	}
}

int main(){
	{
		boost::lock_guard<boost::mutex> lock(mutex);
		for(unsigned int i = 0; i < NUM_TIMERS; ++i){
			OpenP2P::Timeout(double(i % 10) + 0.5)(f);
		}
	
		std::cout << "Start" << std::endl;
	}
	
	for(unsigned int i = 0; i < 10; i++){
		{
			boost::lock_guard<boost::mutex> lock(mutex);
			std::cout << i << std::endl;
		}
		OpenP2P::Wait(OpenP2P::Timeout(1.0));
	}
	
	{
		boost::lock_guard<boost::mutex> lock(mutex);
		std::cout << "Wait for finish" << std::endl;
	}
	
	OpenP2P::Wait(OpenP2P::Timeout(5.0), boost::ref(endSignal));
	
	if(counter == NUM_TIMERS){
		std::cout << "All timers activated" << std::endl;
	}else{
		std::cout << "Error - all timers were not activated" << std::endl;
	}

	return 0;
}

