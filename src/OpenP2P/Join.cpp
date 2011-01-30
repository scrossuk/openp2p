#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Join.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/WaitCallback.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	/*namespace{

		void JoinCallback(Signal& signal, Signal& completeSignal){
			signal.activate();
			completeSignal.activate();
		}

		void JoinCancel(CancelFunction c0, CancelFunction c1){
			c0();
			c1();
		}

		CancelFunction JoinStart(WaitHandler wait0, WaitHandler wait1, WaitCallback callback){
			Signal signal;
			Signal completeSignal(2);
			CancelFunction c0 = wait0(boost::bind(JoinCallback, boost::ref(signal), boost::ref(completeSignal)));
			CancelFunction c1 = wait1(boost::bind(JoinCallback, boost::ref(signal), boost::ref(completeSignal)));
			completeSignal(callback);

			return signal(boost::bind(JoinCancel, c0, c1));
		}

	}

	Join::Join(WaitHandler wait0, WaitHandler wait1){

	}

	CancelFunction Join::

	WaitHandler Join(WaitHandler wait0, WaitHandler wait1){
		return boost::bind(JoinStart, wait0, wait1, _1);
	}*/

}

