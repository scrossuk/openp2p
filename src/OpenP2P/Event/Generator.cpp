#include <mutex>
#include <set>

#include <OpenP2P/Event/Generator.hpp>
#include <OpenP2P/Event/Source.hpp>

#include "SourceImpl.hpp"

namespace OpenP2P {

	namespace Event {
	
		struct GeneratorImpl: public SourceImpl {
			public:
				GeneratorImpl() : isActivated_(false) { }
				
				void activate() {
					std::lock_guard<std::mutex> lock(mutex_);
					if (isActivated_) return;
					
					if (receivers_.empty()) {
						// Event is pending.
						isActivated_ = true;
						return;
					}
					
					// Deliver event immediately.
					(*(receivers_.begin()))->notify();
				}
				
				void addReceiver(Receiver& receiver) {
					std::lock_guard<std::mutex> lock(mutex_);
					receivers_.insert(&receiver);
					
					if (isActivated_) {
						// Deliver pending event.
						receiver.notify();
						
						// Reset.
						isActivated_ = false;
					}
				}
				
				void removeReceiver(Receiver& receiver) {
					std::lock_guard<std::mutex> lock(mutex_);
					receivers_.erase(&receiver);
				}
				
			private:
				std::mutex mutex_;
				bool isActivated_;
				std::set<Receiver*> receivers_;
				
		};
		
		Generator::Generator()
			: impl_(new GeneratorImpl()) { }
			
		Generator::~Generator() { }
		
		void Generator::activate() {
			impl_->activate();
		}
		
		Source Generator::eventSource() const {
			return Source(impl_.get());
		}
		
	}
	
}

