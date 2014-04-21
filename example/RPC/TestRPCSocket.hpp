#include <p2p.hpp>
#include <p2p/RPC.hpp>
#include <p2p/UDP.hpp>

using namespace p2p;

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

