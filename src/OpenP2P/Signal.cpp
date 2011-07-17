#include <map>
#include <boost/thread.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Signal.hpp>


namespace OpenP2P{

	Signal::Signal() : isActivated_(false){ }

	void Signal::activate(){
		Lock lock(mutex_);
		cond_.notifyAll();
		isActivated_ = true;
	}

	void Signal::reset(){
		Lock lock(mutex_);
		isActivated_ = false;
	}

	bool Signal::isActivated(){
		Lock lock(mutex_);
		return isActivated_;
	}

	void Signal::wait(){
		Lock lock(mutex_);
		if(!isActivated_){
			cond_.wait(lock);
		}
	}

	void Signal::cancel(){
		cond_.notifyAll();
	}

}

