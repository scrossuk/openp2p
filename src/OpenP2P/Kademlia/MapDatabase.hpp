#ifndef OPENP2P_KADEMLIA_MAPDATABASE_HPP
#define OPENP2P_KADEMLIA_MAPDATABASE_HPP

#include <cstddef>
#include <boost/optional.hpp>
#include <OpenP2P/Buffer.hpp>
#include <OpenP2P/SafeMap.hpp>
#include <OpenP2P/Kademlia/Database.hpp>
#include <OpenP2P/Kademlia/Id.hpp>

namespace OpenP2P{

	namespace Kademlia{

		template <std::size_t IdSize>
		class MapDatabase: public Database<IdSize>{
			public:
				bool store(const Id<IdSize>& id, const Buffer& Buffer){
					return data_map_.insert(id, Buffer);
				}

				boost::optional<Buffer> retrieve(const Id<IdSize>& id){
					return data_map_.find(id);
				}

			private:
				SafeMap<Id<IdSize>, Buffer> data_map_;

		};

	}

}

#endif
