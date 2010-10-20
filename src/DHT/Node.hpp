#ifndef OPENP2P_DHT_NODE_HPP
#define OPENP2P_DHT_NODE_HPP

#include "../Core.hpp"
#include "Hash.hpp"

namespace openp2p{

	namespace DHT{

		class Hash;

		class Node{
			public:
				static Node Unserialize(Core::BufferIterator&);

				Node(Hash, Core::Endpoint);
				Node(const Node&);

				bool operator==(const Node&) const;
				bool operator<(const Node&) const;

				Hash GetHash() const;

				Core::Endpoint GetEndpoint() const;

				Core::Buffer Serialize() const;

			private:
				Hash mHash;

				Core::Endpoint mEndpoint;

		};

	}

}

#endif
