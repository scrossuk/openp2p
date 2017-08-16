#include <iostream>
#include <boost/optional.hpp>
#include <p2p.hpp>
#include <p2p/Kademlia.hpp>
#include <p2p/UDP.hpp>

template <size_t IdSize>
class TestDatabase: public p2p::Kademlia::Database<IdSize> {
	public:
		TestDatabase() {
			std::cout << "Created database" << std::endl;
		}
		
		bool store(const p2p::Kademlia::Id<IdSize>& id, const p2p::Buffer& data) {
			std::cout << "Database store: " << id.data[0] << std::endl;
			return database_.store(id, data);
		}
		
		boost::optional<p2p::Buffer> retrieve(const p2p::Kademlia::Id<IdSize>& id) {
			std::cout << "Database retrieve: " << id.data[0] << std::endl;
			return database_.retrieve(id);
		}
		
	private:
		p2p::Kademlia::MapDatabase<IdSize> database_;
		
};

typedef p2p::Kademlia::Id<1> IdType;

int main() {
	p2p::UDP::Socket socket(46668);
	
	std::cout << "UDP socket created" << std::endl;
	
	p2p::Kademlia::IdGenerator<1> generator;
	
	std::cout << "Created ID Generator" << std::endl;
	
	p2p::RPCProtocol<p2p::UDP::Endpoint, IdType> protocol(socket, generator);
	
	std::cout << "Created RPC Protocol" << std::endl;
	
	TestDatabase<1> database;
	
	std::cout << "Created database" << std::endl;
	
	IdType myId;
	myId.data[0] = 'B';
	
	p2p::Kademlia::DHT<p2p::UDP::Endpoint, 1> dht(protocol, myId, database);
	
	std::cout << "Created DHT" << std::endl;
	
	char s[1];
	
	std::cin.getline(s, 1);
	
	std::cout << "Got line" << std::endl;
	
	return 0;
}

