#ifndef P2P_CONCURRENCY_MESSAGEQUEUE_HPP
#define P2P_CONCURRENCY_MESSAGEQUEUE_HPP

#include <mutex>
#include <queue>

#include <p2p/Event/Signal.hpp>
#include <p2p/Event/Source.hpp>

namespace p2p {

	template <typename T>
	class MessageQueue {
		public:
			MessageQueue() { }
			
			Event::Source eventSource() const {
				return signal_.eventSource();
			}
			
			bool empty() const {
				std::lock_guard<std::mutex> lock(mutex_);
				return queue_.empty();
			}
			
			T receive() {
				std::lock_guard<std::mutex> lock(mutex_);
				auto message = std::move(queue_.front());
				queue_.pop();
				return message;
			}
			
			void send(T message) {
				std::lock_guard<std::mutex> lock(mutex_);
				queue_.push(std::move(message));
				signal_.activate();
			}
			
		private:
			mutable std::mutex mutex_;
			std::queue<T> queue_;
			Event::Signal signal_;
			
	};
	
}

#endif
