#include <mutex>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Signal.hpp>
#include <p2p/Event/Source.hpp>

namespace p2p {

	namespace Event {
	
		struct SignalImpl {
			std::mutex mutex;
			bool isActive;
			Generator eventGenerator;
			
			inline SignalImpl() :
				isActive(false) { }
		};
		
		Signal::Signal()
			: impl_(new SignalImpl()) { }
			
		Signal::~Signal() { }
		
		Signal::Signal(Signal&& other)
			: impl_(std::move(other.impl_)) { }
		
		Signal& Signal::operator=(Signal&& other) {
			std::swap(impl_, other.impl_);
			return *this;
		}
		
		void Signal::activate() {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			impl_->isActive = true;
			impl_->eventGenerator.activate();
		}
		
		void Signal::reset() {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			impl_->isActive = false;
			impl_->eventGenerator.activate();
		}
		
		bool Signal::isActive() const {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			return impl_->isActive;
		}
		
		Source Signal::eventSource() const {
			return impl_->eventGenerator.eventSource();
		}
		
	}
	
}

