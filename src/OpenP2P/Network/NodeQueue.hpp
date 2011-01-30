#ifndef OPENP2P_KADEMLIA_NODEQUEUE_HPP
#define OPENP2P_KADEMLIA_NODEQUEUE_HPP

#include <cstddef>
#include <queue>
#include <set>
#include <vector>
#include <boost/optional.hpp>
#include <boost/thread.hpp>

namespace OpenP2P {

	namespace Kademlia {

		template <class EndpointType, std::size_t IdSize>
		class NodeQueue {
				typedef Id<IdSize> IdType;
				typedef Node<EndpointType, IdSize> NodeType;
				typedef NodeGroup<EndpointType, IdSize> GroupType;
				typedef typename GroupType::Iterator GroupIteratorType;

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
						}
						else {
							return aDiff < bDiff;
						}
					}
				};

			public:
				NodeQueue(const IdType& id) : id_(id), map_(Compare(id)),
						// Initial distance of the nearest node is maximum
						distance_(MaxId<IdSize>()) { }

				IdType distance() const {
					return distance_;
				}

				void push(const GroupType& group) {
					boost::lock_guard<boost::mutex> lock(mutex_);

					for (GroupIteratorType i = group.iterator(); i.isValid(); i++) {
						map_.insert(std::pair<NodeType, bool>(*i, false));
					}

					// Attempt to reduce the distance of the nearest node
					if (!map_.empty()) {
						distance_ = map_.begin()->first.id ^ id_;
					}
				}

				boost::optional<NodeType> pop() {
					boost::lock_guard<boost::mutex> lock(mutex_);

					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end(); ++i) {
						if (!i->second){
							i->second = true;
							return boost::make_optional(i->first);
						}
					}

					return boost::optional<NodeType>();
				}

				GroupType group() {
					GroupType group;

					for (typename std::map<NodeType, bool, Compare>::iterator i = map_.begin(); i != map_.end(); ++i) {
						group.add(i->first);
					}

					return group;
				}

			private:
				boost::mutex mutex_;
				IdType id_;
				std::map<NodeType, bool, Compare> map_;
				IdType distance_;

		};

	}

}

#endif
