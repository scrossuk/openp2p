#include <sys/time.h>

#include <cstddef>

#include <OpenP2P/Timer.hpp>

namespace OpenP2P {

	Timer::Timer() { }
	
	double Timer::getTime() {
		timeval tv;
		gettimeofday(&tv, NULL);
		return double(tv.tv_sec) + (double(tv.tv_usec) / 1000000.0);
	}
	
	double Timer::getResolution() {
		double startTime = getTime();
		double endTime = startTime;
		
		while (startTime == endTime) {
			endTime = getTime();
		}
		
		return endTime - startTime;
	}
	
}

