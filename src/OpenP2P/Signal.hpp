#ifndef OPENP2P_SIGNAL_HPP
#define OPENP2P_SIGNAL_HPP

#include <map>
#include <boost/thread.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	class Signal: public WaitObject{
		public:
			Signal();
			Signal(std::size_t);

			void activate();

			void reset();

			bool isActivated();

			CancelFunction asyncWait(WaitCallback);

		private:
			void cancel(unsigned int);

			boost::mutex mutex_;
			std::size_t activateCount_, activateMax_;
			std::size_t nextId_;
			std::map<std::size_t, WaitCallback> callbacks_;

	};

}

#endif
