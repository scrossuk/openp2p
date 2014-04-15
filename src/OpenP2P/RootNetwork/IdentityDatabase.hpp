#ifndef OPENP2P_ROOTNETWORK_IDENTITYDATABASE_HPP
#define OPENP2P_ROOTNETWORK_IDENTITYDATABASE_HPP

#include <unordered_map>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
	
		class IdentityDatabase {
			public:
				IdentityDatabase();
				
				PublicIdentity& getIdentity(const PublicKey& key);
				
			private:
				// Non-copyable.
				IdentityDatabase(const IdentityDatabase&) = delete;
				IdentityDatabase& operator=(const IdentityDatabase&) = delete;
				
				std::unordered_map<NodeId, PublicIdentity> identityMap_;
				
		};
		
	}
	
}

#endif
