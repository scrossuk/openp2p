#include <iostream>
#include <boost/asio.hpp>
#include "../../src/Core.hpp"
#include "../../src/DHT.hpp"

using namespace openp2p::Core;
using namespace openp2p::DHT;

void printBuffer(Buffer buffer){
	try{
		std::string str;
		BufferIterator iterator(buffer);
		DataReader reader(iterator);
		reader >> str;

		std::cout << "String: " << str << std::endl;
	}catch(BufferOutOfBoundsException){
		std::cout << "Not a string" << std::endl;
	}
}

void endpointFound(Node node){
	std::cout << "Endpoint found " << node.GetEndpoint().Port() << std::endl;
}

void endpointNotFound(){
	std::cout << "Endpoint not found" << std::endl;
}

void nodeFound(Node node){
	std::cout << "Node found" << std::endl;
}

void nodeNotFound(){
	std::cout << "Node not found" << std::endl;
}

void storeSuccess(){
	std::cout << "Store successful" << std::endl;
}

void storeFail(){
	std::cout << "Store failed" << std::endl;
}

void valueFound(Buffer buffer){
	std::cout << "Found data..." << std::endl;
	printBuffer(buffer);
}

void valueNotFound(){
	std::cout << "Data not found" << std::endl;
}

int main(){
	/*boost::asio::ip::udp::socket socket(GetGlobalIOService());
	socket.open(boost::asio::ip::udp::v4());
	boost::asio::socket_base::reuse_address option(true);
	socket.set_option(option);

	try{
		socket.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 12345));
	}catch(...){
		std::cout << "Exception" << std::endl;
		return 0;
	}*/

	char s[1];

	Network dht(UDPSocketBuilder(46667), Hash('A'), new MapDatabase());

	dht.AddEndpoint(Endpoint(boost::asio::ip::address_v4::loopback(), 46668), endpointFound, endpointNotFound);

	std::cin.getline(s, 1);

	std::cout << "Finding node..." << std::endl;

	dht.FindNode(Hash('B'), nodeFound, nodeNotFound);

	std::cin.getline(s, 1);

	std::cout << "Storing..." << std::endl;

	dht.Store(Hash('C'), Buffer("Hello world"), storeSuccess, storeFail);

	std::cin.getline(s, 1);

	std::cout << "Retrieving..." << std::endl;

	dht.FindValue(Hash('C'), valueFound, valueNotFound);

	std::cin.getline(s, 1);

	return 0;
}
