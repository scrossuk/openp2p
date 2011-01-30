#ifndef OPENP2P_KADEMLIA_STORE_HPP
#define OPENP2P_KADEMLIA_STORE_HPP

#include <cstddef>
#include <string>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/BufferBuilder.hpp>
#include <OpenP2P/BufferIterator.hpp>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P {

	namespace Kademlia {

		namespace Store {

			template <std::size_t IdSize>
			struct Request {
				Id<IdSize> dataId;
				Buffer data;

				Request() { }

				Request(const Id<IdSize>& id, const Buffer& d) : dataId(id), data(d) { }
			};

			struct Reply {
				bool success;

				Reply() : success(false) { }

				Reply(bool s) : success(s) { }
			};

			template <std::size_t IdSize>
			IStream& operator>>(IStream& stream, Request<IdSize>& request) {
				return stream >> request.dataId >> request.data;
			}

			template <std::size_t IdSize>
			OStream& operator<<(OStream& stream, const Request<IdSize>& request) {
				return stream << request.dataId << request.data;
			}

			IStream& operator>>(IStream& stream, Reply& reply) {
				return stream >> reply.success;
			}

			OStream& operator<<(OStream& stream, const Reply& reply) {
				return stream << reply.success;
			}

		}

	}

}

#endif
