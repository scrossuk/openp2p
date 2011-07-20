#include <set>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Notifier.hpp>

namespace OpenP2P {

	namespace Notification {
	
		Notifier::Notifier()
			: isComplete_(false){ }
			
		void Notifier::addHandler(Handler * handler) {
			Lock lock(mutex_);
			
			if(isComplete_) {
				handler->notify();
			} else {
				handlers_.insert(handler);
			}
		}
		
		void Notifier::removeHandler(Handler * handler) {
			Lock lock(mutex_);
			
			// If it is complete, the handler must already have been notified.
			if(!isComplete_) {
				std::multiset<Handler *>::iterator it;
				it = handlers_.find(handler);
				
				if(it != handlers_.end()) {
					handlers_.erase(it);
				}
			}
		}
		
		bool Notifier::isComplete() {
			Lock lock(mutex_);
			return isComplete_;
		}
		
		void Notifier::notify() {
			Lock lock(mutex_);
			
			if(!isComplete_) {
				std::multiset<Handler *>::iterator it;
				
				for(it = handlers_.begin(); it != handlers_.end(); ++it) {
					(*it)->notify();
				}
				
				handlers_.clear();
				
				isComplete_ = true;
			}
		}
		
	}
	
}

