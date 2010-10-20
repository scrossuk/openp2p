#include "../Core.hpp"
#include "Hash.hpp"
#include "Node.hpp"
#include "NodeGroup.hpp"
#include "BucketSet.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace DHT{

		BucketSet::BucketSet(Hash hash) : mHash(hash){ }

		void BucketSet::Add(Node node){
			if(mHash == node.GetHash()){
				return;
			}

			unsigned int index = GetBucketIndex(node.GetHash());

			std::list<Node>& bucket = mBuckets[index];
			for(std::list<Node>::iterator p = bucket.begin(); p != bucket.end(); ++p){
				if(*p == node){
					bucket.erase(p);
					bucket.push_front(node);
					return;
				}
			}

			if(bucket.size() >= BucketSize){
				bucket.pop_back();
			}

			bucket.push_front(node);
		}

		unsigned int BucketSet::GetBucketIndex(Hash hash){
			const Uint8 * mData = mHash.Data();
			const Uint8 * data = hash.Data();
			for(unsigned int i = 0; i < Hash::SizeInBytes; ++i){
				Uint8 distance = mData[i] ^ data[i];
				if(distance != 0){
					for(Uint8 j = 0, m = (1 << 7); j < 8; ++j, m >>= 1){
						if(distance & m){
							return (i * 8) + j;
						}
					}
				}
			}

			return Hash::SizeInBits - 1;
		}

		NodeGroup BucketSet::GetNearest(Hash hash, unsigned int number){
			unsigned int index = GetBucketIndex(hash);
			std::list<Node>::iterator p;
			NodeGroup group(mHash);

			for(unsigned int dist = 0; dist < Hash::SizeInBits && group.Size() < number; dist++){
				if(index >= dist){
					std::list<Node>& lbucket = mBuckets[index - dist];
					for(p = lbucket.begin(); p != lbucket.end() && group.Size() < number; ++p){
						group.Add(*p);
					}
				}

				if((index + dist + 1) < Hash::SizeInBits){
					std::list<Node>& rbucket = mBuckets[index + dist + 1];
					for(p = rbucket.begin(); p != rbucket.end() && group.Size() < number; ++p){
						group.Add(*p);
					}
				}
			}

			return group;
		}

	}

}


