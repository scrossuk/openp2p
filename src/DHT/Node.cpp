#include "Node.hpp"
#include "Hash.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace DHT{

		Node Node::Unserialize(BufferIterator& iterator){
			Hash hash = Hash::Unserialize(iterator);
			Endpoint endpoint = Endpoint::Unserialize(iterator);

			return Node(hash, endpoint);
		}

		Node::Node(Hash hash, Endpoint endpoint) : mHash(hash), mEndpoint(endpoint){ }

		Node::Node(const Node& node) : mHash(node.GetHash()), mEndpoint(node.GetEndpoint()){ }

		bool Node::operator==(const Node& node) const{
			return mHash == node.GetHash();
		}

		bool Node::operator<(const Node& node) const{
			return mHash < node.GetHash();
		}

		Hash Node::GetHash() const{
			return mHash;
		}

		Endpoint Node::GetEndpoint() const{
			return mEndpoint;
		}

		Buffer Node::Serialize() const{
			return mHash.Serialize() + mEndpoint.Serialize();
		}

	}

}


