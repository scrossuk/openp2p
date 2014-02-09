#ifndef OPENP2P_ROOTNETWORK_PUBLICIDENTITY_HPP
#define OPENP2P_ROOTNETWORK_PUBLICIDENTITY_HPP

#include <stdint.h>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/Packet.hpp>
#include <OpenP2P/RootNetwork/PrivateIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class PublicIdentity {
			public:
				PublicIdentity(const PublicKey&, uint64_t packetCount = 0);
				
				PublicIdentity(const PrivateIdentity&);
				
				inline uint64_t nextPacketCount() const {
					return nextPacketCount_;
				}
				
				inline const PublicKey& publicKey() const {
					return publicKey_;
				}
				
				bool verify(const Packet&, const PacketSignature&);
				
			private:
				PublicKey publicKey_;
				uint64_t nextPacketCount_;
				
		};
		
	}
	
}

#endif
