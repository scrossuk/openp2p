#ifndef OPENP2P_ROOTNETWORK_PRIVATEIDENTITY_HPP
#define OPENP2P_ROOTNETWORK_PRIVATEIDENTITY_HPP

#include <stdint.h>

#include <OpenP2P/Crypt/ECDSA/PrivateKey.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>

namespace OpenP2P{

	namespace RootNetwork{
	
		struct PacketSignature{
			Buffer signature;
			Crypt::ECDSA::PublicKey publicKey;
		};

		class PrivateIdentity{
			public:
				PrivateIdentity(const Crypt::ECDSA::PrivateKey&, uint64_t packetCount = 0);

				inline uint64_t packetCount(){
					return packetCount_;
				}
				
				inline Crypt::ECDSA::PrivateKey privateKey(){
					return privateKey_;
				}

				PacketSignature sign(const Packet&);

			private:
				Crypt::ECDSA::PrivateKey privateKey_;
				uint64_t packetCount_;

		};

	}

}

#endif
