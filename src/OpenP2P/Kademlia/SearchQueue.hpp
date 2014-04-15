#ifndef OPENP2P_KADEMLIA_SEARCHQUEUE_HPP
#define OPENP2P_KADEMLIA_SEARCHQUEUE_HPP

#include <cstddef>
#include <queue>
#include <map>
#include <boost/optional.hpp>

#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P {

	namespace Kademlia {
	
		template <class EndpointType, size_t ID_SIZE>
		class SearchQueue {
				typedef Id<ID_SIZE> IdType;
				
				struct Compare {
					const IdType& id_;
					
					Compare(const IdType& id) : id_(id) { }
					
					bool operator()(const IdType& a, const IdType& b) const {
						const auto& aDiff = a ^ id_;
						const auto& bDiff = b ^ id_;
						
						if (aDiff == bDiff) {
							return a < b;
						} else {
							return aDiff < bDiff;
						}
					}
				};
				
			public:
				SearchQueue(const IdType& targetId) : targetId_(targetId), map_(Compare(targetId_)) { }
					
				IdType distance() const {
					return map_.empty() ? IdType::Max() : map_.begin()->first ^ targetId_;
				}
				
				size_t size() const {
					return map_.size();
				}
				
				void add(const IdType& id) {
					map_.insert(id);
				}
				
				bool isNearestVisited() const {
					return map_.empty() ? true : (map_.begin())->second;
				}
				
				void setVisited(const IdType& id) {
					map_.at(id) = true;
				}
				
				std::vector<IdType> getNearest(size_t maxSize) const {
					std::vector<IdType> group;
					
					for (const auto& idPair: map_) {
						if (group.size() == maxSize) break;
						group.push_back(idPair.first);
					}
					
					return group;
				}
				
				std::vector<IdType> getNearestUnvisited(size_t maxSize) const {
					std::vector<IdType> group;
					
					for (const auto& idPair: map_) {
						if (group.size() == maxSize) break;
						
						if (!idPair.second) {
							group.push_back(idPair.first);
						}
					}
					
					return group;
				}
				
			private:
				IdType targetId_;
				std::map<IdType, bool, Compare> map_;
				
		};
		
	}
	
}

#endif
