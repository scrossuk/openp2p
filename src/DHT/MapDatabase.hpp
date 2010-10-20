#ifndef OPENP2P_DHT_MAPDATABASE_HPP
#define OPENP2P_DHT_MAPDATABASE_HPP

#include <map>
#include <boost/tuple/tuple.hpp>
#include "../Core.hpp"
#include "Database.hpp"

namespace openp2p{

	namespace DHT{

		class Hash;

		class MapDatabase: public Database{
			public:
				bool Store(Hash, Core::Buffer);

				boost::tuple<bool, Core::Buffer> Retrieve(Hash);

			private:
				std::map<Hash, Core::Buffer> mDataMap;

		};

	}

}

#endif
