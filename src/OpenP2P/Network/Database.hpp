#ifndef OPENP2P_KADEMLIA_DATABASE_HPP
#define OPENP2P_KADEMLIA_DATABASE_HPP

#include <cstddef>
#include <boost/optional.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <std::size_t IdSize>
		class Database{
			public:
				virtual bool store(const Id<IdSize>&, const Buffer&) = 0;

				virtual boost::optional<Buffer> retrieve(const Id<IdSize>&) = 0;

		};

	}

}

#endif
