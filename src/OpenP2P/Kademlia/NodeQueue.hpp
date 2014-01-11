#ifndef OPENP2P_KADEMLIA_NODEQUEUE_HPP
#define OPENP2P_KADEMLIA_NODEQUEUE_HPP

#include <cstddef>
#include <queue>
#include <map>
#include <boost/optional.hpp>

#include <OpenP2P/Kademlia/Id.hpp>
#include <OpenP2P/Kademlia/Node.hpp>

namespace OpenP2P {

	namespace Kademlia {
	
		template <class EndpointType, size_t IdSize>
		class NodeQueue {
				typedef Id<IdSize> IdType;
				typedef Node<EndpointType, IdSize> NodeType;
				
				struct Compare {
					const IdType& id_;
					
					Compare(const IdType& id) : id_(id) { }
					
					bool operator()(const NodeType& a, const NodeType& b) const {
						const IdType& aId = a.id;
						const IdType& bId = b.id;
						
						const IdType& aDiff = aId ^ id_;
						const IdType& bDiff = bId ^ id_;
						
						if (aDiff == bDiff) {
							return aId < bId;
						} else {
							return aDiff < bDiff;
						}
					}
				};
				
			public:
				NodeQueue(const IdType& targetId) : targetId_(targetId), map_(Compare(targetId_)),
					// Initial distance of the nearest node is maximum
					distance_(IdType::Max()) { }
					
				IdType distance() const {
					return distance_;
				}
				
				size_t size() const {
					return map_.size();
				}
				
				void add(const std::vector<NodeType>& group) {
					for (typename std::vector<NodeType>::const_iterator i = group.begin(); i != group.end(); ++i) {
						map_.insert(std::pair<NodeType, bool>(*i, false));
					}
					
					// Attempt to reduce the distance of the nearest node
					if (!map_.empty()) {
						distance_ = map_.begin()->first.id ^ targetId_;
					}
				}
				
				bool isNearestVisited() {
					return map_.empty() ? true : (map_.begin())->second;
				}
				
				std::vector<NodeType> getNearest(size_t maxSize) {
					std::vector<NodeType> nearestUnvisited;
					
					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end() && nearestUnvisited.size() < maxSize; ++i) {
						nearestUnvisited.push_back(i->first);
					}
					
					return nearestUnvisited;
				}
				
				std::vector<NodeType> getNearestUnvisited(size_t maxSize) {
					std::vector<NodeType> nearestUnvisited;
					
					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end() && nearestUnvisited.size() < maxSize; ++i) {
						if (!i->second) {
							//Set to visited
							i->second = true;
							nearestUnvisited.push_back(i->first);
						}
					}
					
					return nearestUnvisited;
				}
				
			private:
				IdType targetId_;
				std::map<NodeType, bool, Compare> map_;
				IdType distance_;
				
		};
		
	}
	
}

#endif
