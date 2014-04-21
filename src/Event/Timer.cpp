#include <mutex>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <p2p/IOService.hpp>
#include <p2p/Event/Generator.hpp>
#include <p2p/Event/Source.hpp>
#include <p2p/Event/Timer.hpp>

#include "SourceImpl.hpp"

namespace p2p {

	namespace Event {
	
		struct TimerImpl {
			std::mutex mutex;
			bool hasExpired;
			boost::asio::deadline_timer timer;
			Generator eventGenerator;
			
			inline TimerImpl(boost::asio::io_service& pIOService) :
				hasExpired(false), timer(pIOService) { }
		};
		
		namespace {
			
			void timerCallback(TimerImpl* impl, const boost::system::error_code&) {
				std::lock_guard<std::mutex> lock(impl->mutex);
				impl->eventGenerator.activate();
				impl->hasExpired = true;
			}
			
		}
		
		Timer::Timer()
			: impl_(new TimerImpl(GetIOService())) { }
			
		Timer::~Timer() { }
		
		Timer::Timer(Timer&& other)
			: impl_(std::move(other.impl_)) { }
		
		Timer& Timer::operator=(Timer&& other) {
			std::swap(impl_, other.impl_);
			return *this;
		}
		
		void Timer::setMilliseconds(double milliseconds) {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			impl_->timer.expires_from_now(boost::posix_time::milliseconds(milliseconds));
		}
		
		void Timer::schedule() {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			impl_->hasExpired = false;
			impl_->timer.async_wait(boost::bind(timerCallback, impl_.get(), _1));
		}
		
		bool Timer::hasExpired() const {
			std::lock_guard<std::mutex> lock(impl_->mutex);
			return impl_->hasExpired;
		}
		
		Source Timer::eventSource() const {
			return impl_->eventGenerator.eventSource();
		}
		
	}
	
}

