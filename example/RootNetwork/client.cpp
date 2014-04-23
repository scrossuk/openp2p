#include <stdint.h>
#include <stdio.h>

#include <p2p.hpp>
#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/Event/Signal.hpp>
#include <p2p/Event/Wait.hpp>
#include <p2p/MultiplexSocket.hpp>
#include <p2p/Root.hpp>
#include <p2p/UDP.hpp>

using namespace p2p;

class EventThread: public Runnable {
	public:
		EventThread(Root::Core::RPCClient& client, Root::Core::RPCServer& server)
			: client_(client), server_(server) { }
		
		void run() {
			while (!signal_.isActive()) {
				while (client_.processResponse() || server_.processRequest()) { }
				Event::Wait({ client_.eventSource(), server_.eventSource(), signal_.eventSource() });
			}
		}
		
		void cancel() {
			signal_.activate();
		}
		
	private:
		Root::Core::RPCClient& client_;
		Root::Core::RPCServer& server_;
		Event::Signal signal_;
	
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
	MultiplexHost<std::pair<Root::Endpoint, Root::NodeId>, Root::Message> multiplexHost(authenticatedSocket);
	MultiplexClient<std::pair<Root::Endpoint, Root::NodeId>, Root::Message> eventSocket(multiplexHost);
	MultiplexClient<std::pair<Root::Endpoint, Root::NodeId>, Root::Message> rpcSocket(multiplexHost);
	
	Root::Core::RPCServer server(eventSocket);
	server.addNetwork("test");
	server.addNetwork("p2p.rootdht");
	
	// Routine ID generator.
	Root::RoutineIdGenerator routineIdGenerator;
	Root::Core::RPCClient client(rpcSocket, routineIdGenerator);
	
	EventThread eventThreadRunnable(client, server);
	Thread eventThread(eventThreadRunnable);
	
	const auto peerId = client.identify(UDP::Endpoint(IP::V4Address::Localhost(), otherPort)).wait();
	
	printf("Peer's id is '%s'.\n", peerId.hexString().c_str());
	
	const auto endpoint = client.ping(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), peerId).wait();
	
	printf("My endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
	
	const auto networks = client.queryNetworks(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), peerId).wait();
	
	printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
	
	for (size_t i = 0; i < networks.size(); i++) {
		printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
		if (networks.at(i) == Root::NetworkId::Generate("test")) {
			printf("         -> Supports Test network.\n");
		} else if (networks.at(i) == Root::NetworkId::Generate("p2p.rootdht")) {
			printf("         -> Supports DHT network.\n");
		}
	}
	
	return 0;
}

