#ifndef OPENP2P_IDMAP_HPP
#define OPENP2P_IDMAP_HPP

#include <boost/optional.hpp>
#include <OpenP2P/IdGenerator.hpp>
#include <OpenP2P/SafeMap.hpp>

namespace OpenP2P{

	template <typename IdType, typename ValueType>
	class IdMap{
		public:
			IdMap(IdGenerator<IdType>& generator) : generator_(generator){ }

			IdType insert(const ValueType& value){
				IdType id;
				while(!map_.insert(id = generator_.generate(), value));
				return id;
			}

			boost::optional<ValueType> find(const IdType& id){
				return map_.find(id);
			}

		private:
			IdGenerator<IdType>& generator_;
			SafeMap<IdType, ValueType> map_;

	};

}

#endif
