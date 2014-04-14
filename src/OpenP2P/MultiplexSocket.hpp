#ifndef OPENP2P_MULTIPLEXSOCKET_HPP
#define OPENP2P_MULTIPLEXSOCKET_HPP

#include <boost/thread.hpp>

namespace OpenP2P {
	
	template <typename EndpointType, typename MessageType>
	class MultiplexHost {
		public:
			MultiplexHost(Socket<EndpointType, MessageType>& socket)
				: nextClientId_(0), socket_(socket) { }
			
			bool isValid() const {
				return socket_.isValid();
			}
			
			size_t addClient() {
				const size_t clientId = nextClientId_++;
				clientPositions_.insert(std::make_pair(clientId, currentPosition_));
				return clientId;
			}
			
			void removeClient(size_t clientId) {
				clientPositions_.erase(clientId);
			}
			
			Event::Source eventSource(size_t clientId) const {
				
			}
			
			bool send(const EndpointType& endpoint, const MessageType& message) {
				return socket_.send(endpoint, message);
			}
			
			bool receive(size_t clientId, EndpointType& endpoint, MessageType& message) {
				while (!messageQueue_.empty()) {
					if (messageQueue_.top().consumeCount >= clientPositions_
				}
			}
			
		private:
			struct QueuedMessage {
				EndpointType endpoint;
				MessageType message;
				size_t consumeCount;
			};
			
			size_t nextClientId_;
			Socket<EndpointType, MessageType>& socket_;
			size_t currentPosition_;
			std::queue<QueuedMessage> messageQueue_;
			std::map<size_t, size_t> clientPositions_;
			
	};
	
	template <typename EndpointType, typename MessageType>
	class MultiplexClient {
		public:
			MultiplexClient(MultiplexHost<EndpointType, MessageType>& host)
				: host_(host) { }
			
			bool isValid() const {
				return host_.isValid();
			}
			
			Event::Source eventSource() const {
				return host_.eventSource();
			}
			
			bool send(const EndpointType& endpoint, const MessageType& message) {
				
			}
			
			bool receive(EndpointType& endpoint, MessageType& message) {
				
			}
			
		private:
			MultiplexHost<EndpointType, MessageType>& host_;
			
	};
	
}

#endif
