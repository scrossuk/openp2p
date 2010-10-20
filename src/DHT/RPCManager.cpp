#include <boost/function.hpp>
#include <boost/tuple/tuple.hpp>
#include "../Core.hpp"
#include "Hash.hpp"
#include "Node.hpp"
#include "Database.hpp"
#include "RPCManager.hpp"
#include "BucketSet.hpp"
#include <iostream>

namespace openp2p{

	namespace DHT{

		using namespace Core;

		enum RPCType{
			RPC_PING = 0,
			RPC_STORE,
			RPC_FIND_NODE,
			RPC_FIND_VALUE,
			RPC_SUCCESS,
			RPC_FAIL
		};

		struct RPCHandler{
			boost::function<void (Node, Buffer)> onSuccess;
			boost::function<void ()> onFail;

			RPCHandler(boost::function<void (Node, Buffer)> success, boost::function<void ()> fail){
				onSuccess = success;
				onFail = fail;
			}
		};

		RPCManager::RPCManager(SocketBuilder builder, Hash hash, Database * database, double timeout) : mBucketSet(hash), mHash(hash){
			mSocket = builder(SharedPtr<RPCManager>(this).Wrap(mStrand.Wrap(boost::bind(&RPCManager::OnData, this, _1, _2))));
			mDatabase = database;
			mTimeout = timeout;
			mNextID = 0;
			mIsActive = true;
		}

		void RPCManager::Destroy(){
			mStrand.Schedule(boost::bind(&RPCManager::DoDestroy, this));
		}

		void RPCManager::Nearest(Hash hash, boost::function<void (NodeGroup)> onSuccess, boost::function<void ()> onFail){
			mStrand.Schedule(boost::bind(&RPCManager::DoNearest, this, hash, onSuccess, onFail));
		}

		void RPCManager::Ping(Endpoint endpoint, boost::function<void (Node)> onSuccess, boost::function<void ()> onFail){
			mStrand.Schedule(boost::bind(&RPCManager::DoPing, this, endpoint, onSuccess, onFail));
		}

		void RPCManager::Store(Endpoint endpoint, Hash hash, Buffer buffer, boost::function<void ()> onSuccess, boost::function<void ()> onFail){
			mStrand.Schedule(boost::bind(&RPCManager::DoStore, this, endpoint, hash, buffer, onSuccess, onFail));
		}

		void RPCManager::FindNode(Endpoint endpoint, Hash hash, boost::function<void (NodeGroup)> onSuccess, boost::function<void ()> onFail){
			mStrand.Schedule(boost::bind(&RPCManager::DoFindNode, this, endpoint, hash, onSuccess, onFail));
		}

		void RPCManager::FindValue(Endpoint endpoint, Hash hash, boost::function<void (Buffer)> onSuccess, boost::function<void ()> onFail){
			mStrand.Schedule(boost::bind(&RPCManager::DoFindValue, this, endpoint, hash, onSuccess, onFail));
		}

		void RPCManager::OnData(Endpoint endpoint, Buffer buffer){
			if(!mIsActive){
				return;
			}
		
			try{
				std::cout << "Received: " << endpoint.Port() << std::endl;
				BufferIterator iterator(buffer);
				DataReader reader(iterator);
				Uint8 rpcType;
				Uint32 rpcID;
				Uint8 hashData[Hash::SizeInBytes];

				reader >> rpcType;
				reader >> rpcID;
				iterator.Read(hashData, Hash::SizeInBytes);

				Hash senderHash(hashData);
				Node senderNode(senderHash, endpoint);

				mBucketSet.Add(senderNode);

				switch(rpcType){
					case RPC_PING:
						mSocket->Send(endpoint, Buffer(Uint8(RPC_SUCCESS)) + Buffer(rpcID) + mHash.Serialize());
						break;
					case RPC_STORE:
						iterator.Read(hashData, Hash::SizeInBytes);

						if(mDatabase->Store(Hash(hashData), iterator.Read(iterator.Remaining()))){
							mSocket->Send(endpoint, Buffer(Uint8(RPC_SUCCESS)) + Buffer(rpcID) + mHash.Serialize());
						}else{
							mSocket->Send(endpoint, Buffer(Uint8(RPC_FAIL)) + Buffer(rpcID) + mHash.Serialize());
							std::cout << "Store failed" << std::endl;
						}
						break;
					case RPC_FIND_NODE:
						iterator.Read(hashData, Hash::SizeInBytes);

						mSocket->Send(endpoint, Buffer(Uint8(RPC_SUCCESS)) + Buffer(rpcID) + mHash.Serialize() + (mBucketSet.GetNearest(Hash(hashData), 20)).Serialize());
						break;
					case RPC_FIND_VALUE:
						iterator.Read(hashData, Hash::SizeInBytes);

						OnFindValue(endpoint, rpcID, Hash(hashData));
						break;
					case RPC_SUCCESS:
						OnSuccess(rpcID, senderNode, iterator.Read(iterator.Remaining()));
						break;
					case RPC_FAIL:
						OnFail(rpcID);
						break;
					default:
						std::cout << "Unknown rpc" << std::endl;
				}

			}catch(BufferOutOfBoundsException){
				std::cout << "Badly formed message" << std::endl;
				return;
			}
		}

