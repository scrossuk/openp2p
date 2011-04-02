#ifndef OPENP2P_KADEMLIA_NODE_HPP
#define OPENP2P_KADEMLIA_NODE_HPP

#include <cstddef>
#include <OpenP2P/BinaryStream.hpp>
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

		template <typename EndpointType, std::size_t IdSize>
		bool operator==(const Node<EndpointType, IdSize>& a, const Node<EndpointType, IdSize>& b){
			return a.id == b.id;
		}

		template <typename EndpointType, std::size_t IdSize>
		bool operator!=(const Node<EndpointType, IdSize>& a, const Node<EndpointType, IdSize>& b){
			return !(a == b);
		}

		template <typename EndpointType, std::size_t IdSize>
		bool operator<(const Node<EndpointType, IdSize>& a, const Node<EndpointType, IdSize>& b){
			return a.id < b.id;
		}

		template <class EndpointType, std::size_t IdSize>
		BinaryStream& operator>>(BinaryStream& stream, Node<EndpointType, IdSize>& node){
			return stream >> node.id >> node.endpoint;
		}

		template <class EndpointType, std::size_t IdSize>
		BinaryStream& operator<<(BinaryStream& stream, const Node<EndpointType, IdSize>& node){
			return stream << node.id << node.endpoint;
		}

	}

}

#endif
