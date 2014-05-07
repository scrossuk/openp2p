#include <stdint.h>
#include <stdio.h>

#include <p2p/Concurrency.hpp>
#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/Event/Signal.hpp>
#include <p2p/Event/Wait.hpp>
#include <p2p/Kademlia.hpp>
#include <p2p/Root.hpp>
#include <p2p/Transport.hpp>
#include <p2p/UDP.hpp>

#include "Logger.hpp"

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

class KademliaRPCSocket: public p2p::Kademlia::RPCSocket<Root::NodeId> {
	public:
		KademliaRPCSocket(Root::DHT::Service& dhtService, Logger& logger)
			: dhtService_(dhtService), logger_(logger) { }
		
		RPC::Operation<std::vector<Root::NodeId>> performFind(const Root::NodeId& destId, const Root::NodeId& searchId) {
			logger_.log(STR("Kademlia querying node '%s'.", destId.hexString().c_str()));
			return RPC::Chain(dhtService_.getNearestNodes(destId, searchId),
				[] (std::vector<Root::NodeInfo> nodeList) {
					std::vector<Root::NodeId> idList;
					for (const auto& nodeInfo: nodeList) {
						idList.push_back(nodeInfo.id);
					}
					return idList;
				});
		}
		
	private:
		Root::DHT::Service& dhtService_;
		Logger& logger_;
		
};

class QueryNodeThread: public Runnable {
	public:
		QueryNodeThread(Logger& logger,
				const Root::NodeId& myId, p2p::Kademlia::BucketSet<Root::NodeId>& dhtBucketSet,
				Root::Core::Service& coreService, Root::DHT::Service& dhtService,
				MessageQueue<Root::NodePair>& messageQueue)
			: logger_(logger),
			myId_(myId), dhtBucketSet_(dhtBucketSet),
			coreService_(coreService), dhtService_(dhtService),
			messageQueue_(messageQueue) { }
		
