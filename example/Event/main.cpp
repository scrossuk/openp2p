#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

#include <p2p/Concurrency.hpp>
#include <p2p/Event.hpp>

using namespace p2p;

#define COLOR_RESET  "\x1B[0m"
#define COLOR_RED  "\x1B[31m"
#define COLOR_GREEN  "\x1B[32m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_BLUE  "\x1B[34m"
#define COLOR_MAGENTA  "\x1B[35m"
#define COLOR_CYAN  "\x1B[36m"
#define COLOR_WHITE  "\x1B[37m"

class EventThreadRunnable: public Runnable {
	public:
		EventThreadRunnable() { }
		virtual ~EventThreadRunnable() { }
		
		struct SetToFalse {
			bool& v;
			SetToFalse(bool& pV) : v(pV) { }
			~SetToFalse() {
				v = false;
			}
		};
		
		void activate() {
			printf(COLOR_BLUE "Activate!\n" COLOR_RESET);
			
			eventGenerator_.activate();
		}
		
		void sleepFor(size_t secs) {
			while (secs > 0) {
				printf(COLOR_BLUE "%llu secs...\n" COLOR_RESET, (unsigned long long) secs);
				sleep(1);
				secs -= 1;
			}
		}
		
		void run() {
			SetToFalse setToFalse(isRunning_);
			
			activate();
			
			sleepFor(2);
			
			activate();
			
			sleepFor(5);
			
			activate();
			
			sleepFor(2);
			
			activate();
		}
		
		Event::Source eventSource() {
			return eventGenerator_.eventSource();
		}
		
		void cancel() { }
		
		bool isRunning() const {
			return isRunning_;
		}
		
	private:
		bool isRunning_;
		Event::Generator eventGenerator_;
		
};

int main() {
	EventThreadRunnable eventThreadRunnable;
	
	Thread thread(eventThreadRunnable);
	
	printf(COLOR_GREEN "--- Started.\n" COLOR_RESET);
	
	while (eventThreadRunnable.isRunning()) {
		printf(COLOR_GREEN "--- Waiting...\n" COLOR_RESET);
		Event::Wait(std::vector<Event::Source>(1, eventThreadRunnable.eventSource()));
		printf(COLOR_GREEN "--- Event received.\n" COLOR_RESET);
		sleep(3);
	}
	
	printf(COLOR_GREEN "--- Done.\n" COLOR_RESET);
	
	return 0;
}

