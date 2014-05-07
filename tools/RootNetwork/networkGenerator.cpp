#include <stdint.h>
#include <stdio.h>

#include <string>

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

class NodeThread: public p2p::Runnable {
	public:
		NodeThread(unsigned short myPort, unsigned short otherPort, Logger& logger)
			: myPort_(myPort), udpSocket_(myPort), otherPort_(otherPort), logger_(logger) { }
		
		NodeThread(NodeThread&&) = default;
		NodeThread& operator=(NodeThread&&) = default;
		
		void run() {
			// Generate private key.
			Crypt::AutoSeededRandomPool rand;
			Crypt::ECDSA::PrivateKey privateKey(rand, Crypt::ECDSA::brainpoolP256r1);
			
			Root::NodeDatabase nodeDatabase;
			Root::PrivateIdentity privateIdentity(rand, privateKey);
			
			// Send/receive data on appropriate transport.
			Root::TransportSocket transportSocket(udpSocket_);
			
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
			
			QueryNodeThread queryNodeThreadRunnable(logger_, privateIdentity.id(), dhtBucketSet, coreService, dhtService, messageQueue);
			Thread queryNodeThread(queryNodeThreadRunnable);
			
			const auto peerEndpoint = UDP::Endpoint(IP::V4Address::Localhost(), otherPort_);
			
			logger_.log(STR("Querying peer at port %llu...", (unsigned long long) otherPort_));
			
			auto getPeerRPC = coreService.identify(peerEndpoint);
			bool hasProcessedPeer = false;
			
			Event::Timer timer;
			timer.setMilliseconds(2000.0);
			timer.schedule();
			
			while (!signal_.isActive()) {
				while (coreService.processMessage() || dhtService.processMessage()) { }
				
				if (getPeerRPC.isComplete() && !hasProcessedPeer) {
					const auto peerId = getPeerRPC.get();
					logger_.log(STR("Got peer ID: %s", peerId.hexString().c_str()));
					messageQueue.send(Root::NodePair(peerId, peerEndpoint));
					hasProcessedPeer = true;
				}
				
				if (!hasProcessedPeer && timer.hasExpired()) {
					logger_.log(STR("Failed to receive IDENTIFY reply for node at port %llu from node at port %llu...",
						(unsigned long long) myPort_, (unsigned long long) otherPort_));
					abort();
				}
				
				Event::Wait({ coreService.eventSource(), dhtService.eventSource(), signal_.eventSource(), timer.eventSource() });
			}
			
			logger_.log(STR("Exiting node thread for node at port %llu.", (unsigned long long) myPort_));
		}
		
		void cancel() {
			logger_.log(STR("Cancelled node thread for node at port %llu.", (unsigned long long) myPort_));
			signal_.activate();
		}
		
	private:
		NodeThread(const NodeThread&) = delete;
		NodeThread& operator=(const NodeThread&) = delete;
		
		unsigned short myPort_;
		UDP::Socket udpSocket_;
		unsigned short otherPort_;
		Logger& logger_;
		Event::Signal signal_;
		
};

int main(int argc, char** argv) {
	Logger logger;
	
	if (argc != 3) {
		logger.log("Usage: networkGenerator [start port] [end port]");
		return -1;
	}
	
	const auto startPort = atoi(argv[1]);
	const auto endPort = atoi(argv[2]);
	
	std::vector<NodeThread> nodeRunnables;
	std::vector<p2p::Thread> nodeThreads;
	
	for (unsigned short port = startPort; port <= endPort; port++) {
		logger.log(STR("Creating node at port %u.", (unsigned int) port));
		const auto nextPort = (port == endPort) ? startPort : (port + 1);
		nodeRunnables.push_back(NodeThread(port, nextPort, logger));
	}
	
	for (size_t i = 0; i < nodeRunnables.size(); i++) {
		nodeThreads.push_back(p2p::Thread(nodeRunnables.at(i)));
	}
	
	logger.log("Press 'q' to quit.");
	
	while (true) {
		logger.showPrompt();
		
		const auto rawCommand = readLine();
		if (rawCommand.empty()) break;
		
		if (rawCommand == "q") break;
	}
	
	return 0;
}

