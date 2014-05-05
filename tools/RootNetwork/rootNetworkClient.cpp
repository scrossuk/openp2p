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

template <typename T>
class MessageQueue {
	public:
		MessageQueue() { }
		
		Event::Source eventSource() const {
			return signal_.eventSource();
		}
		
		bool empty() const {
			std::lock_guard<std::mutex> lock(mutex_);
			return queue_.empty();
		}
		
		T receive() {
			std::lock_guard<std::mutex> lock(mutex_);
			auto message = std::move(queue_.front());
			queue_.pop();
			return message;
		}
		
		void send(T message) {
			std::lock_guard<std::mutex> lock(mutex_);
			queue_.push(std::move(message));
			signal_.activate();
		}
		
	private:
		mutable std::mutex mutex_;
		std::queue<T> queue_;
		Event::Signal signal_;
		
};

class QueryNodeThread: public Runnable {
	public:
		QueryNodeThread(const Root::NodeId& myId, p2p::Kademlia::BucketSet<Root::NodeId>& dhtBucketSet,
				Root::Core::Service& coreService, Root::DHT::Service& dhtService,
				MessageQueue<Root::NodePair>& messageQueue)
			: myId_(myId), dhtBucketSet_(dhtBucketSet),
			coreService_(coreService), dhtService_(dhtService),
			messageQueue_(messageQueue) { }
		
		void run() {
			std::set<Root::NodeId> knownNodes;
			
			while (!signal_.isActive()) {
				while (!messageQueue_.empty()) {
					const auto nodePair = messageQueue_.receive();
					const auto& peerId = nodePair.id;
					const auto& peerEndpoint = nodePair.endpoint;
					
					printf("Querying node '%s'...\n", peerId.hexString().c_str());
					
					if (myId_ == peerId) {
						printf("That's me! I'm not going to query myself...\n");
						continue;
					}
					
					if (knownNodes.find(peerId) != knownNodes.end()) {
						printf("Node already queried.\n");
						continue;
					}
					
					knownNodes.insert(peerId);
					
					const auto endpoint = coreService_.ping(peerEndpoint, peerId).wait();
					
					printf("Node reports my endpoint is '%s'.\n", endpoint.udpEndpoint.toString().c_str());
					// TODO: add this endpoint to our set of endpoints.
					
					const auto networks = coreService_.queryNetworks(peerEndpoint, peerId).wait();
					
					printf("Node supports %llu networks.\n", (unsigned long long) networks.size());
					
					bool supportsDHT = false;
					for (size_t i = 0; i < networks.size(); i++) {
						printf("    Network %llu: %s.\n", (unsigned long long) i, networks.at(i).hexString().c_str());
						if (networks.at(i) == Root::NetworkId::Generate("p2p.rootdht")) {
							printf("        -> Supports DHT network.\n");
							supportsDHT = true;
						}
					}
					
					if (!supportsDHT) {
						printf("Node doesn't support DHT.\n");
						continue;
					}
					
					dhtBucketSet_.add(peerId);
					
					const auto peerNearestNodes = dhtService_.getNearestNodes(peerId, myId_).wait();
					
					if (peerNearestNodes.empty()) {
						printf("Node doesn't seem to know any other nodes.\n");
						continue;
					}
					
					printf("Node reports our nearest nodes as:\n");
					
					for (const auto& dhtNode: peerNearestNodes) {
						printf("    Node '%s'\n", dhtNode.id.hexString().c_str());
						assert(!dhtNode.endpointSet.empty());
						
						// Query all our nearest nodes.
						messageQueue_.send(Root::NodePair(dhtNode.id, *(dhtNode.endpointSet.begin())));
					}
				}
				
				Event::Wait({ messageQueue_.eventSource(), signal_.eventSource() });
			}
		}
		
		void cancel() {
			signal_.activate();
		}
		
	private:
		Root::NodeId myId_;
		p2p::Kademlia::BucketSet<Root::NodeId>& dhtBucketSet_;
		Root::Core::Service& coreService_;
		Root::DHT::Service& dhtService_;
		MessageQueue<Root::NodePair>& messageQueue_;
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

class InterceptSocket: public p2p::Socket<Root::NodePair, Root::Message> {
	public:
		InterceptSocket(Root::NodeDatabase& nodeDatabase, MessageQueue<Root::NodePair>& messageQueue, p2p::Socket<Root::NodePair, Root::Message>& socket)
			: nodeDatabase_(nodeDatabase), messageQueue_(messageQueue), socket_(socket) { }
		
		bool isValid() const {
			return socket_.isValid();
		}
		
		Event::Source eventSource() const {
			return socket_.eventSource();
		}
		
		bool receive(Root::NodePair& nodePair, Root::Message& message) {
			const bool result = socket_.receive(nodePair, message);
			if (!result) return false;
			
			if (nodeDatabase_.isKnownId(nodePair.id)) {
				if (nodeDatabase_.nodeEntry(nodePair.id).endpointSet.empty()) {
					messageQueue_.send(nodePair);
				}
			}
			
			return result;
		}
		
		bool send(const Root::NodePair& nodePair, const Root::Message& message) {
			return socket_.send(nodePair, message);
		}
		
	private:
		Root::NodeDatabase& nodeDatabase_;
		MessageQueue<Root::NodePair>& messageQueue_;
		p2p::Socket<Root::NodePair, Root::Message>& socket_;
		
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
	
	ClientIdentityDelegate identityDelegate(nodeDatabase, privateIdentity);
	
	// Sign all outgoing packets and verify incoming packets.
	Root::AuthenticatedSocket authenticatedSocket(identityDelegate, packetSocket);
	
	MessageQueue<Root::NodePair> messageQueue;
	
	InterceptSocket interceptSocket(nodeDatabase, messageQueue, authenticatedSocket);
	
	// Multiplex messages for core and DHT services.
	MultiplexHost<Root::NodePair, Root::Message> multiplexHost(interceptSocket);
	MultiplexClient<Root::NodePair, Root::Message> coreSocket(multiplexHost);
	MultiplexClient<Root::NodePair, Root::Message> dhtMultiplexSocket(multiplexHost);
	
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
	
	QueryNodeThread queryNodeThreadRunnable(privateIdentity.id(), dhtBucketSet, coreService, dhtService, messageQueue);
	Thread queryNodeThread(queryNodeThreadRunnable);
	
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
			
			const auto peerEndpoint = UDP::Endpoint(IP::V4Address::Localhost(), nodePort);
			
			const auto peerId = coreService.identify(peerEndpoint).wait();
			
			printf("%s: node's id is '%s'.\n", command.c_str(), peerId.hexString().c_str());
			
			messageQueue.send(Root::NodePair(peerId, peerEndpoint));
			
			printf("%s: submitted node to be queried\n", command.c_str());
		} else if (command == "ls" || command == "list") {
			for (const auto& nodeEntryPair: nodeDatabase.map()) {
				const auto& nodeEntry = nodeEntryPair.second;
				printf("%s:     Node '%s'.\n", command.c_str(), nodeEntry.identity.id().hexString().c_str());
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

