#include <stdint.h>
#include <stdio.h>

#include <OpenP2P.hpp>
#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

int main() {
	UDP::Socket socket(46667);
	
	Crypt::AutoSeededRandomPool rand;
	Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
	
	RootNetwork::PrivateIdentity privateIdentity(privateKey);
	
	RootNetwork::PacketSocket packetSocket(socket);
	
	RootNetwork::AuthenticatedSocket authenticatedSocket(privateIdentity, packetSocket);
	
	RootNetwork::Service service(authenticatedSocket);
	
	const RootNetwork::NodeId nodeId = service.identifyEndpoint(UDP::Endpoint(IP::V4Address::Localhost(), 46668));
	
	(void) nodeId;
	
	const auto endpoint = service.pingNode(UDP::Endpoint(IP::V4Address::Localhost(), 46668), RootNetwork::NodeId());
	
	printf("My endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
	
	const auto networks = service.queryNetworks(UDP::Endpoint(IP::V4Address::Localhost(), 46668), RootNetwork::NodeId());
	
	printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
	
	for (size_t i = 0; i < networks.size(); i++) {
		printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
		if (networks.at(i) == RootNetwork::NetworkId::Generate("test")) {
			printf("         -> Supports 'test' network.\n");
		}
	}
	
	return 0;
}

