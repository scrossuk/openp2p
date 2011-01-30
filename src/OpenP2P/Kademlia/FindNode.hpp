#ifndef OPENP2P_KADEMLIA_FINDNODE_HPP
#define OPENP2P_KADEMLIA_FINDNODE_HPP

#include <cstddef>
#include <string>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P {

	namespace Kademlia {

		namespace FindNode {

			template <std::size_t IdSize>
			struct Request {
				Id<IdSize> nodeId;

				Request(){ }

				Request(const Id<IdSize>& id) : nodeId(id){ }
			};

			template <typename EndpointType, std::size_t IdSize>
			struct Reply {
				NodeGroup<EndpointType, IdSize> group;

				Reply(){ }

				Reply(const NodeGroup<EndpointType, IdSize>& g) : group(g){ }
			};

			template <std::size_t IdSize>
			IStream& operator>>(IStream& stream, Request<IdSize>& request) {
				return stream >> request.nodeId;
			}

			template <std::size_t IdSize>
			OStream& operator<<(OStream& stream, const Request<IdSize>& request) {
				return stream << request.nodeId;
			}

			template <typename EndpointType, std::size_t IdSize>
			IStream& operator>>(IStream& stream, Reply<EndpointType, IdSize>& reply) {
				return stream >> reply.group;
			}

			template <typename EndpointType, std::size_t IdSize>
			OStream& operator<<(OStream& stream, const Reply<EndpointType, IdSize>& reply) {
				return stream << reply.group;
			}

		}

	}

}

#endif
