#ifndef OPENP2P_KADEMLIA_BUCKETSET_HPP
#define OPENP2P_KADEMLIA_BUCKETSET_HPP

#include <cstddef>
#include <list>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Kademlia/Id.hpp>
#include <OpenP2P/Kademlia/Node.hpp>

namespace OpenP2P {

	namespace Kademlia {
	
		template <class EndpointType, size_t IdSize, size_t MaxBucketSize = 20>
		class BucketSet {
				typedef Id<IdSize> IdType;
				typedef Node<EndpointType, IdSize> NodeType;
				typedef std::vector<NodeType> GroupType;
				typedef typename GroupType::iterator GroupIteratorType;
				
			public:
				BucketSet(const IdType& id) : id_(id) { }
				
				IdType getId() const {
					return id_;
				}
				
				void add(const NodeType& node) {
					if (id_ == node.id) {
						return;
					}
					
					Lock lock(mutex_);
					size_t index = getBucket(node.id);
					
					std::list<NodeType>& bucket = buckets_[index];
					
					for (typename std::list<NodeType>::iterator p = bucket.begin(); p != bucket.end(); ++p) {
						if (p->id == node.id) {
							bucket.erase(p);
							bucket.push_front(node);
							return;
						}
					}
					
					if (bucket.size() >= MaxBucketSize) {
						bucket.pop_back();
					}
					
					bucket.push_front(node);
				}
				
				GroupType getNearest(const IdType& id, size_t number) {
					Lock lock(mutex_);
					size_t index = getBucket(id);
					GroupType group;
					
					for (size_t dist = 0; dist < IdSize && group.size() < number; dist++) {
						size_t l = index - dist;
						
						if (l >= 0) {
							const std::list<NodeType>& lbucket = buckets_[l];
							
							for (typename std::list<NodeType>::const_iterator p = lbucket.begin(); p != lbucket.end() && group.size() < number; ++p) {
								group.push_back(*p);
							}
						}
						
						
						size_t r = index + dist + 1;
						
						if (r < IdSize) {
							const std::list<NodeType>& rbucket = buckets_[r];
							
							for (typename std::list<NodeType>::const_iterator p = rbucket.begin(); p != rbucket.end() && group.size() < number; ++p) {
								group.push_back(*p);
							}
						}
					}
					
					return group;
				}
				
			private:
				size_t getBucket(const IdType& id) const {
					for (size_t i = 0; i < IdSize; ++i) {
						uint8_t distance = id_.data[i] ^ id.data[i];
						
						if (distance != 0) {
							for (uint8_t j = 0, m = (1 << 7); j < 8; ++j, m >>= 1) {
								if (distance & m) {
									return (i << 3) | j;
								}
							}
						}
					}
					
					return (IdSize << 3) - 1;
				}
				
				Mutex mutex_;
				const IdType id_;
				std::list<NodeType> buckets_[IdSize << 3];
				
		};
		
	}
	
}

#endif
