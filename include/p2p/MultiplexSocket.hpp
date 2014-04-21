#ifndef P2P_MULTIPLEXSOCKET_HPP
#define P2P_MULTIPLEXSOCKET_HPP

#include <stdint.h>

#include <deque>
#include <mutex>

#include <p2p/Socket.hpp>

#include <p2p/Event/MultiGenerator.hpp>
#include <p2p/Event/Source.hpp>

namespace p2p {
	
	template <typename EndpointType, typename MessageType>
	class MultiplexHost {
		public:
			MultiplexHost(Socket<EndpointType, MessageType>& socket)
				: socket_(socket), multiGenerator_(socket.eventSource()),
				basePosition_(0ull), numClients_(0ull) { }
			
			uint64_t addClient() {
				std::lock_guard<std::mutex> lock(mutex_);
				
				numClients_++;
				return basePosition_;
			}
			
			void removeClient(uint64_t clientPosition) {
				std::lock_guard<std::mutex> lock(mutex_);
				
				assert(clientPosition >= basePosition_);
				numClients_--;
				
				const auto queuePosition = clientPosition - basePosition_;
				
				// Reduce consume count of messages read by this client.
				for (size_t i = 0; i < queuePosition; i++) {
					messageQueue_.at(i).consumeCount--;
				}
			}
			
			bool isValid(uint64_t clientPosition) const {
				std::lock_guard<std::mutex> lock(mutex_);
				
				assert(clientPosition >= basePosition_);
				const auto currentPosition = basePosition_ + static_cast<uint64_t>(messageQueue_.size());
				
				if (clientPosition < currentPosition) {
					// Make sure client reads all queued data.
					return true;
				} else {
					return socket_.isValid();
				}
			}
			
			Event::MultiGenerator& eventGenerator() {
				return multiGenerator_;
			}
			
			const Event::MultiGenerator& eventGenerator() const {
				return multiGenerator_;
			}
			
			bool send(const EndpointType& endpoint, const MessageType& message) {
				std::lock_guard<std::mutex> lock(mutex_);
				return socket_.send(endpoint, message);
			}
			
			bool receive(uint64_t clientPosition, EndpointType& endpoint, MessageType& message) {
				std::lock_guard<std::mutex> lock(mutex_);
				
				assert(clientPosition >= basePosition_);
				const auto currentPosition = basePosition_ + static_cast<uint64_t>(messageQueue_.size());
				
				if (clientPosition < currentPosition) {
					const auto queuePosition = clientPosition - basePosition_;
					auto& messageData = messageQueue_.back();
					endpoint = messageData.endpoint;
					message = messageData.message;
					messageData.consumeCount++;
					
					if (queuePosition == 0 && messageData.consumeCount == numClients_) {
						basePosition_++;
						messageQueue_.pop_front();
					}
					
					return true;
				} else {
					// Try to read from the socket.
					const bool result = socket_.receive(endpoint, message);
					if (!result) return false;
					
					if (numClients_ > 1) {
						messageQueue_.push_back(QueuedMessage(endpoint, message, 1ull));
					}
					
					return true;
				}
			}
			
		private:
			// Non-copyable.
			MultiplexHost(const MultiplexHost<EndpointType, MessageType>&) = delete;
			MultiplexHost<EndpointType, MessageType>& operator=(MultiplexHost<EndpointType, MessageType>) = delete;
			
			struct QueuedMessage {
				EndpointType endpoint;
				MessageType message;
				size_t consumeCount;
				
				inline QueuedMessage(const EndpointType& pEndpoint,
						const MessageType& pMessage, size_t pConsumeCount)
					: endpoint(pEndpoint), message(pMessage),
					consumeCount(pConsumeCount) { }
			};
			
			mutable std::mutex mutex_;
			Socket<EndpointType, MessageType>& socket_;
			Event::MultiGenerator multiGenerator_;
			uint64_t basePosition_;
			size_t numClients_;
			std::deque<QueuedMessage> messageQueue_;
			
	};
	
	template <typename EndpointType, typename MessageType>
	class MultiplexClient: public Socket<EndpointType, MessageType> {
		public:
			MultiplexClient(MultiplexHost<EndpointType, MessageType>& host)
				: host_(host), position_(0ull), clientId_(0ull) {
					position_ = host_.addClient();
					clientId_ = host_.eventGenerator().addClient();
				}
				
			~MultiplexClient() {
				host_.eventGenerator().removeClient(clientId_);
				host_.removeClient(position_);
			}
			
			// Moveable.
			MultiplexClient(MultiplexClient<EndpointType, MessageType>&&) = default;
			MultiplexClient<EndpointType, MessageType>& operator=(MultiplexClient<EndpointType, MessageType>&&) = default;
			
			bool isValid() const {
				return host_.isValid(position_);
			}
			
			Event::Source eventSource() const {
				return host_.eventGenerator().eventSource(clientId_);
			}
			
			bool send(const EndpointType& endpoint, const MessageType& message) {
				return host_.send(endpoint, message);
			}
			
			bool receive(EndpointType& endpoint, MessageType& message) {
				const bool result = host_.receive(position_, endpoint, message);
				if (result) position_++;
				return result;
			}
			
		private:
			// Non-copyable.
			MultiplexClient(const MultiplexClient<EndpointType, MessageType>&) = delete;
			MultiplexClient<EndpointType, MessageType>& operator=(const MultiplexClient<EndpointType, MessageType>&) = delete;
			
			MultiplexHost<EndpointType, MessageType>& host_;
			uint64_t position_;
			size_t clientId_;
			
	};
	
}

#endif
