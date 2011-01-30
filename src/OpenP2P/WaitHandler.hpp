#ifndef OPENP2P_WAITHANDLER_HPP
#define OPENP2P_WAITHANDLER_HPP

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/ref.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/WaitCallback.hpp>

namespace OpenP2P{

	class WaitObject{
		public:
			virtual CancelFunction asyncWait(WaitCallback) = 0;
	};

	inline CancelFunction CallWait(WaitObject& object, WaitCallback callback){
		return object.asyncWait(callback);
	}

	class WaitHandler{
		public:
			WaitHandler(CancelFunction (*func)(WaitCallback)) : func_(func){ }

			WaitHandler(boost::function<CancelFunction (WaitCallback)> func) : func_(func){ }

			WaitHandler(WaitObject& object) : func_(boost::bind(CallWait, boost::ref(object), _1)){ }

			CancelFunction asyncWait(WaitCallback callback){
				return func_(callback);
			}

		private:
			boost::function<CancelFunction (WaitCallback)> func_;

	};

}

#endif