		void RPCManager::OnFindValue(Endpoint endpoint, Uint32 rpcID, Hash hash){
			boost::tuple<bool, Buffer> r = mDatabase->Retrieve(hash);
			bool hasValue = r.get<0>();
			Buffer buffer = r.get<1>();
			if(hasValue){
				mSocket->Send(endpoint, Buffer(Uint8(RPC_SUCCESS)) + Buffer(rpcID) + mHash.Serialize() + buffer);
			}else{
				mSocket->Send(endpoint, Buffer(Uint8(RPC_FAIL)) + Buffer(rpcID) + mHash.Serialize());
			}
		}

		void RPCManager::OnSuccess(Uint32 rpcID, Node node, Buffer buffer){
			std::map<Uint32, RPCHandler>::iterator p = mRPCs.find(rpcID);

			if(p != mRPCs.end()){
				p->second.onSuccess(node, buffer);
				mRPCs.erase(p);
			}else{
				std::cout << "RPC not found" << std::endl;
			}
		}

		void RPCManager::OnFail(Uint32 rpcID){
			std::map<Uint32, RPCHandler>::iterator p = mRPCs.find(rpcID);

			if(p != mRPCs.end()){
				p->second.onFail();
				mRPCs.erase(p);
			}else{
				std::cout << "RPC not found" << std::endl;
			}
		}

		void RPCManager::DoDestroy(){
			mIsActive = false;
			
			for(std::map<Uint32, RPCHandler>::iterator p = mRPCs.begin(); p != mRPCs.end(); p++){
				p->second.onFail();
			}
			
			mRPCs.clear();
			
			delete mSocket;
		}

		Uint32 RPCManager::AddRPC(RPCHandler rpcHandler){
			mRPCs.insert(std::pair<Uint32, RPCHandler>(mNextID, rpcHandler));
			return mNextID++;
		}

		void RPCManager::DoNearest(Hash hash, boost::function<void (NodeGroup)> onSuccess, boost::function<void ()> onFail){
			if(mIsActive){
				onSuccess(mBucketSet.GetNearest(hash, 20));
			}else{
				onFail();
			}
		}

		void RPCManager::DoPing(Core::Endpoint endpoint, boost::function<void (Node)> onSuccess, boost::function<void ()> onFail){
			if(!mIsActive){
				onFail();
				return;
			}

			Join join;
			RPCHandler handler(boost::bind(&RPCManager::HandlePing, this, join.Wrap(onSuccess), _1), join.Wrap(onFail));
			Uint32 rpcID = AddRPC(handler);
			mSocket->Send(endpoint, Buffer(Uint8(RPC_PING)) + Buffer(rpcID) + mHash.Serialize());
			mTimer.Add(mTimeout, join.Wrap(onFail));
		}

		void RPCManager::DoStore(Core::Endpoint endpoint, Hash hash, Buffer buffer, boost::function<void ()> onSuccess, boost::function<void ()> onFail){
			if(!mIsActive){
				onFail();
				return;
			}

			Join join;
			RPCHandler handler(boost::bind(&RPCManager::HandleStore, this, join.Wrap(onSuccess)), join.Wrap(onFail));
			Uint32 rpcID = AddRPC(handler);
			mSocket->Send(endpoint, Buffer(Uint8(RPC_STORE)) + Buffer(rpcID) + mHash.Serialize() + hash.Serialize() + buffer);
			mTimer.Add(mTimeout, join.Wrap(onFail));
		}

		void RPCManager::DoFindNode(Endpoint endpoint, Hash hash, boost::function<void (NodeGroup)> onSuccess, boost::function<void ()> onFail){
			if(!mIsActive){
				onFail();
				return;
			}

			Join join;
			RPCHandler handler(boost::bind(&RPCManager::HandleFindNode, this, join.Wrap(onSuccess), _2), join.Wrap(onFail));
			Uint32 rpcID = AddRPC(handler);
			mSocket->Send(endpoint, Buffer(Uint8(RPC_FIND_NODE)) + Buffer(rpcID) + mHash.Serialize() + hash.Serialize());
			mTimer.Add(mTimeout, join.Wrap(onFail));
		}

		void RPCManager::DoFindValue(Endpoint endpoint, Hash hash, boost::function<void (Buffer)> onSuccess, boost::function<void ()> onFail){
			if(!mIsActive){
				onFail();
				return;
			}

			Join join;
			RPCHandler handler(boost::bind(&RPCManager::HandleFindValue, this, join.Wrap(onSuccess), _2), join.Wrap(onFail));
			Uint32 rpcID = AddRPC(handler);
			mSocket->Send(endpoint, Buffer(Uint8(RPC_FIND_VALUE)) + Buffer(rpcID) + mHash.Serialize() + hash.Serialize());
			mTimer.Add(mTimeout, join.Wrap(onFail));
		}

		void RPCManager::HandlePing(boost::function<void (Node)> success, Node node){
			success(node);
		}

		void RPCManager::HandleStore(boost::function<void ()> success){
			success();
		}

		void RPCManager::HandleFindNode(boost::function<void (NodeGroup)> success, Buffer buffer){
			BufferIterator iterator(buffer);
			success(NodeGroup::Unserialize(mHash, iterator));
		}

		void RPCManager::HandleFindValue(boost::function<void (Buffer)> success, Buffer buffer){
			success(buffer);
		}

	}

}


