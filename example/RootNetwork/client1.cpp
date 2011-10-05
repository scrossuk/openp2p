#include <stdint.h>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <OpenP2P.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

int main(){
	/*UDP::Socket socket(46667);
	
	Crypt::AutoSeededRandomPool rand;
	
	Crypt::ECDSA::PrivateKey privateKey(rand);
	
	RootNetwork::PrivateIdentity privateIdentity(privateKey);
	
	RootNetwork::Service service(socket, privateIdentity);
	
	boost::optional<RootNetwork::Node> r1 = service.addEndpoint(UDP::Endpoint(boost::asio::ip::address_v4::loopback(), 46668));
	
	if(r1.get()){
		std::cout << "Endpoint found" << std::endl;
	}else{
		std::cout << "Endpoint not found" << std::endl;
	}
	
	std::cout << "Finding node..." << std::endl;
	
	RootNetwork::PublicIdentity publicIdentity(privateIdentity);
	
	//Look for ourselves
	Future<RootNetwork::Node> r2 = service.findNode(publicIdentity.id());
	
	if(*r2){
		std::cout << "Node found" << std::endl;
	}else{
		std::cout << "Node not found" << std::endl;
	}
	
	//Subscribe to a sub-network
	service.subscribe(RootNetwork::SubNetworkId("test"));*/
	
	return 0;
}

