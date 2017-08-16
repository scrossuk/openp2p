#ifndef P2P_ROOTNETWORK_PRIVATEIDENTITY_HPP
#define P2P_ROOTNETWORK_PRIVATEIDENTITY_HPP

#include <stdint.h>

#include <p2p/Root/Root_api.hpp>
#include <p2p/Crypt/RandomPool.hpp>

#include <p2p/Root/Key.hpp>
#include <p2p/Root/NodeId.hpp>
#include <p2p/Root/Packet.hpp>

namespace p2p {

	namespace Root {
	
		class ROOT_OPENP2P_API PrivateIdentity {
			public:
				PrivateIdentity(Crypt::RandomPool& randomPool, const PrivateKey&, uint64_t packetCount = 0);
				
				uint64_t nextPacketCount() const;
				
				const PrivateKey& privateKey() const;
				
				NodeId id() const;
				
				PacketSignature sign(const Packet&);
				
			private:
				Crypt::RandomPool& randomPool_;
				PrivateKey privateKey_;
				uint64_t nextPacketCount_;
				
		};
		
	}
	
}

#endif
