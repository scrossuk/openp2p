#ifndef OPENP2P_KADEMLIA_NODEGROUP_HPP
#define OPENP2P_KADEMLIA_NODEGROUP_HPP

#include <stdint.h>
#include <cstddef>
#include <map>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Kademlia/Id.hpp>
#include <OpenP2P/Kademlia/Node.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <class EndpointType, std::size_t IdSize>
		class NodeGroup{
			public:
				typedef Id<IdSize> IdType;
				typedef Node<EndpointType, IdSize> NodeType;
				typedef NodeGroup<EndpointType, IdSize> GroupType;
				typedef std::vector<NodeType> ListType;
				typedef typename ListType::const_iterator IteratorType;

				class Iterator{
					public:
						Iterator(const ListType& list, const IteratorType& pos) : list_(list), pos_(pos){ }

						Iterator(const Iterator& iter) : list_(iter.list_), pos_(iter.pos_){ }

						NodeType operator*() const{
							return *pos_;
						}

						void operator++(){
							++pos_;
						}

						void operator++(int){
							++pos_;
						}

						bool isValid() const{
							return pos_ != list_.end();
						}

					private:
						const ListType& list_;
						IteratorType pos_;

				};

				NodeGroup(){ }

				NodeGroup(const GroupType& group) : list_(group.list_){ }

				/*bool operator<(const GroupType& group) const{
					if(id_ != group.id_){
						return false;
					}

					if(empty()){
						return false;
					}else if(group.empty()){
						return true;
					}

					IdType id1 = (map_.begin())->first;
					IdType id2 = (group.map_.begin())->first;

					return id1 < id2;
				}*/

				boost::optional<NodeType> nearest() const{
					if(!list_.empty()){
						return boost::make_optional(*(list_.begin()));
					}else{
						return boost::optional<NodeType>();
					}
				}

				void add(const NodeType& node){
					list_.push_back(node);
				}

				void add(const GroupType& group){
					for(Iterator i = group.iterator(); i.isValid(); i++){
						add(*i);
					}
				}

				Iterator iterator() const{
					return Iterator(list_, list_.begin());
				}

				std::size_t size() const{
					return list_.size();
				}

				bool empty() const{
					return list_.empty();
				}

			private:
				ListType list_;

		};

		template <class EndpointType, std::size_t IdSize>
		IStream& operator>>(IStream& stream, NodeGroup<EndpointType, IdSize>& group){
			uint16_t numNodes;
			stream >> numNodes;

			for(unsigned int i = 0; i < numNodes; i++){
				Node<EndpointType, IdSize> node;
				stream >> node;
				group.add(node);
			}

			return stream;
		}

		template <class EndpointType, std::size_t IdSize>
		OStream& operator<<(OStream& stream, const NodeGroup<EndpointType, IdSize>& group){
			stream << uint16_t(group.size());

			for(typename NodeGroup<EndpointType, IdSize>::Iterator i = group.iterator(); i.isValid(); i++){
				stream << *i;
			}

			return stream;
		}

	}

}

#endif
