#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <vector>

#include <OpenP2P/Event/Source.hpp>
#include <OpenP2P/Event/Wait.hpp>

#include "SourceImpl.hpp"

namespace OpenP2P {

	namespace Event {
	
		namespace {
		
			class WaitReceiver: public Receiver {
				public:
					WaitReceiver() : isNotified_(false) { }
						
					void notify() {
						std::lock_guard<std::mutex> lock(mutex_);
						isNotified_ = true;
						condition_.notify_one();
					}
					
					void wait() {
						std::unique_lock<std::mutex> lock(mutex_);
						if (isNotified_) return;
						condition_.wait(lock);
					}
					
				private:
					// Non-copyable.
					WaitReceiver(const WaitReceiver&) = delete;
					WaitReceiver& operator=(WaitReceiver) = delete;
					
					std::mutex mutex_;
					std::condition_variable condition_;
					bool isNotified_;
					
			};
			
			class AddReceiver {
				public:
					AddReceiver(Source source, Receiver& receiver)
						: source_(source), receiver_(receiver) {
						if (source_.impl() != nullptr) source_.impl()->addReceiver(receiver_);
					}
					
					~AddReceiver() {
						if (source_.impl() != nullptr) source_.impl()->removeReceiver(receiver_);
					}
					
				private:
					// Non-copyable.
					AddReceiver(const AddReceiver&) = delete;
					AddReceiver& operator=(AddReceiver) = delete;
					
					Source source_;
					Receiver& receiver_;
					
			};
			
			void WaitRecurse(WaitReceiver& receiver, const std::vector<Source>& sources, size_t position) {
				if (position < sources.size()) {
					AddReceiver addReceiver(sources.at(position), receiver);
					WaitRecurse(receiver, sources, position + 1);
				} else {
					receiver.wait();
				}
			}
			
		}
		
		void Wait(const std::vector<Source>& sources) {
			if (sources.empty()) {
				throw std::runtime_error("Called Wait() with no event sources.");
			}
			
			WaitReceiver receiver;
			WaitRecurse(receiver, sources, 0);
		}
		
	}
	
}

