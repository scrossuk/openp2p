#include <stdint.h>

#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>
#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>
#include <OpenP2P/Crypt/ECDSA/SignStream.hpp>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		PrivateIdentity::PrivateIdentity(const PrivateKey& pPrivateKey, uint64_t pPacketCount)
			: privateKey_(pPrivateKey), nextPacketCount_(pPacketCount) { }
			
		PacketSignature PrivateIdentity::sign(const Packet& packet) {
			// Advance packet count to prevent replays.
			nextPacketCount_++;
			
			Crypt::AutoSeededRandomPool rand;
			Crypt::ECDSA::SignStream signStream(rand, privateKey_);
			BinaryOStream binStream(signStream);
			WritePacket(binStream, packet);
			
			PacketSignature sig;
			sig.signature = signStream.signature();
			sig.publicKey = Crypt::ECDSA::PublicKey(rand, privateKey_);
			
			assert(sig.signature.size() == SIGNATURE_SIZE_BYTES);
			return sig;
		}
		
	}
	
}

