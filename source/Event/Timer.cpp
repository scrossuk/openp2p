#include <condition_variable>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Source.hpp>
#include <p2p/Event/Timer.hpp>
#include <p2p/Event/IOService.hpp>
#include <p2p/Event/SourceImpl.hpp>

namespace p2p {

	namespace Event {
	
		struct TimerImpl {
			std::mutex mutex;
			std::condition_variable condition;
			bool isRunning;
			double milliseconds;
			boost::asio::deadline_timer timer;
			Generator eventGenerator;
			
			inline TimerImpl() :
				isRunning(false), milliseconds(0.0),
				timer(GetIOService()) { }
		};
		
		namespace {
			
			void timerCallback(TimerImpl* impl, const boost::system::error_code&) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->isRunning = false;
				impl->condition.notify_all();
			}
			
		}
		
		Timer::Timer()
			: impl_(new TimerImpl()) { }
			
		Timer::~Timer() {
			std::unique_lock<std::mutex> lock(impl_->mutex);
			(void) impl_->timer.cancel();
			while (impl_->isRunning) {
				impl_->condition.wait(lock);
			}
		}
		
		Timer::Timer(Timer&& other)
			: impl_(std::move(other.impl_)) { }
		
		Timer& Timer::operator=(Timer&& other) {
			std::swap(impl_, other.impl_);
			return *this;
		}
		
		void Timer::setMilliseconds(double milliseconds) {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			impl_->milliseconds = milliseconds;
		}
		
		void Timer::schedule() {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			impl_->isRunning = true;
			impl_->timer.expires_from_now(boost::posix_time::milliseconds(impl_->milliseconds));
			impl_->timer.async_wait(boost::bind(timerCallback, impl_.get(), _1));
		}
		
		bool Timer::hasExpired() const {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			return !impl_->isRunning;
		}
		
		Source Timer::eventSource() const {
			return impl_->eventGenerator.eventSource();
		}
		
	}
	
}

