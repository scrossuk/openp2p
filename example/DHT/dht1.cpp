#include <stdint.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <p2p.hpp>
#include <p2p/Kademlia.hpp>
#include <p2p/UDP.hpp>

void printData(const p2p::Buffer& buffer) {
	p2p::BufferIterator iterator(buffer);
	
	uint8_t v;
	
	while (iterator >> v) {
		std::cout << (char) v;
	}
	
	std::cout << std::endl;
}

typedef p2p::Kademlia::Id<1> IdType;
typedef p2p::Kademlia::Node<p2p::UDP::Endpoint, 1> NodeType;
typedef p2p::Kademlia::NodeGroup<p2p::UDP::Endpoint, 1> GroupType;

int main() {
	p2p::UDP::Socket socket(46667);
	
	std::cout << "UDP socket created" << std::endl;
	
	p2p::Kademlia::IdGenerator<1> generator;
	
	std::cout << "Created ID Generator" << std::endl;
	
	p2p::RPC::Protocol<p2p::UDP::Endpoint, IdType> protocol(socket, generator);
	
	std::cout << "Created RPC Protocol" << std::endl;
	
	p2p::Kademlia::MapDatabase<1> database;
	
	std::cout << "Created database" << std::endl;
	
	IdType myId;
	myId.data[0] = 'A';
	
	p2p::Kademlia::DHT<p2p::UDP::Endpoint, 1> dht(protocol, myId, database);
	
	std::cout << "Created DHT" << std::endl;
	
	boost::optional<NodeType> r1 = dht.addEndpoint(p2p::UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 46668), p2p::Timeout(2.0));
	
	if (r1) {
		std::cout << "Endpoint found " << (*r1).endpoint.port() << " " << r1->id.data[0] << std::endl;
	} else {
		std::cout << "Endpoint not found" << std::endl;
	}
	
	std::cout << "Finding node..." << std::endl;
	
	IdType findId;
	findId.data[0] = 'B';
	boost::optional<NodeType> r2 = dht.findNode(findId, p2p::Timeout(2.0));
	
	if (r2) {
		std::cout << "Node found" << std::endl;
	} else {
		std::cout << "Node not found" << std::endl;
	}
	
	std::cout << "Bucket has:" << std::endl;
	
	GroupType group1 = dht.bucketNearest(findId);
	
	for (GroupType::Iterator i = group1.iterator(); i.isValid(); i++) {
		std::cout << "   " << (*i).endpoint.port() << " " << (*i).id.data[0] << std::endl;
	}
	
	std::cout << "DHT Nearest is:" << std::endl;
	GroupType group2 = dht.findNearest(findId);
	
	for (GroupType::Iterator i = group2.iterator(); i.isValid(); i++) {
		std::cout << "   " << (*i).endpoint.port() << " " << (*i).id.data[0] << std::endl;
	}
	
	std::cout << "Storing..." << std::endl;
	
	IdType dataId;
	dataId.data[0] = 'C';
	bool r3 = dht.store(dataId, p2p::MakeBuffer<TextStream>("Hello world"), p2p::Timeout(2.0));
	
	if (r3) {
		std::cout << "Store successful" << std::endl;
	} else {
		std::cout << "Store failed" << std::endl;
	}
	
	std::cout << "Retrieving..." << std::endl;
	
	boost::optional<p2p::Buffer> r4 = dht.findValue(dataId, p2p::Timeout(2.0));
	
	if (r4) {
		std::cout << "Found data..." << std::endl;
		printData(*r4);
	} else {
		std::cout << "Data not found" << std::endl;
	}
	
	return 0;
}

