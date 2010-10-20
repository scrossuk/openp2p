#ifndef OPENP2P_DHT_RPCMANAGER_HPP
#define OPENP2P_DHT_RPCMANAGER_HPP

#include <map>
#include <boost/function.hpp>
#include "../Core.hpp"
#include "Hash.hpp"
#include "NodeGroup.hpp"
#include "BucketSet.hpp"
#include "Node.hpp"

namespace openp2p{

	namespace DHT{

		class Node;
		class Database;
		class Hash;
		class BucketSet;
		struct RPCHandler;

		class RPCManager: public Core::SharedCount{
			public:
				RPCManager(Core::SocketBuilder, Hash, Database *, double);

				void Destroy();

				void Nearest(Hash, boost::function<void (NodeGroup)>, boost::function<void ()>);

				void Ping(Core::Endpoint, boost::function<void (Node)>, boost::function<void ()>);

				void Store(Core::Endpoint, Hash, Core::Buffer, boost::function<void ()>, boost::function<void ()>);

				void FindNode(Core::Endpoint, Hash, boost::function<void (NodeGroup)>, boost::function<void ()>);

				void FindValue(Core::Endpoint, Hash, boost::function<void (Core::Buffer)>, boost::function<void ()>);

			private:
				void OnData(Core::Endpoint, Core::Buffer);

				void OnFindValue(Core::Endpoint, Core::Uint32, Hash);

				void OnSuccess(Core::Uint32, Node, Core::Buffer);

				void OnFail(Core::Uint32);

				void DoDestroy();

				Core::Uint32 AddRPC(RPCHandler);

				void DoNearest(Hash, boost::function<void (NodeGroup)>, boost::function<void ()>);

				void DoPing(Core::Endpoint, boost::function<void (Node)>, boost::function<void ()>);

				void DoStore(Core::Endpoint, Hash, Core::Buffer, boost::function<void ()>, boost::function<void ()>);

				void DoFindNode(Core::Endpoint, Hash, boost::function<void (NodeGroup)>, boost::function<void ()>);

				void DoFindValue(Core::Endpoint, Hash, boost::function<void (Core::Buffer)>, boost::function<void ()>);

				void HandlePing(boost::function<void (Node)>, Node);

				void HandleStore(boost::function<void ()>);

				void HandleFindNode(boost::function<void (NodeGroup)>, Core::Buffer);

				void HandleFindValue(boost::function<void (Core::Buffer)>, Core::Buffer);

				bool mIsActive;
				BucketSet mBucketSet;
				Core::Socket * mSocket;
				Hash mHash;
				Core::Strand mStrand;
				std::map<Core::Uint32, RPCHandler> mRPCs;
				Database * mDatabase;
				double mTimeout;
				Core::Uint32 mNextID;
				Core::Timer mTimer;


		};

	}

}

#endif
