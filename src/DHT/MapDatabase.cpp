#include <boost/tuple/tuple.hpp>
#include <map>
#include "../Core.hpp"
#include "MapDatabase.hpp"
#include "Hash.hpp"

using namespace openp2p::Core;

namespace openp2p{

	namespace DHT{

		bool MapDatabase::Store(Hash hash, Buffer buffer){
			mDataMap[hash] = buffer;
			return true;
		}

		boost::tuple<bool, Buffer> MapDatabase::Retrieve(Hash hash){
			std::map<Hash, Buffer>::iterator p = mDataMap.find(hash);
			if(p != mDataMap.end()){
				return boost::make_tuple(true, p->second);
			}else{
				return boost::make_tuple(false, Buffer());
			}
		}

	}

}

