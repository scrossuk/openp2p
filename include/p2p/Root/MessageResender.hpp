#ifndef P2P_ROOTNETWORK_MESSAGERESENDER_HPP
#define P2P_ROOTNETWORK_MESSAGERESENDER_HPP

#include <map>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Timer.hpp>

#include <p2p/Root/Message.hpp>
#include <p2p/Root/Types.hpp>

namespace p2p {

	namespace Root {
	
		template <typename EndpointType>
		class MessageResender {
			public:
				MessageResender(Socket<EndpointType, Message>& socket, double timeoutMilliseconds)
					: socket_(socket) {
						timer_.setMilliseconds(timeoutMilliseconds);
						timer_.schedule();
					}
				
				Event::Source eventSource() const {
					return timer_.eventSource();
				}
				
				void processResends() {
					if (!timer_.hasExpired()) return;
					
					// Re-send requests.
					for (const auto& messagePair: routineMap_) {
						const auto& routineMessage = messagePair.second;
						socket_.send(routineMessage.endpoint, routineMessage.message);
					}
					
					timer_.schedule();
				}
				
				void startRoutine(RoutineId id, const EndpointType& endpoint, const Message& message) {
					routineMap_.insert(std::make_pair(id, RoutineMessage(endpoint, message)));
					(void) socket_.send(endpoint, message);
				}
				
				void endRoutine(RoutineId id) {
					routineMap_.erase(id);
				}
				
			private:
				// Non-copyable.
				MessageResender(const MessageResender&) = delete;
				MessageResender& operator=(MessageResender) = delete;
				
				struct RoutineMessage {
					EndpointType endpoint;
					Message message;
					
					inline RoutineMessage(EndpointType pEndpoint, Message pMessage)
						: endpoint(std::move(pEndpoint)),
						message(std::move(pMessage)) { }
				};
				
				Event::Timer timer_;
				std::map<RoutineId, RoutineMessage> routineMap_;
				Socket<EndpointType, Message>& socket_;
				
		};
		
	}
	
}

#endif
