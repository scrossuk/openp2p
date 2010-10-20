#ifndef OPENP2P_DHT_BUCKETSET_HPP
#define OPENP2P_DHT_BUCKETSET_HPP

#include <list>
#include "Hash.hpp"
#include "Node.hpp"

namespace openp2p{

	namespace DHT{

		class NodeGroup;

		const unsigned int BucketSize = 20;

		class BucketSet{
			public:
				BucketSet(Hash);

				void Add(Node);

				unsigned int GetBucketIndex(Hash);

				NodeGroup GetNearest(Hash, unsigned int);

			private:
				Hash mHash;
				std::list<Node> mBuckets[Hash::SizeInBits];

		};

	}

}

#endif
