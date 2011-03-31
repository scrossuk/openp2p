#ifndef OPENP2P_SIGNAL_HPP
#define OPENP2P_SIGNAL_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Cancellable.hpp>
#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Mutex.hpp>

namespace OpenP2P{

	class Signal: public Cancellable, boost::noncopyable{
		public:
			Signal();

			void activate();

			void reset();

			bool isActivated();

			void wait();
			
			void cancel();

		private:
			Mutex mutex_;
			Condition cond_;
			bool isActivated_;

	};

}

#endif
