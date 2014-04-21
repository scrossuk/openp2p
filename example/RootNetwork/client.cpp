#include <stdint.h>
#include <stdio.h>

#include <p2p.hpp>
#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/MultiplexSocket.hpp>
#include <p2p/Root.hpp>
#include <p2p/UDP.hpp>

using namespace p2p;

class EventThread: public Runnable {
	public:
		EventThread(Socket<Root::Endpoint, Root::Message>& socket, Root::RoutineIdGenerator& routineIdGenerator)
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
		Root::Core::RPCService service_;
	
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
	Root::TransportSocket transportSocket(udpSocket);
	
	// Serialize/unserialize packets.
	Root::PacketSocket packetSocket(transportSocket);
	
	// Generate a private key for our node.
	Crypt::AutoSeededRandomPool rand;
	Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
	
	Root::NodeDatabase nodeDatabase;
	Root::PrivateIdentity privateIdentity(privateKey);
	
	printf("My id is '%s'.\n", privateIdentity.id().hexString().c_str());
	
	// Sign all outgoing packets and verify incoming packets.
	Root::AuthenticatedSocket authenticatedSocket(nodeDatabase, privateIdentity, packetSocket);
	
	// Multiplex messages for processing incoming requests in
	// one thread while performing outgoing requests in another.
	MultiplexHost<Root::Endpoint, Root::Message> multiplexHost(authenticatedSocket);
	MultiplexClient<Root::Endpoint, Root::Message> eventSocket(multiplexHost);
	MultiplexClient<Root::Endpoint, Root::Message> rpcSocket(multiplexHost);
	
	// Routine ID generator.
	Root::RoutineIdGenerator routineIdGenerator;
	
	EventThread eventThreadRunnable(eventSocket, routineIdGenerator);
	Thread eventThread(eventThreadRunnable);
	
	Root::Core::RPCService service(rpcSocket, routineIdGenerator);
	
	const auto peerId = service.identifyEndpoint(UDP::Endpoint(IP::V4Address::Localhost(), otherPort));
	
	printf("Peer's id is '%s'.\n", peerId.hexString().c_str());
	
	const auto endpoint = service.pingNode(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), Root::NodeId());
	
	printf("My endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
	
	const auto networks = service.queryNetworks(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), Root::NodeId());
	
	printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
	
	for (size_t i = 0; i < networks.size(); i++) {
		printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
		if (networks.at(i) == Root::NetworkId::Generate("test")) {
			printf("         -> Supports 'test' network.\n");
		}
	}
	
	return 0;
}

