#ifndef OPENP2P_NETWORK_GETSUBSCRIBERS_HPP
#define OPENP2P_NETWORK_GETSUBSCRIBERS_HPP

#include <cstddef>
#include <boost/optional.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Kademlia/Database.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P {

	namespace Network {

		namespace GetSubscribers {

			template <std::size_t IdSize>
			struct Request {
				Id<IdSize> dataId;

				Request(){ }

				Request(const Id<IdSize>& id) : dataId(id){ }
			};

			struct Reply {
				boost::optional<Buffer> data;

				Reply(){ }

				Reply(const boost::optional<Buffer>& b) : data(b){ }
			};

			template <std::size_t IdSize>
			IStream& operator>>(IStream& stream, Request<IdSize>& request) {
				return stream >> request.dataId;
			}

			template <std::size_t IdSize>
			OStream& operator<<(OStream& stream, const Request<IdSize>& request) {
				return stream << request.dataId;
			}

			IStream& operator>>(IStream& stream, Reply& reply) {
				return stream >> reply.data;
			}

			OStream& operator<<(OStream& stream, const Reply& reply) {
				return stream << reply.data;
			}

		}

	}

}

#endif
