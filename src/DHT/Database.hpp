#ifndef OPENP2P_DHT_DATABASE_HPP
#define OPENP2P_DHT_DATABASE_HPP

#include <boost/tuple/tuple.hpp>
#include "../Core.hpp"

namespace openp2p{

	namespace DHT{

		class Hash;

		class Database{
			public:
				virtual bool Store(Hash, Core::Buffer) = 0;

				virtual boost::tuple<bool, Core::Buffer> Retrieve(Hash) = 0;

				virtual ~Database(){ }

		};

	}

}

#endif
