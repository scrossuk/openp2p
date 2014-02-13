#include <stdint.h>
#include <iostream>
#include <OpenP2P.hpp>
#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

int main() {
	UDP::Socket socket(46668);
	
	Crypt::AutoSeededRandomPool rand;
	Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
	
	RootNetwork::PrivateIdentity privateIdentity(privateKey);
	
	RootNetwork::PacketSocket packetSocket(socket);
	
	RootNetwork::AuthenticatedSocket authenticatedSocket(privateIdentity, packetSocket);
	
	RootNetwork::Service service(authenticatedSocket);
	
	service.addNetwork("test");
	
	service.processRequests();
	
	return 0;
}

