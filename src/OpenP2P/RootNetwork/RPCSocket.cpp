#include <OpenP2P/BinaryStream.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/Socket.hpp>

#include <OpenP2P/RootNetwork/Endpoint.hpp>
#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/RPCSocket.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		RPCSocket::RPCSocket(Socket<Endpoint>& socket) : socket_(socket){

		}

		void RPCSocket::send(const Endpoint& endpoint, bool isRequest, const Id& id, const Buffer& data){
			BufferBuilder builder;
			BinaryStream stream(builder);
			stream << isRequest << id << data;
			socket_.send(endpoint, builder.getBuffer());
		}

		bool RPCSocket::receive(Endpoint& endpoint, bool& isRequest, Id& id, Buffer& data){
			Buffer buffer;
			while(socket_.receive(endpoint, buffer)){
				BufferIterator iterator(buffer);
				BinaryStream stream(iterator);

				if(stream >> isRequest >> id >> data){
					return true;
				}
			}

			return false;
		}

		void RPCSocket::cancel(){
			socket_.cancel();
		}

		void RPCSocket::close(){
			socket_.close();
		}

	}

}

