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

		template <class EndpointType, std::size_t IdSize>
		class NodeQueue {
				typedef Id<IdSize> IdType;
				typedef Node<EndpointType, IdSize> NodeType;
				typedef std::vector<NodeType> GroupType;
				typedef typename GroupType::iterator GroupIteratorType;

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
				NodeQueue(const IdType& targetId) : targetId_(targetId), map_(Compare(id)),
						// Initial distance of the nearest node is maximum
						distance_(maxId<IdSize>()) { }

				IdType distance() const {
					return distance_;
				}

				std::size_t size() const{
					return map_.size();
				}

				void add(const GroupType& group) {
					for (GroupIteratorType i = group.begin(); i != group.end(); ++i) {
						map_.insert(std::pair<NodeType, bool>(*i, false));
					}

					// Attempt to reduce the distance of the nearest node
					if (!map_.empty()) {
						distance_ = map_.begin()->first.id ^ targetId_;
					}
				}

				bool isNearestVisited(){
					if(map_.empty()) return true;
					return (map_.begin())->second;
				}

				boost::optional<NodeType> getNearest(){
					
				}

				//Looks for an unvisited node, and returns one if it is found, setting it as visited
				boost::optional<NodeType> getNearestUnvisited() {
					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end(); ++i) {
						if (!i->second){
							//Set to visited
							i->second = true;
							return boost::make_optional(i->first);
						}
					}

					//All nodes have been visited
					return boost::optional<NodeType>();
				}

				template <std::size_t N>
				void getNearestFixedGroup(boost::array<NodeType, N>& group){
					std::size_t c = 0;
					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end() && c < N; ++i, ++c) {
						group[c] = i->first;
					}
				}

				GroupType getNearestGroup(){
					GroupType group;
					std::size_t c = 0;
					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end() && c < group.size(); ++i, ++c) {
						group[c] = i->first;
					}
				}

			private:
				IdType targetId_;
				std::map<NodeType, bool, Compare> map_;
				IdType distance_;

		};

	}

}

#endif
