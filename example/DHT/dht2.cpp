#include <iostream>
#include <boost/optional.hpp>
#include <OpenP2P.hpp>
#include <OpenP2P/Kademlia.hpp>
#include <OpenP2P/UDP.hpp>

template <size_t IdSize>
class TestDatabase: public OpenP2P::Kademlia::Database<IdSize> {
	public:
		TestDatabase() {
			std::cout << "Created database" << std::endl;
		}
		
		bool store(const OpenP2P::Kademlia::Id<IdSize>& id, const OpenP2P::Buffer& data) {
			std::cout << "Database store: " << id.data[0] << std::endl;
			return database_.store(id, data);
		}
		
		boost::optional<OpenP2P::Buffer> retrieve(const OpenP2P::Kademlia::Id<IdSize>& id) {
			std::cout << "Database retrieve: " << id.data[0] << std::endl;
			return database_.retrieve(id);
		}
		
	private:
		OpenP2P::Kademlia::MapDatabase<IdSize> database_;
		
};

typedef OpenP2P::Kademlia::Id<1> IdType;

int main() {
	OpenP2P::UDP::Socket socket(46668);
	
	std::cout << "UDP socket created" << std::endl;
	
	OpenP2P::Kademlia::IdGenerator<1> generator;
	
	std::cout << "Created ID Generator" << std::endl;
	
	OpenP2P::RPCProtocol<OpenP2P::UDP::Endpoint, IdType> protocol(socket, generator);
	
	std::cout << "Created RPC Protocol" << std::endl;
	
	TestDatabase<1> database;
	
	std::cout << "Created database" << std::endl;
	
	IdType myId;
	myId.data[0] = 'B';
	
	OpenP2P::Kademlia::DHT<OpenP2P::UDP::Endpoint, 1> dht(protocol, myId, database);
	
	std::cout << "Created DHT" << std::endl;
	
	char s[1];
	
	std::cin.getline(s, 1);
	
	std::cout << "Got line" << std::endl;
	
	return 0;
}

