#include <stdint.h>
#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>
#include <OpenP2P/Crypt/ECDSA/VerifyStream.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		PublicIdentity::PublicIdentity(const PublicKey& publicKey, uint64_t packetCount = 0)
			: publicKey_(publicKey), packetCount_(packetCount) { }
			
		PublicIdentity::PublicIdentity(const PrivateIdentity& identity)
			: publicKey_(identity.privateKey()), packetCount_(identity.packetCount()) { }
			
		bool PublicIdentity::verify(const Packet& packet, const PacketSignature& sig) {
			if (sig.packetCount < packetCount_) {
				return false;
			}
			
			Crypt::ECDSA::VerifyStream verifyStream(publicKey_, sig.signature);
			BinaryOStream binStream(verifyStream);
			binStream << packet << sig.packetCount;
			
			return verifyStream.isValid();
		}
		
	}
	
}

