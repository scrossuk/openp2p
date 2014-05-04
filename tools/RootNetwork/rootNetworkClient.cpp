#include <stdint.h>
#include <stdio.h>

#include <p2p.hpp>
#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/Event/Signal.hpp>
#include <p2p/Event/Wait.hpp>
#include <p2p/Kademlia.hpp>
#include <p2p/MultiplexSocket.hpp>
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
		DHTServerDelegate(p2p::Kademlia::BucketSet<Root::NodeId>& dhtBucketSet, p2p::Root::NodeDatabase& nodeDatabase)
			: dhtBucketSet_(dhtBucketSet), nodeDatabase_(nodeDatabase) { }
		
		std::vector<Root::NodeInfo> getNearestNodes(const Root::NodeId& targetId) {
			const auto idList = dhtBucketSet_.getNearest(targetId);
			std::vector<Root::NodeInfo> nodeList;
			for (const auto& id: idList) {
				nodeList.push_back(nodeDatabase_.nodeEntry(id).toNodeInfo());
			}
			return nodeList;
		}
		
		void subscribe(const Root::NodeId& targetId, const Root::NodeInfo& nodeInfo) {
			(void) targetId;
			(void) nodeInfo;
		}
		
		std::vector<Root::NodeInfo> getSubscribers(const Root::NodeId& targetId) {
			(void) targetId;
			return {};
		}
		
	private:
		p2p::Kademlia::BucketSet<Root::NodeId>& dhtBucketSet_;
		p2p::Root::NodeDatabase& nodeDatabase_;
		
};

std::string readLine() {
	std::string line;
	
	while (true) {
		const int c = getc(stdin);
		if (c == EOF) {
			return "";
		}
		
		if (c == '\n') {
			if (!line.empty()) {
				break;
			} else {
				continue;
			}
		}
		
		line += (char) c;
	}
	
	return line;
}

std::vector<std::string> parse(const std::string& args);

int main(int argc, char** argv) {
	if (argc != 2) {
		printf("Usage: rootNetworkClient [my port]\n");
		return -1;
	}
	
	const auto myPort = atoi(argv[1]);
	
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
	
	// Multiplex messages for core and DHT services.
	MultiplexHost<std::pair<Root::Endpoint, Root::NodeId>, Root::Message> multiplexHost(authenticatedSocket);
	MultiplexClient<std::pair<Root::Endpoint, Root::NodeId>, Root::Message> coreSocket(multiplexHost);
	MultiplexClient<std::pair<Root::Endpoint, Root::NodeId>, Root::Message> dhtMultiplexSocket(multiplexHost);
	
	Root::EndpointMapSocket dhtSocket(dhtMultiplexSocket, nodeDatabase);
	
	// Routine ID generator.
	Root::RoutineIdGenerator routineIdGenerator;
	
	Root::Core::Service coreService(coreSocket, routineIdGenerator);
	coreService.addNetwork("p2p.rootdht");
	
	p2p::Kademlia::BucketSet<Root::NodeId> dhtBucketSet(privateIdentity.id());
	
	DHTServerDelegate dhtDelegate(dhtBucketSet, nodeDatabase);
	Root::DHT::Service dhtService(dhtSocket, routineIdGenerator, dhtDelegate);
	
	EventThread eventThreadRunnable(coreService, dhtService);
	Thread eventThread(eventThreadRunnable);
	
	std::set<Root::NodeId> knownNodes;
	
	while (true) {
		printf("$ ");
		fflush(stdout);
		
		const auto rawCommand = readLine();
		if (rawCommand.empty()) break;
		
		printf("\r");
		fflush(stdout);
		
		const auto commandArgs = parse(rawCommand);
		if (commandArgs.empty()) {
			printf("error: Command not specified.\n");
			continue;
		}
		
		const auto& command = commandArgs.at(0);
		
		if (command == "a" || command == "add") {
			if (commandArgs.size() != 2) {
				printf("%s: incorrect number of arguments\n", command.c_str());
				continue;
			}
			
			const auto nodePort = atoi(commandArgs.at(1).c_str());
			printf("%s: querying for node at port '%d'...\n", command.c_str(), nodePort);
			
			const auto nodeEndpoint = UDP::Endpoint(IP::V4Address::Localhost(), nodePort);
			
			const auto peerId = coreService.identify(nodeEndpoint).wait();
			
			printf("%s: node's id is '%s'.\n", command.c_str(), peerId.hexString().c_str());
			
			if (knownNodes.find(peerId) != knownNodes.end()) {
				printf("%s: we already know about this node...\n", command.c_str());
				continue;
			}
			
			knownNodes.insert(peerId);
			
			const auto endpoint = coreService.ping(nodeEndpoint, peerId).wait();
			
			printf("%s: node reports my endpoint is '%s'.\n", command.c_str(), endpoint.udpEndpoint.toString().c_str());
			
			const auto networks = coreService.queryNetworks(nodeEndpoint, peerId).wait();
			
			printf("%s: node supports %llu networks.\n", command.c_str(), (unsigned long long) networks.size());
			
			bool supportsDHT = false;
			for (size_t i = 0; i < networks.size(); i++) {
				printf("%s:    Network %llu: %s.\n", command.c_str(), (unsigned long long) i, networks.at(i).hexString().c_str());
				if (networks.at(i) == Root::NetworkId::Generate("p2p.rootdht")) {
					printf("%s:         -> Supports DHT network.\n", command.c_str());
					supportsDHT = true;
				}
			}
			
			if (!supportsDHT) {
				printf("%s: Node doesn't support DHT.\n", command.c_str());
				continue;
			}
			
			dhtBucketSet.add(peerId);
			
			const auto peerNearestNodes = dhtService.getNearestNodes(peerId, privateIdentity.id()).wait();
			
			printf("%s: peer's nearest nodes are:\n", command.c_str());
			
			for (const auto& dhtNode: peerNearestNodes) {
				printf("%s:     Node '%s'\n", command.c_str(), dhtNode.id.hexString().c_str());
			}
		} else if (command == "q" || command == "quit") {
			printf("%s: exiting...\n", command.c_str());
			break;
		} else if (command == "h" || command == "help") {
			printf("%s: available commands:\n", command.c_str());
			printf("%s:     add (a) [port]: Add a node by its UDP port.\n", command.c_str());
			printf("%s:     help (h): Display this help text.\n", command.c_str());
			printf("%s:     quit (q): Exit application.\n", command.c_str());
		} else {
			printf("%s: command not found\n", command.c_str());
		}
	}
	
	return 0;
}

