#include <unordered_map>

#include <OpenP2P/RootNetwork/Key.hpp>
#include <OpenP2P/RootNetwork/IdentityDatabase.hpp>
#include <OpenP2P/RootNetwork/NodeId.hpp>
#include <OpenP2P/RootNetwork/PublicIdentity.hpp>

namespace OpenP2P {

	namespace RootNetwork {
		
		IdentityDatabase::IdentityDatabase() { }
		
		PublicIdentity& IdentityDatabase::getIdentity(const PublicKey& key) {
			const auto nodeId = NodeId::Generate(key);
			const auto iterator = identityMap_.find(nodeId);
			if (iterator != identityMap_.end()) {
				return iterator->second;
			}
			
			const auto newIterator = identityMap_.emplace(nodeId, PublicIdentity(key, 0)).first;
			return newIterator->second;
		}
		
	}
	
}

