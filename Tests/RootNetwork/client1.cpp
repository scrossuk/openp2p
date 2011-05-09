#include <stdint.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <OpenP2P.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

int main(){
	UDP::Socket socket(46667);
	
	RootNetwork::DHT dht(socket, RootNetwork::Id::Zero());
	
	boost::optional<RootNetwork::Node> r1 = dht.addEndpoint(UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 46668));
	if(r1){
		std::cout << "Endpoint found " << r1->endpoint << std::endl;
	}else{
		std::cout << "Endpoint not found" << std::endl;
	}

	/*std::cout << "Finding node..." << std::endl;

	IdType findId;
	findId.data[0] = 'B';
	boost::optional<NodeType> r2 = dht.findNode(findId, OpenP2P::Timeout(2.0));
	if(r2){
		std::cout << "Node found" << std::endl;
	}else{
		std::cout << "Node not found" << std::endl;
	}
	
	std::cout << "Bucket has:" << std::endl;
	
	GroupType group1 = dht.bucketNearest(findId);
	for(GroupType::Iterator i = group1.iterator(); i.isValid(); i++){
		std::cout << "   " << (*i).endpoint.port() << " " << (*i).id.data[0] << std::endl;
	}
	
	std::cout << "DHT Nearest is:" << std::endl;
	GroupType group2 = dht.findNearest(findId);
	for(GroupType::Iterator i = group2.iterator(); i.isValid(); i++){
		std::cout << "   " << (*i).endpoint.port() << " " << (*i).id.data[0] << std::endl;
	}

	std::cout << "Storing..." << std::endl;

	IdType dataId;
	dataId.data[0] = 'C';
	bool r3 = dht.store(dataId, OpenP2P::MakeBuffer<TextStream>("Hello world"), OpenP2P::Timeout(2.0));
	if(r3){
		std::cout << "Store successful" << std::endl;
	}else{
		std::cout << "Store failed" << std::endl;
	}

	std::cout << "Retrieving..." << std::endl;

	boost::optional<OpenP2P::Buffer> r4 = dht.findValue(dataId, OpenP2P::Timeout(2.0));
	if(r4){
		std::cout << "Found data..." << std::endl;
		printData(*r4);
	}else{
		std::cout << "Data not found" << std::endl;
	}*/

	return 0;
}

