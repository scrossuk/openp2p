#include <stdio.h>

#include <p2p/Event/Timer.hpp>
#include <p2p/Event/Wait.hpp>

int main() {
	p2p::Event::Timer timer5Secs, timer10Secs;
	timer5Secs.setMilliseconds(5000);
	timer10Secs.setMilliseconds(10000);
	
	timer5Secs.schedule();
	timer10Secs.schedule();
	
	while (true) {
		const bool wasExpired5Secs = timer5Secs.hasExpired(), wasExpired10Secs = timer10Secs.hasExpired();
		if (wasExpired5Secs && wasExpired10Secs) break;
		
		p2p::Event::Wait({ timer5Secs.eventSource(), timer10Secs.eventSource() });
		
		if (!wasExpired5Secs && timer5Secs.hasExpired()) {
			printf("5 second timer has expired!\n");
		}
		
		if (!wasExpired10Secs && timer10Secs.hasExpired()) {
			printf("10 second timer has expired!\n");
		}
	}
	
	return 0;
}

