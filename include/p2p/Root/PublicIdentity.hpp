#ifndef P2P_ROOTNETWORK_PUBLICIDENTITY_HPP
#define P2P_ROOTNETWORK_PUBLICIDENTITY_HPP

#include <stdint.h>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Crypt/RandomPool.hpp>

#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>
#include <p2p/Root/PrivateIdentity.hpp>

namespace p2p {

	namespace Root {
	
		class ROOT_OPENP2P_API PublicIdentity {
			public:
				PublicIdentity(const PublicKey&, uint64_t packetCount = 0);
				
				PublicIdentity(Crypt::RandomPool& randomPool, const PrivateIdentity&);
				
				uint64_t nextPacketCount() const;
				
				const PublicKey& publicKey() const;
				
				NodeId id() const;
				
				bool verify(const Packet&, const PacketSignature&);
				
			private:
				PublicKey publicKey_;
				uint64_t nextPacketCount_;
				
		};
		
	}
	
}

#endif
