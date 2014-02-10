#ifndef OPENP2P_ROOTNETWORK_SIGNEDPACKET_HPP
#define OPENP2P_ROOTNETWORK_SIGNEDPACKET_HPP

#include <OpenP2P/Buffer.hpp>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		struct PacketSignature {
			Buffer signature;
			PublicKey publicKey;
		};
		
		inline PacketSignature ReadSignature(BlockingReader& reader) {
			PacketSignature sig;
			sig.signature.resize(SIGNATURE_SIZE_BYTES);
			reader.readAll(sig.signature.data(), sig.signature.size());
		}
		
		struct SignedPacket {
			Packet packet;
			PacketSignature signature;
		};
		
	}
	
}

#endif
