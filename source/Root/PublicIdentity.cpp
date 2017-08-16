#include <stdint.h>

#include <p2p/Crypt/RandomPool.hpp>
#include <p2p/Crypt/ECDSA/PublicKey.hpp>
#include <p2p/Crypt/ECDSA/VerifyStream.hpp>

#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PrivateIdentity.hpp>
#include <p2p/Root/PublicIdentity.hpp>

namespace p2p {

	namespace Root {
	
		PublicIdentity::PublicIdentity(const PublicKey& pPublicKey, uint64_t packetCount)
			: publicKey_(pPublicKey), nextPacketCount_(packetCount) { }
			
		PublicIdentity::PublicIdentity(Crypt::RandomPool& randomPool, const PrivateIdentity& identity)
			: nextPacketCount_(identity.nextPacketCount()) {
				publicKey_ = PublicKey(randomPool, identity.privateKey());
			}
		
		uint64_t PublicIdentity::nextPacketCount() const {
			return nextPacketCount_;
		}
		
		const PublicKey& PublicIdentity::publicKey() const {
			return publicKey_;
		}
		
		NodeId PublicIdentity::id() const {
			return NodeId::Generate(publicKey_);
		}
		
		bool PublicIdentity::verify(const Packet& packet, const PacketSignature& sig) {
			assert(sig.signature.size() == SIGNATURE_SIZE_BYTES);
			
			if (packet.header.messageCounter < nextPacketCount_) {
				return false;
			}
			
			nextPacketCount_ = packet.header.messageCounter + 1;
			
			Crypt::ECDSA::VerifyStream verifyStream(publicKey_, sig.signature);
			BinaryOStream binStream(verifyStream);
			WritePacket(binStream, packet);
			
			return verifyStream.isSignatureValid();
		}
		
	}
	
}

