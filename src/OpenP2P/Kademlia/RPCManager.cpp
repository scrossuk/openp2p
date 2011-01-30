#include <boost/optional.hpp>
#include "../Core.hpp"
#include "Hash.hpp"
#include "Node.hpp"
#include "Database.hpp"
#include "RPCManager.hpp"
#include "BucketSet.hpp"
#include <iostream>

namespace openp2p{

	namespace Kademlia{

		using namespace Core;
		
		const double DefaultTimeout = 5.0;

		enum RPCType{
			RPC_PING = 0,
			RPC_STORE,
			RPC_FIND_NODE,
			RPC_FIND_VALUE,
			RPC_SUCCESS,
			RPC_FAIL
		};

		struct RPCHandler{
			Signal& signal;
			bool success;
			Node node;
			Buffer buffer;

			RPCHandler(Signal& s) : signal(s){ }
			
		};

		RPCManager::RPCManager(Socket& socket, Hash hash, Database& database) :
				mBucketSet(hash), mHash(hash), mDatabase(database), mSocket(socket),
				mThread(boost::bind(&RPCManager::ReceiveThread, this)){
			
			mNextID = 0;
		}
		
		RPCManager::~RPCManager(){
			mSignal.Activate();
			mThread.join();
		}

		NodeGroup RPCManager::Nearest(Hash hash){
			return mBucketSet.GetNearest(hash, 20);
		}

		Node RPCManager::Ping(Endpoint endpoint){
			Signal signal;
			RPCHandler handler(signal);
			Uint32 rpcID = AddRPC(&handler);
			BufferBuilder builder;
			builder << Uint8(RPC_PING) << rpcID << mHash;
			mSocket.Send(endpoint, *builder);
			signal.Wait();
			return handler.node;
		}

		bool RPCManager::Store(Endpoint endpoint, Hash hash, Buffer buffer){
			Signal signal;
			RPCHandler handler(signal);
			Uint32 rpcID = AddRPC(&handler);
			BufferBuilder builder;
			builder << Uint8(RPC_STORE) << rpcID << mHash << hash << buffer;
			mSocket.Send(endpoint, *builder);
			
			Timeout timeout(DefaultTimeout);
			if(Join(signal, timeout) == timeout){
				return false;
			}
			
			return handler.success;
		}

		void RPCManager::FindNode(Endpoint endpoint, Hash hash){
			Signal signal;
			RPCHandler handler(signal);
			Uint32 rpcID = AddRPC(handler);
			BufferBuilder builder;
			builder << Uint8(RPC_FIND_NODE) << rpcID << mHash << hash;
			mSocket.Send(endpoint, *builder);
			signal.Wait();
			return handler.
		}

		void RPCManager::FindValue(Endpoint endpoint, Hash hash, boost::function<void (Buffer)> onSuccess, boost::function<void ()> onFail){
			Join join;
			RPCHandler handler(boost::bind(&RPCManager::HandleFindValue, this, join.Wrap(onSuccess), _2), join.Wrap(onFail));
			Uint32 rpcID = AddRPC(handler);
			mSocket.Send(endpoint, Buffer(Uint8(RPC_FIND_VALUE)) + Buffer(rpcID) + mHash.Serialize() + hash.Serialize());
			mTimer.Add(mTimeout, join.Wrap(onFail));
		}

		void RPCManager::ReceiveThread(){
			while(true){
				Endpoint endpoint;
				Buffer buffer;
				if(!socketWait.Receive(endpoint, buffer, mSignal)){
					return;
				}
			
				try{
					std::cout << "Received: " << endpoint.port() << std::endl;
					BufferIterator iterator(buffer);
					Uint8 rpcType;
					Uint32 rpcID;
					id sender_id, data_id;
					Buffer data;
					BufferBuilder builder;
					
					iterator >> rpcType >> rpcID >> senderHash;

					Node senderNode(senderHash, endpoint);

					mBucketSet.Add(senderNode);

					switch(rpcType){
						case RPC_PING:
							builder << Uint8(RPC_SUCCESS) << rpcID << mHash;
							mSocket.Send(endpoint, *builder);
							break;
						case RPC_STORE:
							iterator >> dataHash >> data;

							if(mDatabase->Store(dataHash, data)){
								builder << Uint8(RPC_SUCCESS) << rpcID << mHash;
							}else{
								builder << Uint8(RPC_FAIL) << rpcID << mHash;
								std::cout << "Store failed" << std::endl;
							}
							mSocket.Send(endpoint, *builder);
							break;
						case RPC_FIND_NODE:
							iterator >> dataHash;
							
							builder << Uint8(RPC_SUCCESS) << rpcID << mHash << mBucketSet.GetNearest(Hash(hashData), 20);
							socket_.send(endpoint, *builder);
							break;
						case RPC_FIND_VALUE:
							iterator >> dataHash;
							
							OnFindValue(endpoint, rpcID, dataHash);
							break;
						case RPC_SUCCESS:
							builder >> data;
						
							OnSuccess(rpcID, senderNode, data);
							break;
						case RPC_FAIL:
							OnFail(rpcID);
							break;
						default:
							std::cout << "Unknown rpc" << std::endl;
					}

				}catch(IStreamException){
					std::cout << "Badly formed message" << std::endl;
					return;
				}
			}
		}

		void RPCManager::OnFindValue(Endpoint endpoint, Uint32 rpcID, Hash hash){
			boost::optional<Buffer> r = mDatabase.Retrieve(hash);
			BufferBuilder builder;
			
			if(r){
				builder << Uint8(RPC_SUCCESS) << rpcID << mHash << *r;
			}else{
				builder << Uint8(RPC_FAIL) << rpcID << mHash;
			}
			
			mSocket.Send(endpoint, *builder);
		}

		void RPCManager::OnSuccess(Uint32 rpcID, Node node, Buffer buffer){
			std::map<Uint32, RPCHandler *>::iterator p = mRPCs.find(rpcID);

			if(p != mRPCs.end()){
				RPCHandler * handler = p->second;
				handler->success = true;
				handler->node = node;
				handler->buffer = buffer;
				handler->signal.Activate();
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

		Uint32 RPCManager::AddRPC(RPCHandler * rpcHandler){
			mRPCs.insert(std::pair<Uint32, RPCHandler *>(mNextID, rpcHandler));
			return mNextID++;
		}

	}

}


