#include <stdint.h>
#include <stdio.h>

#include <OpenP2P.hpp>
#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/MultiplexSocket.hpp>
#include <OpenP2P/RootNetwork.hpp>
#include <OpenP2P/UDP.hpp>

using namespace OpenP2P;

class EventThread: public Runnable {
	public:
		EventThread(Socket<RootNetwork::Endpoint, RootNetwork::Message>& socket, RootNetwork::RoutineIdGenerator& routineIdGenerator)
			: service_(socket, routineIdGenerator) { }
		
		void run() {
			service_.addNetwork("test");
			service_.addNetwork("p2p.rootdht");
			service_.processRequests();
		}
		
		void cancel() {
			// TODO...
		}
		
	private:
		RootNetwork::RPCService service_;
	
};

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("Usage: client [my port] [other port]\n");
		return -1;
	}
	
	const auto myPort = atoi(argv[1]);
	const auto otherPort = atoi(argv[2]);
	
	UDP::Socket udpSocket(myPort);
	
	// Send/receive data on appropriate transport.
	RootNetwork::TransportSocket transportSocket(udpSocket);
	
	// Serialize/unserialize packets.
	RootNetwork::PacketSocket packetSocket(transportSocket);
	
	// Generate a private key for our node.
	Crypt::AutoSeededRandomPool rand;
	Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
	
	RootNetwork::IdentityDatabase identityDatabase;
	RootNetwork::PrivateIdentity privateIdentity(privateKey);
	
	printf("My id is '%s'.\n", privateIdentity.id().hexString().c_str());
	
	// Sign all outgoing packets and verify incoming packets.
	RootNetwork::AuthenticatedSocket authenticatedSocket(identityDatabase, privateIdentity, packetSocket);
	
	// Multiplex messages for processing incoming requests in
	// one thread while performing outgoing requests in another.
	MultiplexHost<RootNetwork::Endpoint, RootNetwork::Message> multiplexHost(authenticatedSocket);
	MultiplexClient<RootNetwork::Endpoint, RootNetwork::Message> eventSocket(multiplexHost);
	MultiplexClient<RootNetwork::Endpoint, RootNetwork::Message> rpcSocket(multiplexHost);
	
	// Routine ID generator.
	RootNetwork::RoutineIdGenerator routineIdGenerator;
	
	EventThread eventThreadRunnable(eventSocket, routineIdGenerator);
	Thread eventThread(eventThreadRunnable);
	
	RootNetwork::RPCService service(rpcSocket, routineIdGenerator);
	
	const auto peerId = service.identifyEndpoint(UDP::Endpoint(IP::V4Address::Localhost(), otherPort));
	
	printf("Peer's id is '%s'.\n", peerId.hexString().c_str());
	
	const auto endpoint = service.pingNode(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), RootNetwork::NodeId());
	
	printf("My endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
	
	const auto networks = service.queryNetworks(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), RootNetwork::NodeId());
	
	printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
	
	for (size_t i = 0; i < networks.size(); i++) {
		printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
		if (networks.at(i) == RootNetwork::NetworkId::Generate("test")) {
			printf("         -> Supports 'test' network.\n");
		}
	}
	
	return 0;
}

