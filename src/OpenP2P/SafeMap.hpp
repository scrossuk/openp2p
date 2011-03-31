#ifndef OPENP2P_SAFEMAP_HPP
#define OPENP2P_SAFEMAP_HPP

#include <map>
#include <boost/optional.hpp>
#include <boost/thread.hpp>

namespace OpenP2P{

	template <typename KeyType, typename ValueType>
	class SafeMap{
		typedef std::map<KeyType, ValueType> MapType;
		typedef typename MapType::iterator IteratorType;

		public:
			SafeMap(){ }

			SafeMap(const MapType& map) : map_(map){ }

			bool insert(const KeyType& key, const ValueType& value){
				boost::lock_guard<boost::mutex> lock(mutex_);
				IteratorType i = map_.find(key);
				std::pair<IteratorType, bool> s = map_.insert(std::pair<KeyType, ValueType>(key, value));
				return s.second;
			}

			boost::optional<ValueType> find(const KeyType& key){
				boost::lock_guard<boost::mutex> lock(mutex_);
				IteratorType i = map_.find(key);
				if(i != map_.end()){
					return boost::make_optional(i->second);
				}else{
					return boost::optional<ValueType>();
				}
			}

		private:
			boost::mutex mutex_;
			MapType map_;

	};

}

#endif
