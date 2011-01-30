#ifndef OPENP2P_KADEMLIA_BUCKETSET_HPP
#define OPENP2P_KADEMLIA_BUCKETSET_HPP

#include <cstddef>
#include <list>
#include <OpenP2P/Kademlia/Id.hpp>
#include <OpenP2P/Kademlia/Node.hpp>
#include <OpenP2P/Kademlia/NodeGroup.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <class EndpointType, std::size_t IdSize, std::size_t MaxBucketSize = 20>
		class BucketSet{
			typedef Id<IdSize> IdType;
			typedef Node<EndpointType, IdSize> NodeType;
			typedef NodeGroup<EndpointType, IdSize> GroupType;

			public:
				BucketSet(const IdType& id) : id_(id){ }

				void add(const NodeType& node){
					if(id_ == node.id){
						return;
					}

					std::size_t index = getBucket(node.id);

					std::list<NodeType>& bucket = buckets_[index];
					for(typename std::list<NodeType>::iterator p = bucket.begin(); p != bucket.end(); ++p){
						if(p->id == node.id){
							bucket.erase(p);
							bucket.push_front(node);
							return;
						}
					}

					if(bucket.size() >= MaxBucketSize){
						bucket.pop_back();
					}

					bucket.push_front(node);
				}

				std::size_t getBucket(const IdType& id){
					for(std::size_t i = 0; i < IdSize; ++i){
						uint8_t distance = id_.data[i] ^ id.data[i];
						if(distance != 0){
							for(uint8_t j = 0, m = (1 << 7); j < 8; ++j, m >>= 1){
								if(distance & m){
									return (i << 3) | j;
								}
							}
						}
					}

					return (IdSize << 3) - 1;
				}

				GroupType nearest(const IdType& id, std::size_t number){
					std::size_t index = getBucket(id);
					typename std::list<NodeType>::iterator p;
					GroupType group;

					for(std::size_t dist = 0; dist < IdSize && group.size() < number; dist++){
						std::size_t l = index - dist;
						if(l >= 0){
							std::list<NodeType>& lbucket = buckets_[l];
							for(p = lbucket.begin(); p != lbucket.end() && group.size() < number; ++p){
								group.add(*p);
							}
						}


						std::size_t r = index + dist + 1;
						if(r < IdSize){
							std::list<NodeType>& rbucket = buckets_[r];
							for(p = rbucket.begin(); p != rbucket.end() && group.size() < number; ++p){
								group.add(*p);
							}
						}
					}

					return group;
				}

			private:
				Id<IdSize> id_;
				std::list<NodeType> buckets_[IdSize << 3];

		};

	}

}

#endif
