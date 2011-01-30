#ifndef OPENP2P_KADEMLIA_NODE_HPP
#define OPENP2P_KADEMLIA_NODE_HPP

#include <cstddef>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <typename EndpointType, std::size_t IdSize>
		struct Node{
			Id<IdSize> id;
			EndpointType endpoint;

			Node(){ }

			Node(const Id<IdSize>& i, const EndpointType& e) : id(i), endpoint(e){ }

		};

		template <class EndpointType, std::size_t IdSize>
		IStream& operator>>(IStream& stream, Node<EndpointType, IdSize>& node){
			return stream >> node.id >> node.endpoint;
		}

		template <class EndpointType, std::size_t IdSize>
		OStream& operator<<(OStream& stream, const Node<EndpointType, IdSize>& node){
			return stream << node.id << node.endpoint;
		}

	}

}

#endif
