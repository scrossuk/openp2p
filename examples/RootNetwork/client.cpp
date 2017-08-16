#include <stdint.h>
#include <stdio.h>

#include <p2p/Concurrency/Thread.hpp>
#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/Event/Signal.hpp>
#include <p2p/Event/Wait.hpp>
#include <p2p/Transport/MultiplexSocket.hpp>
#include <p2p/Root.hpp>
#include <p2p/UDP.hpp>

using namespace p2p;

class EventThread: public Runnable {
	public:
		EventThread(Root::Core::Service& coreService, Root::DHT::Service& dhtService)
			: coreService_(coreService), dhtService_(dhtService) { }
		
		void run() {
			while (!signal_.isActive()) {
				while (coreService_.processMessage() || dhtService_.processMessage()) { }
				Event::Wait({ coreService_.eventSource(), dhtService_.eventSource(), signal_.eventSource() });
			}
		}
		
		void cancel() {
			signal_.activate();
		}
		
	private:
		Root::Core::Service& coreService_;
		Root::DHT::Service& dhtService_;
		Event::Signal signal_;
	
};

class DHTServerDelegate: public Root::DHT::ServerDelegate {
	public:
		std::vector<Root::NodeInfo> getNearestNodes(const Root::NodeId& targetId) {
			printf("Received request for nearest nodes to ID '%s'.\n", targetId.hexString().c_str());
			// Return a fake node...
			return { Root::NodeInfo(targetId, { UDP::Endpoint(IP::V4Address::Localhost(), 10000) }) };
		}
		
		void subscribe(const Root::NodeId& targetId, const Root::NodeInfo& nodeInfo) {
			(void) targetId;
			(void) nodeInfo;
			printf("Received subscribe request for ID '%s'.\n", targetId.hexString().c_str());
		}
		
		std::vector<Root::NodeInfo> getSubscribers(const Root::NodeId& targetId) {
			(void) targetId;
			printf("Received request for subscribers to ID '%s'.\n", targetId.hexString().c_str());
			return {};
		}
		
};

class ClientIdentityDelegate: public Root::IdentityDelegate {
	public:
		ClientIdentityDelegate(Root::NodeDatabase& nodeDatabase, Root::PrivateIdentity& privateIdentity)
			: nodeDatabase_(nodeDatabase), privateIdentity_(privateIdentity) { }
		
		Root::PrivateIdentity& getPrivateIdentity() {
			return privateIdentity_;
		}
		
		Root::PublicIdentity& getPublicIdentity(const Root::PublicKey& key) {
			const auto nodeId = Root::NodeId::Generate(key);
			if (!nodeDatabase_.isKnownId(nodeId)) {
				nodeDatabase_.addNode(nodeId, Root::NodeEntry(Root::PublicIdentity(key, 0)));
			}
			return nodeDatabase_.nodeEntry(nodeId).identity;
		}
		
	private:
		Root::NodeDatabase& nodeDatabase_;
		Root::PrivateIdentity& privateIdentity_;
		
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
	Root::PrivateIdentity privateIdentity(rand, privateKey);
	
	printf("My id is '%s'.\n", privateIdentity.id().hexString().c_str());
	
	// Sign all outgoing packets and verify incoming packets.
	ClientIdentityDelegate identityDelegate(nodeDatabase, privateIdentity);
	Root::AuthenticatedSocket authenticatedSocket(identityDelegate, packetSocket);
	
	// Multiplex messages for processing incoming requests in
	// one thread while performing outgoing requests in another.
	MultiplexHost<Root::NodePair, Root::Message> multiplexHost(authenticatedSocket);
	MultiplexClient<Root::NodePair, Root::Message> coreSocket(multiplexHost);
	MultiplexClient<Root::NodePair, Root::Message> dhtMultiplexSocket(multiplexHost);
	
	Root::EndpointMapSocket dhtSocket(dhtMultiplexSocket, nodeDatabase);
	
	// Routine ID generator.
	Root::RoutineIdGenerator routineIdGenerator;
	
	Root::Core::Service coreService(coreSocket, routineIdGenerator);
	coreService.addNetwork("p2p.rootdht");
	
	DHTServerDelegate dhtDelegate;
	Root::DHT::Service dhtService(dhtSocket, routineIdGenerator, dhtDelegate);
	
	EventThread eventThreadRunnable(coreService, dhtService);
	Thread eventThread(eventThreadRunnable);
	
	const auto peerId = coreService.identify(UDP::Endpoint(IP::V4Address::Localhost(), otherPort)).get();
	
	printf("Peer's id is '%s'.\n", peerId.hexString().c_str());
	
	const auto endpoint = coreService.ping(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), peerId).get();
	
	printf("My endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
	
	const auto networks = coreService.queryNetworks(UDP::Endpoint(IP::V4Address::Localhost(), otherPort), peerId).get();
	
	printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
	
	bool supportsDHT = false;
	
	for (size_t i = 0; i < networks.size(); i++) {
		printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
		if (networks.at(i) == Root::NetworkId::Generate("p2p.rootdht")) {
			printf("         -> Supports DHT network.\n");
			supportsDHT = true;
		}
	}
	
	if (!supportsDHT) {
		printf("Node does not support DHT...\n");
		return 0;
	}
	
	const auto myNearestNodes = dhtService.getNearestNodes(peerId, privateIdentity.id()).get();
	printf("Peer reports %llu nearest nodes.\n", (unsigned long long) myNearestNodes.size());
	
	return 0;
}