		void run() {
			std::set<Root::NodeId> knownNodes;
			
			while (!signal_.isActive()) {
				while (!messageQueue_.empty()) {
					const auto nodePair = messageQueue_.receive();
					const auto& peerId = nodePair.id;
					const auto& peerEndpoint = nodePair.endpoint;
					
					if (myId_ == peerId) {
						// Node is current node.
						continue;
					}
					
					if (knownNodes.find(peerId) != knownNodes.end()) {
						// Node already queried.
						continue;
					}
					
					logger_.log(STR("--- Querying node '%s'...", peerId.hexString().c_str()));
					
					knownNodes.insert(peerId);
					
					const auto endpoint = coreService_.ping(peerEndpoint, peerId).get();
					
					logger_.log(STR("Node reports my endpoint is '%s'.", endpoint.udpEndpoint.toString().c_str()));
					// TODO: add this endpoint to our set of endpoints.
					
					const auto networks = coreService_.queryNetworks(peerEndpoint, peerId).get();
					
					logger_.log(STR("Node supports %llu networks.", (unsigned long long) networks.size()));
					
					bool supportsDHT = false;
					for (size_t i = 0; i < networks.size(); i++) {
						logger_.log(STR("    Network %llu: %s.", (unsigned long long) i, networks.at(i).hexString().c_str()));
						if (networks.at(i) == Root::NetworkId::Generate("p2p.rootdht")) {
							logger_.log("        -> Supports DHT network.");
							supportsDHT = true;
						}
					}
					
					if (!supportsDHT) {
						logger_.log("Node doesn't support DHT.");
						continue;
					}
					
					dhtBucketSet_.add(peerId);
					
					const auto peerNearestNodes = dhtService_.getNearestNodes(peerId, myId_).get();
					
					if (peerNearestNodes.empty()) {
						logger_.log("Node doesn't seem to know any other nodes.");
						continue;
					}
					
					logger_.log("Node reports our nearest nodes as:");
					
					for (const auto& dhtNode: peerNearestNodes) {
						logger_.log(STR("    Node '%s'", dhtNode.id.hexString().c_str()));
						if (dhtNode.endpointSet.empty()) continue;
						
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
		Logger& logger_;
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

class NodeDetectDelegate: public Root::NodeDetectDelegate {
	public:
		NodeDetectDelegate(Root::NodeDatabase& nodeDatabase, MessageQueue<Root::NodePair>& messageQueue)
			: nodeDatabase_(nodeDatabase), messageQueue_(messageQueue) { }
		
		void detectedNodePair(const Root::NodePair& nodePair) {
			if (!nodeDatabase_.isKnownId(nodePair.id)) return;
			
			if (nodeDatabase_.nodeEntry(nodePair.id).endpointSet.empty()) {
				messageQueue_.send(nodePair);
			}
		}
		
	private:
		Root::NodeDatabase& nodeDatabase_;
		MessageQueue<Root::NodePair>& messageQueue_;
	
};

Root::NodeId processTextId(const std::string& textId) {
	assert(textId.size() <= 64);
	assert((textId.size() % 2) == 0);
	
	Root::NodeId nodeId;
	
	for (size_t i = 0; i < (textId.size() / 2); i++) {
		const auto subString = textId.substr(i * 2, 2);
		const auto value = strtol(subString.c_str(), nullptr, 16);
		nodeId[i] = value;
	}
	
	return nodeId;
}

std::vector<std::string> parse(const std::string& args);

int main(int argc, char** argv) {
	Logger logger;
	
	if (argc != 2) {
		logger.log("Usage: rootNetworkClient [my port]");
		return -1;
	}
	
	const auto myPort = atoi(argv[1]);
	
	UDP::Socket udpSocket(myPort);
	
	// Generate a private key for our node.
	Crypt::AutoSeededRandomPool rand;
	Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
	
	Root::NodeDatabase nodeDatabase;
	Root::PrivateIdentity privateIdentity(rand, privateKey);
	
	logger.log("========");
	logger.log(STR("======== My id is '%s'.", privateIdentity.id().hexString().c_str()));
	logger.log("========");
	
	// Send/receive data on appropriate transport.
	Root::TransportSocket transportSocket(udpSocket);
	
	// Serialize/unserialize packets.
	Root::PacketSocket packetSocket(transportSocket);
	
	// Sign all outgoing packets and verify incoming packets.
	ClientIdentityDelegate identityDelegate(nodeDatabase, privateIdentity);
	Root::AuthenticatedSocket authenticatedSocket(identityDelegate, packetSocket);
	
	MessageQueue<Root::NodePair> messageQueue;
	
	NodeDetectDelegate nodeDetectDelegate(nodeDatabase, messageQueue);
	Root::NodeDetectSocket nodeDetectSocket(authenticatedSocket, nodeDetectDelegate);
	
	// Multiplex messages for core and DHT services.
	MultiplexHost<Root::NodePair, Root::Message> multiplexHost(nodeDetectSocket);
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
	
	QueryNodeThread queryNodeThreadRunnable(logger, privateIdentity.id(), dhtBucketSet, coreService, dhtService, messageQueue);
	Thread queryNodeThread(queryNodeThreadRunnable);
	
	while (true) {
		logger.showPrompt();
		
		const auto rawCommand = readLine();
		if (rawCommand.empty()) break;
		
		const auto commandArgs = parse(rawCommand);
		if (commandArgs.empty()) {
			logger.log("error: Command not specified.");
			continue;
		}
		
		const auto& command = commandArgs.at(0);
		
		if (command == "add") {
			if (commandArgs.size() != 2) {
				logger.log(STR("%s: incorrect number of arguments", command.c_str()));
				continue;
			}
			
			const auto nodePort = atoi(commandArgs.at(1).c_str());
			logger.log(STR("%s: querying for node at port '%d'...", command.c_str(), nodePort));
			
			const auto peerEndpoint = UDP::Endpoint(IP::V4Address::Localhost(), nodePort);
			
			const auto peerId = coreService.identify(peerEndpoint).get();
			
			logger.log(STR("%s: node's id is '%s'.", command.c_str(), peerId.hexString().c_str()));
			
			messageQueue.send(Root::NodePair(peerId, peerEndpoint));
			
			logger.log(STR("%s: submitted node to be queried", command.c_str()));
		} else if (command == "find") {
			if (commandArgs.size() != 2) {
				logger.log(STR("%s: incorrect number of arguments", command.c_str()));
				continue;
			}
			
			const auto& searchIdText = commandArgs.at(1);
			if (searchIdText.size() > 64) {
				logger.log(STR("%s: search id '%s' is longer than 64 characters", command.c_str(), searchIdText.c_str()));
				continue;
			}
			
			if ((searchIdText.size() % 2) == 1) {
				logger.log(STR("%s: search id '%s' is not a multiple of 2 characters in length", command.c_str(), searchIdText.c_str()));
				continue;
			}
			
			const auto searchId = processTextId(searchIdText);
			
			logger.log(STR("%s: searching for ID '%s'...", command.c_str(), searchId.hexString().c_str()));
			
			KademliaRPCSocket kademliaRPCSocket(dhtService, logger);
			const auto nearestList = p2p::Kademlia::iterativeSearch<Root::NodeId, 2>(dhtBucketSet, kademliaRPCSocket, searchId);
			
			logger.log(STR("%s: found %llu nearest nodes:", command.c_str(), (unsigned long long) nearestList.size()));
			
			for (const auto& resultNodeId: nearestList) {
				logger.log(STR("%s:     Node '%s'", command.c_str(), resultNodeId.hexString().c_str()));
			}
		} else if (command == "ls" || command == "list") {
			for (const auto& nodeEntryPair: nodeDatabase.map()) {
				const auto& nodeEntry = nodeEntryPair.second;
				logger.log(STR("%s:     Node '%s' ->", command.c_str(), nodeEntry.identity.id().hexString().c_str()));
				for (const auto& endpoint: nodeEntry.endpointSet) {
					logger.log(STR("%s:         %s", command.c_str(), endpoint.toString().c_str()));
				}
			}
		} else if (command == "q" || command == "quit") {
			logger.log(STR("%s: exiting...", command.c_str()));
			break;
		} else if (command == "h" || command == "help") {
			logger.log(STR("%s: available commands:", command.c_str()));
			logger.log(STR("%s:     add (a) [port]: Add a node by its UDP port.", command.c_str()));
			logger.log(STR("%s:     help (h): Display this help text.", command.c_str()));
			logger.log(STR("%s:     quit (q): Exit application.", command.c_str()));
		} else {
			logger.log(STR("%s: command not found", command.c_str()));
		}
	}
	
	return 0;
}

