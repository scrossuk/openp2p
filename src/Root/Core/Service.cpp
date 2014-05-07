#include <stdio.h>

#include <queue>

#include <p2p/Transport/Socket.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/UnionGenerator.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NetworkId.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>
#include <p2p/Root/RoutineIdGenerator.hpp>

#include <p2p/Root/Core/RPCMessage.hpp>
#include <p2p/Root/Core/Service.hpp>

namespace p2p {

	namespace Root {
	
		namespace Core {
			
			Service::Service(Socket<NodePair, Message>& socket, RoutineIdGenerator& routineIdGenerator)
				: socket_(socket), multiplexHost_(socket),
				clientSocket_(multiplexHost_),
				serverSocket_(multiplexHost_),
				client_(clientSocket_, routineIdGenerator),
				server_(serverSocket_),
				unionGenerator_({ client_.eventSource(), server_.eventSource() }) { }
			
			Service::~Service() { }
			
			Event::Source Service::eventSource() const {
				return unionGenerator_.eventSource();
			}
			
			bool Service::processMessage() {
				return client_.processResponse() || server_.processRequest();
			}
			
			void Service::addNetwork(const std::string& networkName) {
				server_.addNetwork(networkName);
			}
			
			RPC::Operation<NodeId> Service::identify(const Endpoint& endpoint) {
				return client_.identify(endpoint);
			}
			
			RPC::Operation<Endpoint> Service::ping(const Endpoint& endpoint, const NodeId& nodeId) {
				return client_.ping(endpoint, nodeId);
			}
			
			RPC::Operation<std::vector<NetworkId>> Service::queryNetworks(const Endpoint& endpoint, const NodeId& nodeId) {
				return client_.queryNetworks(endpoint, nodeId);
			}
			
		}
		
	}
	
}


