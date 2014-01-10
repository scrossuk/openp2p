#include <OpenP2P.hpp>
#include <OpenP2P/RPC.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

class TestRPCSocket: public RPC::Socket<IP::Endpoint, uint32_t> {
	public:
		TestRPCSocket(UDP::Socket& socket) : socket_(socket) { }
		
		void send(const IP::Endpoint& endpoint, bool isRequest, const uint32_t& id, const Buffer& data) {
			Buffer buffer;
			BufferBuilder builder(buffer);
			BinaryOStream stream(builder);
			stream << isRequest << id << data;
			
			socket_.send(endpoint, buffer);
		}
		
		bool receive(IP::Endpoint& endpoint, bool& isRequest, uint32_t& id, Buffer& data) {
			Buffer buff;
			
			if (socket_.receive(endpoint, buff)) {
				BufferIterator iterator(buff);
				BinaryIStream stream(iterator);
				return (stream >> isRequest >> id >> data);
			} else {
				return false;
			}
		}
		
		void cancel() {
			//
		}
		
		void close() {
			socket_.close();
		}
		
	private:
		UDP::Socket& socket_;
		
};

