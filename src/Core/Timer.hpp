#ifndef OPENP2P_CORE_TIMER_HPP
#define OPENP2P_CORE_TIMER_HPP

#include <boost/function.hpp>

namespace openp2p{

	namespace Core{

		class Timer{
			public:
				Timer();

				~Timer();

				//Time given in seconds
				void Add(double, boost::function<void ()>);

			private:
				void * mInternal;

		};

	}

}

#endif
