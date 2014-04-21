#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include <p2p/Buffer.hpp>

#include <p2p/Stream/BinaryStream.hpp>

#include <p2p/Event/Source.hpp>
#include <p2p/Event/Wait.hpp>

#include <p2p/Root/AuthenticatedSocket.hpp>
#include <p2p/Root/Endpoint.hpp>
#include <p2p/Root/Message.hpp>
#include <p2p/Root/NodeDatabase.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>

namespace p2p {

	namespace Root {
	
		AuthenticatedSocket::AuthenticatedSocket(NodeDatabase& nodeDatabase, PrivateIdentity& privateIdentity, Socket<Endpoint, SignedPacket>& socket)
			: nodeDatabase_(nodeDatabase),
			privateIdentity_(privateIdentity),
			socket_(socket) { }
			
		bool AuthenticatedSocket::isValid() const {
			return socket_.isValid();
		}
		
		Event::Source AuthenticatedSocket::eventSource() const {
			return socket_.eventSource();
		}
		
		namespace {
			
			PublicIdentity& getIdentity(NodeDatabase& nodeDatabase, const PublicKey& key) {
				const auto nodeId = NodeId::Generate(key);
				if (!nodeDatabase.isKnownId(nodeId)) {
					nodeDatabase.addNode(nodeId, NodeInfo(PublicIdentity(key, 0)));
				}
				
				return nodeDatabase.nodeInfo(nodeId).identity;
			}
			
		}
		
		bool AuthenticatedSocket::receive(Endpoint& endpoint, Message& message) {
			SignedPacket signedPacket;
			
			if (!socket_.receive(endpoint, signedPacket)) {
				return false;
			}
			
			const auto& packet = signedPacket.packet;
			
			auto& publicIdentity = getIdentity(nodeDatabase_, signedPacket.signature.publicKey);
			if (!publicIdentity.verify(packet, signedPacket.signature)) {
				return false;
			}
			
			message.sourceId = publicIdentity.id();
			message.destinationId = packet.header.destinationId;
			message.isError = packet.header.err;
			message.subnetwork = packet.header.sub ? boost::make_optional(packet.header.subnetworkId) : boost::none;
			message.type = packet.header.type;
			message.routine = packet.header.routine;
			message.routineState = packet.header.state;
			message.payload = packet.payload;
			return true;
		}
		
		bool AuthenticatedSocket::send(const Endpoint& endpoint, const Message& message) {
			SignedPacket signedPacket;
			auto& packet = signedPacket.packet;
			packet.header.version = VERSION_1;
			packet.header.state = message.routineState;
			packet.header.err = message.isError;
			packet.header.sub = message.subnetwork;
			packet.header.type = message.type;
			packet.header.length = message.payload.size();
			packet.header.routine = message.routine;
			packet.header.messageCounter = privateIdentity_.nextPacketCount();
			packet.header.destinationId = message.destinationId;
			packet.header.subnetworkId = message.subnetwork ? *(message.subnetwork) : NetworkId();
			
			packet.payload = message.payload;
			
			signedPacket.signature = privateIdentity_.sign(signedPacket.packet);
			return socket_.send(endpoint, signedPacket);
		}
		
	}
	
}

