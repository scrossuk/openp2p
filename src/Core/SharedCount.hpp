#ifndef OPENP2P_CORE_SHAREDCOUNT_HPP
#define OPENP2P_CORE_SHAREDCOUNT_HPP

#include <boost/detail/atomic_count.hpp>

namespace openp2p{

	namespace Core{

		class SharedCount{
			public:
				SharedCount() : mCounter(0){ }

				inline void IncRef(){
					++mCounter;
				}

				inline void DecRef(){
					--mCounter;
					if(mCounter == 0){
						delete this;
					}
				}

			protected:
				virtual ~SharedCount(){ }

			private:
				boost::detail::atomic_count mCounter;

		};

	}

}

#endif
