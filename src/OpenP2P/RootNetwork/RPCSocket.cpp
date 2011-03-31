#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/IdSocket.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		IdSocket::IdSocket(Socket<Endpoint>& socket) : socket_(socket){
			
		}

		void IdSocket::send(const Endpoint& endpoint, const Id& id, const Buffer& data){
			BufferBuilder builder;
			BinaryStream stream(builder);
			stream << RPC_REQUEST << id << data;
			socket_.send(endpoint, builder.getBuffer());
		}
				
		bool IdSocket::receive(Endpoint& endpoint, Id& id, Buffer& data);
			Buffer buffer;
			while(socket_.receive(endpoint, buffer)){
				BufferIterator iterator(buffer);
				bool isRequest = false;
				BinaryStream stream(iterator);

				stream >> isRequest >> id >> data;
				if(isRequest){
					
				}else{
					
				}
			}

			return false;
		}

		void IdSocket::cancel(){
			socket_.cancel();
		}

	}

}

