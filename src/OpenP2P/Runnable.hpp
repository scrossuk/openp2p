#ifndef OPENP2P_RUNNABLE_HPP
#define OPENP2P_RUNNABLE_HPP

#include <OpenP2P/Cancellable.hpp>

namespace OpenP2P{

	class Runnable: public Cancellable{
		public:
			virtual void run() = 0;

	};

}

#endif
