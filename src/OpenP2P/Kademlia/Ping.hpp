#ifndef OPENP2P_KADEMLIA_PING_HPP
#define OPENP2P_KADEMLIA_PING_HPP

#include <cstddef>
#include <string>
#include <OpenP2P/IStream.hpp>
#include <OpenP2P/OStream.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P {

	namespace Kademlia {

		namespace Ping {

			struct Request { };

			template <std::size_t IdSize>
			struct Reply {
				Id<IdSize> id;

				Reply() { }

				Reply(const Id<IdSize>& i) : id(i) { }
			};

			IStream& operator>>(IStream& stream, Request& request) {
				return stream;
			}

			OStream& operator<<(OStream& stream, const Request& request) {
				return stream;
			}

			template <std::size_t IdSize>
			IStream& operator>>(IStream& stream, Reply<IdSize>& reply) {
				return stream >> reply.id;
			}

			template <std::size_t IdSize>
			OStream& operator<<(OStream& stream, const Reply<IdSize>& reply) {
				return stream << reply.id;
			}

		}

	}

}

#endif
