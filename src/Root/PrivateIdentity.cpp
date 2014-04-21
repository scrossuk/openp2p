#include <stdint.h>

#include <p2p/Crypt/AutoSeededRandomPool.hpp>
#include <p2p/Crypt/ECDSA/SignStream.hpp>

#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PrivateIdentity.hpp>

namespace p2p {

	namespace Root {
	
		PrivateIdentity::PrivateIdentity(const PrivateKey& pPrivateKey, uint64_t pPacketCount)
			: privateKey_(pPrivateKey), nextPacketCount_(pPacketCount) { }
		
		uint64_t PrivateIdentity::nextPacketCount() const {
			return nextPacketCount_;
		}
		
		const PrivateKey& PrivateIdentity::privateKey() const {
			return privateKey_;
		}
		
		NodeId PrivateIdentity::id() const {
			Crypt::AutoSeededRandomPool rand;
			return NodeId::Generate(PublicKey(rand, privateKey_));
		}
		
		PacketSignature PrivateIdentity::sign(const Packet& packet) {
			// Advance packet count to prevent replays.
			nextPacketCount_++;
			
			Crypt::AutoSeededRandomPool rand;
			Crypt::ECDSA::SignStream signStream(rand, privateKey_);
			BinaryOStream binStream(signStream);
			WritePacket(binStream, packet);
			
			PacketSignature sig;
			sig.signature = signStream.signature();
			sig.publicKey = PublicKey(rand, privateKey_);
			
			assert(sig.signature.size() == SIGNATURE_SIZE_BYTES);
			return sig;
		}
		
	}
	
}

