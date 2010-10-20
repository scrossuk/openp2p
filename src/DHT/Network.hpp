#ifndef OPENP2P_DHT_NETWORK_HPP
#define OPENP2P_DHT_NETWORK_HPP

#include <boost/function.hpp>
#include "../Core.hpp"

namespace openp2p{

	namespace DHT{

		class Database;
		class Hash;
		class Node;
		class RPCManager;
		class NodeGroup;

		class Network{
			public:
				Network(Core::SocketBuilder, Hash, Database *);

				~Network();

				void AddEndpoint(Core::Endpoint, boost::function<void (Node)>, boost::function<void ()>);

				void Store(Hash, Buffer, boost::function<void ()>, boost::function<void ()>);

				void FindNode(Hash, boost::function<void (Node)>, boost::function<void ()>);
				
				void FindNearestNodes(Hash, boost::function<void (NodeGroup)>, boost::function<void ()>);

				void FindValue(Hash, boost::function<void (Core::Buffer)>, boost::function<void ()>);

			private:
				Core::SharedPtr<RPCManager> mRPCManager;

		};

	}

}

#endif
