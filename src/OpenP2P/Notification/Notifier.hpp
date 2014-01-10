#ifndef OPENP2P_NOTIFICATION_NOTIFIER_HPP
#define OPENP2P_NOTIFICATION_NOTIFIER_HPP

#include <set>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Notification/Handler.hpp>

namespace OpenP2P {

	namespace Notification {
	
		class Notifier {
			public:
				Notifier();
				
				void addHandler(Handler* handler);
				
				void removeHandler(Handler* handler);
				
				bool isComplete();
				
				void notify();
				
			private:
				Mutex mutex_;
				bool isComplete_;
				std::multiset<Handler*> handlers_;
				
		};
		
	}
	
}

#endif
