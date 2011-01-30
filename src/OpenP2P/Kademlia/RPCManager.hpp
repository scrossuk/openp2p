#ifndef OPENP2P_KADEMLIA_RPCMANAGER_HPP
#define OPENP2P_KADEMLIA_RPCMANAGER_HPP

#include <stdint.h>
#include <map>
#include <boost/optional.hpp>
#include <openp2p/buffer.hpp>
#include <openp2p/basic_socket.hpp>
#include <openp2p/kademlia/bucket_set.hpp>
#include <openp2p/kademlia/database.hpp>
#include <openp2p/kademlia/id.hpp>
#include <openp2p/kademlia/node.hpp>
#include <openp2p/kademlia/node_group.hpp>

namespace openp2p{

	namespace kademlia{

		enum rpc_type{
			RPC_PING = 0,
			RPC_STORE = 1,
			RPC_FIND_NODE = 2,
			RPC_FIND_VALUE = 3,
			RPC_SUCCESS = 4,
			RPC_FAIL = 5
		};

		template <class endpoint_type, std::size_t id_size>
		struct rpc_handler{
			signal& signal;
			bool success;
			node<endpoint_type, id_size> node;
			buffer buffer;

			rpc_handler(signal& s) : signal(s){ }
			
		};

		template <class endpoint_type, std::size_t id_size>
		class rpc_manager{
			public:
				rpc_manager(basic_socket<endpoint_type>& socket, const id<id_size>& id, database& database)
					: socket_(socket), id_(id), database_(database), bucket_set_(id),
					thread_(boost::bind(&rpc_manager::receive_thread, this)),
					next_id_(0){ }

				~rpc_manager(){
					signal_.activate();
					thread_.join();
				}

				node_group<endpoint_type> nearest(const id<id_size>& id){
					return bucket_set_.get_nearest(id, 20);
				}

				boost::optional< node<endpoint_type> > ping(const endpoint_type& endpoint){
					signal signal;
					rpc_handler handler(signal);
					Uint32 rpc_id = add_rpc(&handler);
					BufferBuilder builder;
					builder << uint8_t(RPC_PING) << rpc_id << id_;
					socket_.send(endpoint, *builder);
					wait(signal);
					return handler.node;
				}

				bool store(const endpoint_type& endpoint, const id<id_size>& id, const buffer& buffer){
					signal signal;
					rpc_handler handler(signal);
					uint32_t rpc_id = add_rpc(&handler);
					buffer_builder builder;
					builder << uint8_t(RPC_STORE) << rpc_id << id_ << hash << buffer;
					socket_.send(endpoint, *builder);
			
					Timeout timeout(DefaultTimeout);
					if(join(signal, timeout) == timeout){
						return false;
					}
			
					return handler.success;
				}

				node_group<endpoint_type> find_node(const endpoint_type& endpoint, const id<id_size>& id){
					Signal signal;
					rpc_handler handler(signal);
					Uint32 rpc_id = add_rpc(handler);
					BufferBuilder builder;
					builder << uint8_t(RPC_FIND_NODE) << rpc_id << id_ << hash;
					socket_.send(endpoint, *builder);
					signal.Wait();
					return handler.
				}

				boost::optional<buffer> find_value(const endpoint_type& endpoint, const id<id_size>& id){
					Join join;
					rpc_handler handler(boost::bind(&RPCManager::HandleFindValue, this, join.Wrap(onSuccess), _2), join.Wrap(onFail));
					Uint32 rpc_id = add_rpc(handler);
					socket_.send(endpoint, Buffer(uint8_t(RPC_FIND_VALUE)) + Buffer(rpc_id) + id_.Serialize() + hash.Serialize());
					mTimer.Add(mTimeout, join.Wrap(onFail));
				}

			private:
				void receive_thread(){
					while(true){
						endpoint_type endpoint;
						buffer buffer;
						if(!socket_.receive(endpoint, buffer, signal_)){
							return;
						}
			
						buffer_iterator iterator(buffer);
						uint8_t rpc_type;
						uint32_t rpc_id;
						id<id_size> sender_id, data_id;
						buffer data;
						buffer_builder builder;
					
						if(!(iterator >> rpc_type >> rpc_id >> sender_id)){
							continue;
						}

						node<endpoint_type, id_size> sender_node(sender_id, endpoint);

						bucket_set_.add(sender_node);

						switch(rpc_type){
							case RPC_PING:
								builder << uint8_t(RPC_SUCCESS) << rpc_id << id_;
								socket_.send(endpoint, *builder);
								break;
							case RPC_STORE:
								iterator >> data_id >> data;

								if(database_->store(data_id, data)){
									builder << uint8_t(RPC_SUCCESS) << rpc_id << id_;
								}else{
									builder << uint8_t(RPC_FAIL) << rpc_id << id_;
								}
								
								socket_.send(endpoint, *builder);
								break;
							case RPC_FIND_NODE:
								iterator >> data_id;
							
								builder << uint8_t(RPC_SUCCESS) << rpc_id << id_ << bucket_set_.get_nearest(data_id, 20);
								socket_.send(endpoint, *builder);
								break;
							case RPC_FIND_VALUE:
								iterator >> data_id;
							
								on_find_value(endpoint, rpc_id, data_id);
								break;
							case RPC_SUCCESS:
								builder >> data;
						
								on_success(rpc_id, sender_node, data);
								break;
							case RPC_FAIL:
								on_fail(rpc_id);
								break;
							default:
								std::cout << "Unknown rpc" << std::endl;
						}
					}
				}

				void on_find_value(const endpoint_type& endpoint, uint32_t rpc_id, const id<id_size>& id){
					boost::optional<buffer> r = database_.retrieve(id);
					buffer_builder builder;
			
					if(r){
						builder << uint8_t(RPC_SUCCESS) << rpc_id << id_ << *r;
					}else{
						builder << uint8_t(RPC_FAIL) << rpc_id << id_;
					}
			
					socket_.send(endpoint, *builder);
				}

				void on_success(uint32_t rpc_id, const node<endpoint_type, id_size>& node, const buffer& buffer){
					std::map<uint32_t, rpc_handler *>::iterator p = rpcs_.find(rpc_id);

					if(p != rpcs_.end()){
						rpc_handler * handler = p->second;
						handler->success = true;
						handler->node = node;
						handler->buffer = buffer;
						handler->signal.activate();
						rpcs_.erase(p);
					}
				}

				void on_fail(uint32_t rpc_id){
					std::map<uint32_t, rpc_handler *>::iterator p = rpcs_.find(rpc_id);

					if(p != rpcs_.end()){
						rpc_handler * handler = p->second;
						handler->success = false;
						handler->signal.activate();
						rpcs_.erase(p);
					}
				}

				uint32_t add_rpc(rpc_handler * handler){
					rpcs_.insert(std::pair<uint32_t, rpc_handler *>(next_id_, handler));
					return next_id_++;
				}

				signal signal_;
				bucket_set bucket_set_;
				basic_socket<endpoint_type>& socket_;
				hash hash_;
				std::map<uint32_t, rpc_handler *> rpcs_;
				database& database_;
				uint32_t next_id_;
				boost::thread thread_;

		};

	}

}

#endif
