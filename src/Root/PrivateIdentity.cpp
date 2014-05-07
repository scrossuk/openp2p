#include <stdint.h>

#include <p2p/Crypt/RandomPool.hpp>
#include <p2p/Crypt/ECDSA/SignStream.hpp>

#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PrivateIdentity.hpp>

namespace p2p {

	namespace Root {
	
		PrivateIdentity::PrivateIdentity(Crypt::RandomPool& randomPool, const PrivateKey& pPrivateKey, uint64_t pPacketCount)
			: randomPool_(randomPool), privateKey_(pPrivateKey), nextPacketCount_(pPacketCount) { }
		
		uint64_t PrivateIdentity::nextPacketCount() const {
			return nextPacketCount_;
		}
		
		const PrivateKey& PrivateIdentity::privateKey() const {
			return privateKey_;
		}
		
		NodeId PrivateIdentity::id() const {
			return NodeId::Generate(PublicKey(randomPool_, privateKey_));
		}
		
		PacketSignature PrivateIdentity::sign(const Packet& packet) {
			// Advance packet count to prevent replays.
			nextPacketCount_++;
			
			Crypt::ECDSA::SignStream signStream(randomPool_, privateKey_);
			BinaryOStream binStream(signStream);
			WritePacket(binStream, packet);
			
			PacketSignature sig;
			sig.signature = signStream.signature();
			sig.publicKey = PublicKey(randomPool_, privateKey_);
			
			assert(sig.signature.size() == SIGNATURE_SIZE_BYTES);
			return sig;
		}
		
	}
	
}

