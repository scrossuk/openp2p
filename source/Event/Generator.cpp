#include <mutex>
#include <set>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Source.hpp>

#include <p2p/Event/SourceImpl.hpp>

namespace p2p {

	namespace Event {
	
		struct GeneratorImpl: public SourceImpl {
			public:
				GeneratorImpl(bool initialState) : isActivated_(initialState) { }
				
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
		
		Generator::Generator(bool initialState)
			: impl_(new GeneratorImpl(initialState)) { }
			
		Generator::~Generator() { }
		
		Generator::Generator(Generator&& other)
			: impl_(std::move(other.impl_)) { }
		
		Generator& Generator::operator=(Generator&& other) {
			std::swap(impl_, other.impl_);
			return *this;
		}
		
		void Generator::activate() {
			impl_->activate();
		}
		
		Source Generator::eventSource() const {
			return Source(impl_.get());
		}
		
	}
	
}

