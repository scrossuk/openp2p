#ifndef OPENP2P_ROOTNETWORK_RPCS_HPP
#define OPENP2P_ROOTNETWORK_RPCS_HPP

#include <OpenP2P/RootNetwork/Id.hpp>
#include <OpenP2P/RootNetwork/Node.hpp>

namespace OpenP2P{

	namespace RootNetwork{

		namespace Ping{

			struct Request{ };

			inline BinaryStream& operator>>(BinaryStream& stream, Request&){
				return stream;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Request&){
				return stream;
			}

			struct Reply{ };

			inline BinaryStream& operator>>(BinaryStream& stream, Reply&){
				return stream;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Reply&){
				return stream;
			}

		}

		namespace FindNode{
	
			struct Request{
				Id nodeId;
			};

			inline BinaryStream& operator>>(BinaryStream& stream, Request& request){
				return stream >> request.nodeId;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Request& request){
				return stream << request.nodeId;
			}

			struct Reply{
				std::vector<Node> nearestGroup;
			};

			inline BinaryStream& operator>>(BinaryStream& stream, Reply& reply){
				uint8_t length;
				stream >> length;
				reply.nearestGroup.resize(length);
				return stream >> reply.nearestGroup;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Reply& reply){
				return stream << uint8_t(reply.nearestGroup.size()) << reply.nearestGroup;
			}

		}

		namespace Subscribe{
	
			struct Request{
				Id subscriptionId;
			};

			inline BinaryStream& operator>>(BinaryStream& stream, Request& request){
				return stream >> request.subscriptionId;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Request& request){
				return stream << request.subscriptionId;
			}

		}

		namespace GetSubscribers{
	
			struct Request{
				Id subscriptionId;
			};

			inline BinaryStream& operator>>(BinaryStream& stream, Request& request){
				return stream >> request.subscriptionId;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Request& request){
				return stream << request.subscriptionId;
			}

			struct Reply{
				std::vector<Node> subscribersGroup;
			};

			inline BinaryStream& operator>>(BinaryStream& stream, Reply& reply){
				uint8_t length;
				stream >> length;
				reply.subscribersGroup.resize(length);
				return stream >> reply.subscribersGroup;
			}

			inline BinaryStream& operator<<(BinaryStream& stream, const Reply& reply){
				return stream << uint8_t(reply.subscribersGroup.size()) << reply.subscribersGroup;
			}

		}
	
	}
	
}

#endif
