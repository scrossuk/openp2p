#include <iostream>
#include "../../src/Core.hpp"
#include <boost/thread.hpp>

using namespace openp2p::Core;

unsigned int counter = 0;

void f(){
	std::cout << "Timer " << counter++ << std::endl;
}

const unsigned int NUM_TIMERS = 10000;

int main(){
	Timer timer;
	
	for(unsigned int i = 0; i < NUM_TIMERS; ++i){
		timer.Add(double(i % 10) + 0.5, f);
	}
	
	boost::this_thread::sleep(boost::posix_time::seconds(11));
	
	if(counter == NUM_TIMERS){
		std::cout << "All timers activated" << std::endl;
	}else{
		std::cout << "Error - all timers were not activated" << std::endl;
	}

	return 0;
}

