#include <vector>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>

#include <OpenP2P/Block.hpp>
#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Wait.hpp>
#include <OpenP2P/WaitHandler.hpp>

namespace OpenP2P{

	namespace{

		void WaitCallbackFunc(boost::mutex& mutex, boost::condition_variable& cond, std::size_t& firstToFinish, std::size_t id, std::size_t& numToComplete){
			boost::lock_guard<boost::mutex> lock(mutex);
			if(numToComplete == 4){
				firstToFinish = id;
				cond.notify_one();
			}
			numToComplete--;
			if(numToComplete == 0){
				cond.notify_one();
			}
		}

	}

	std::size_t Wait(WaitHandler handler0, WaitHandler handler1, WaitHandler handler2, WaitHandler handler3){
		boost::mutex mutex;
		boost::condition_variable cond;
		boost::unique_lock<boost::mutex> lock(mutex);
		unsigned int firstToFinish;
		unsigned int numToComplete = 4;

		CancelFunction cancel0 = handler0.asyncWait(boost::bind(WaitCallbackFunc, boost::ref(mutex), boost::ref(cond), boost::ref(firstToFinish), 0, boost::ref(numToComplete)));
		CancelFunction cancel1 = handler1.asyncWait(boost::bind(WaitCallbackFunc, boost::ref(mutex), boost::ref(cond), boost::ref(firstToFinish), 1, boost::ref(numToComplete)));
		CancelFunction cancel2 = handler2.asyncWait(boost::bind(WaitCallbackFunc, boost::ref(mutex), boost::ref(cond), boost::ref(firstToFinish), 2, boost::ref(numToComplete)));
		CancelFunction cancel3 = handler3.asyncWait(boost::bind(WaitCallbackFunc, boost::ref(mutex), boost::ref(cond), boost::ref(firstToFinish), 3, boost::ref(numToComplete)));

		cond.wait(lock);

		cancel0();
		cancel1();
		cancel2();
		cancel3();

		if(numToComplete > 0){
			cond.wait(lock);
		}

		return firstToFinish;
	}

	void Wait(WaitHandler handler0){
		Wait(handler0, Block, Block, Block);
	}

	std::size_t Wait(WaitHandler handler0, WaitHandler handler1){
		return Wait(handler0, handler1, Block, Block);
	}

	std::size_t Wait(WaitHandler handler0, WaitHandler handler1, WaitHandler handler2){
		return Wait(handler0, handler1, handler2, Block);
	}

}

