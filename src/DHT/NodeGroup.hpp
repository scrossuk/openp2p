#ifndef OPENP2P_DHT_NODEGROUP_HPP
#define OPENP2P_DHT_NODEGROUP_HPP

#include <map>
#include <boost/function.hpp>
#include "Node.hpp"
#include "Hash.hpp"

namespace openp2p{

	namespace DHT{

		class NodeGroup{
			public:
				class Iterator{
					public:
						Iterator(const std::multimap<Hash, Node>& map, std::multimap<Hash, Node>::const_iterator pos) : mMap(map), mPos(pos){ }

						Iterator(const Iterator& iterator) : mMap(iterator.mMap), mPos(iterator.mPos){ }

						inline Node operator*() const{
							return mPos->second;
						}

						inline void operator++(){
							++mPos;
						}

						inline void operator++(int){
							++mPos;
						}

						inline bool IsValid() const{
							return mPos != mMap.end();
						}

					private:
						const std::multimap<Hash, Node>& mMap;
						std::multimap<Hash, Node>::const_iterator mPos;

				};

				static NodeGroup Unserialize(Hash hash, Core::BufferIterator& iterator){
					Core::DataReader reader(iterator);

					Core::Uint16 numNodes;
					reader >> numNodes;

					NodeGroup group(hash);

					for(unsigned int i = 0; i < numNodes; i++){
						group.Add(Node::Unserialize(iterator));
					}

					return group;
				}

				NodeGroup(Hash hash) : mHash(hash){ }

				NodeGroup(const NodeGroup& list) : mHash(list.mHash), mMap(list.mMap){ }

				bool operator<(const NodeGroup& group) const{
					if(mHash != group.mHash){
						return false;
					}

					if(IsEmpty()){
						return false;
					}else if(group.IsEmpty()){
						return true;
					}

					Hash hash1 = (mMap.begin())->first;
					Hash hash2 = (group.mMap.begin())->first;

					return hash1 < hash2;
				}

				inline Node Nearest() const{
					return (mMap.begin())->second;
				}

				inline void Add(Node node){
					mMap.insert(std::pair<Hash, Node>(mHash ^ node.GetHash(), node));
				}

				inline void Add(const NodeGroup& group){
					for(Iterator i = group.GetIterator(); i.IsValid(); i++){
						Add(*i);
					}
				}

				inline NodeGroup::Iterator GetIterator() const{
					return Iterator(mMap, mMap.begin());
				}

				inline unsigned int Size() const{
					return mMap.size();
				}

				inline bool IsEmpty() const{
					return mMap.empty();
				}

				Core::Buffer Serialize() const{
					Core::BufferQueue queue;
					queue.Add(Core::Buffer(Core::Uint16(mMap.size())));

					for(Iterator i = GetIterator(); i.IsValid(); i++){
						queue.Add((*i).Serialize());
					}

					return queue.ReadAll();
				}

			private:
				Hash mHash;
				std::multimap<Hash, Node> mMap;

		};

	}

}

#endif
