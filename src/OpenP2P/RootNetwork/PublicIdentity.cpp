#include <stdint.h>

#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>
#include <OpenP2P/Crypt/ECDSA/VerifyStream.hpp>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		PublicIdentity::PublicIdentity(const PublicKey& pPublicKey, uint64_t packetCount)
			: publicKey_(pPublicKey), nextPacketCount_(packetCount) { }
			
		PublicIdentity::PublicIdentity(const PrivateIdentity& identity)
			: nextPacketCount_(identity.nextPacketCount()) {
				Crypt::AutoSeededRandomPool rand;
				publicKey_ = PublicKey(rand, identity.privateKey());
			}
			
		bool PublicIdentity::verify(const Packet& packet, const PacketSignature& sig) {
			assert(sig.signature.size() == SIGNATURE_SIZE_BYTES);
			
			if (packet.header.messageCounter < nextPacketCount_) {
				return false;
			}
			
			nextPacketCount_++;
			
			Crypt::ECDSA::VerifyStream verifyStream(publicKey_, sig.signature);
			BinaryOStream binStream(verifyStream);
			WritePacket(binStream, packet);
			
			return verifyStream.isSignatureValid();
		}
		
	}
	
}

