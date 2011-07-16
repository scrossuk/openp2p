#include <stdint.h>
#include <OpenP2P/Crypt/AutoSeededRandomPool.hpp>
#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>
#include <OpenP2P/Crypt/ECDSA/PublicKey.hpp>
#include <OpenP2P/Crypt/ECDSA/SignStream.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		PrivateIdentity::PrivateIdentity(const Crypt::ECDSA::PrivateKey& privateKey, uint64_t packetCount = 0)
			: privateKey_(privateKey), packetCount_(packetCount){ }

		PacketSignature PrivateIdentity::sign(const Packet& packet){
			Crypt::AutoSeededRandomPool rand;
			Crypt::ECDSA::SignStream signStream(rand, privateKey_);
			BinaryOStream binStream(signStream);
			binStream << packet << packetCount_;
			
			PacketSignature sig;
			sig.packetCount = packetCount_;
			sig.signature = signStream.signature();
			sig.publicKey = Crypt::ECDSA::PublicKey(privateKey_);
			return sig;
		}

	}

}

