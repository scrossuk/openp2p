#include <condition_variable>
#include <mutex>

#include <p2p/Concurrency/Signal.hpp>

namespace p2p {

	Signal::Signal() : isActivated_(false) { }
	
	void Signal::activate() {
		std::lock_guard<std::mutex> lock(mutex_);
		condition_.notify_all();
		isActivated_ = true;
	}
	
	void Signal::reset() {
		std::lock_guard<std::mutex> lock(mutex_);
		isActivated_ = false;
	}
	
	bool Signal::isActivated() const {
		std::lock_guard<std::mutex> lock(mutex_);
		return isActivated_;
	}
	
	void Signal::wait() {
		std::unique_lock<std::mutex> lock(mutex_);
		
		while (!isActivated_) {
			condition_.wait(lock);
		}
	}
	
	void Signal::cancel() {
		std::lock_guard<std::mutex> lock(mutex_);
		condition_.notify_all();
	}
	
}

