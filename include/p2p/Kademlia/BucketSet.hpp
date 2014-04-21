#ifndef P2P_KADEMLIA_BUCKETSET_HPP
#define P2P_KADEMLIA_BUCKETSET_HPP

#include <array>
#include <list>
#include <mutex>
#include <vector>

namespace p2p {

	namespace Kademlia {
	
		template <typename IdType, size_t MAX_BUCKET_SIZE = 20>
		class BucketSet {
			public:
				BucketSet(const IdType& id) : id_(id) { }
				
				const IdType& getId() const {
					return id_;
				}
				
				void add(const IdType& id) {
					if (id_ == id) {
						return;
					}
					
					std::lock_guard<std::mutex> lock(mutex_);
					
					const size_t index = getBucket(id);
					
					auto& bucket = buckets_.at(index);
					
					for (const auto& nodeId: bucket) {
						if (nodeId == id) {
							return;
						}
					}
					
					if (bucket.size() >= MAX_BUCKET_SIZE) {
						bucket.pop_back();
					}
					
					bucket.push_front(id);
				}
				
				std::vector<NodeType> getNearest(const IdType& id, size_t maxGroupSize) const {
					std::lock_guard<std::mutex> lock(mutex_);
					
					const size_t index = getBucket(id);
					std::vector<NodeType> group;
					
					for (size_t dist = 0; dist < IdSize && group.size() < maxGroupSize; dist++) {
						if (index >= dist) {
							for (const auto& nodeId: buckets_.at(index - dist)) {
								if (group.size() >= maxGroupSize) {
									break;
								}
								group.push_back(nodeId);
							}
						}
						
						const auto rightPosition = index + dist + 1;
						if (rightPosition < IdType::SIZE_IN_BITS) {
							for (const auto& nodeId: buckets_[rightPosition]) {
								if (group.size() >= maxGroupSize) {
									break;
								}
								group.push_back(nodeId);
							}
						}
					}
					
					return group;
				}
				
			private:
				size_t getBucket(const IdType& id) const {
					for (size_t i = 0; i < IdType::SIZE_IN_BITS; ++i) {
						if (id_.bitAt(i) != id.bitAt(i)) {
							return i;
						}
					}
					
					return IdType::SIZE_IN_BITS - 1;
				}
				
				mutable std::mutex mutex_;
				IdType id_;
				std::array<std::list<IdType>, IdType::SIZE_IN_BITS> buckets_;
				
		};
		
	}
	
}

#endif
