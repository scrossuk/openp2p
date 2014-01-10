#include <stdint.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <OpenP2P.hpp>
#include <OpenP2P/Kademlia.hpp>
#include <OpenP2P/UDP.hpp>

void printData(const OpenP2P::Buffer& buffer) {
	OpenP2P::BufferIterator iterator(buffer);
	
	uint8_t v;
	
	while (iterator >> v) {
		std::cout << (char) v;
	}
	
	std::cout << std::endl;
}

typedef OpenP2P::Kademlia::Id<1> IdType;
typedef OpenP2P::Kademlia::Node<OpenP2P::UDP::Endpoint, 1> NodeType;
typedef OpenP2P::Kademlia::NodeGroup<OpenP2P::UDP::Endpoint, 1> GroupType;

int main() {
	OpenP2P::UDP::Socket socket(46667);
	
	std::cout << "UDP socket created" << std::endl;
	
	OpenP2P::Kademlia::IdGenerator<1> generator;
	
	std::cout << "Created ID Generator" << std::endl;
	
	OpenP2P::RPC::Protocol<OpenP2P::UDP::Endpoint, IdType> protocol(socket, generator);
	
	std::cout << "Created RPC Protocol" << std::endl;
	
	OpenP2P::Kademlia::MapDatabase<1> database;
	
	std::cout << "Created database" << std::endl;
	
	IdType myId;
	myId.data[0] = 'A';
	
	OpenP2P::Kademlia::DHT<OpenP2P::UDP::Endpoint, 1> dht(protocol, myId, database);
	
	std::cout << "Created DHT" << std::endl;
	
	boost::optional<NodeType> r1 = dht.addEndpoint(OpenP2P::UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 46668), OpenP2P::Timeout(2.0));
	
	if (r1) {
		std::cout << "Endpoint found " << (*r1).endpoint.port() << " " << r1->id.data[0] << std::endl;
	} else {
		std::cout << "Endpoint not found" << std::endl;
	}
	
	std::cout << "Finding node..." << std::endl;
	
	IdType findId;
	findId.data[0] = 'B';
	boost::optional<NodeType> r2 = dht.findNode(findId, OpenP2P::Timeout(2.0));
	
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
	bool r3 = dht.store(dataId, OpenP2P::MakeBuffer<TextStream>("Hello world"), OpenP2P::Timeout(2.0));
	
	if (r3) {
		std::cout << "Store successful" << std::endl;
	} else {
		std::cout << "Store failed" << std::endl;
	}
	
	std::cout << "Retrieving..." << std::endl;
	
	boost::optional<OpenP2P::Buffer> r4 = dht.findValue(dataId, OpenP2P::Timeout(2.0));
	
	if (r4) {
		std::cout << "Found data..." << std::endl;
		printData(*r4);
	} else {
		std::cout << "Data not found" << std::endl;
	}
	
	return 0;
}

