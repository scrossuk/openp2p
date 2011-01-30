#include <map>
#include <boost/thread.hpp>

#include <OpenP2P/CancelFunction.hpp>
#include <OpenP2P/Dispatcher.hpp>
#include <OpenP2P/NullFunction.hpp>
#include <OpenP2P/Signal.hpp>
#include <OpenP2P/WaitCallback.hpp>

namespace OpenP2P{

	Signal::Signal() : activateCount_(0), activateMax_(1), nextId_(0){ }

	Signal::Signal(unsigned int activateMax) : activateCount_(0), activateMax_(activateMax), nextId_(0){ }

	void Signal::activate(){
		boost::lock_guard<boost::mutex> lock(mutex_);
		if(activateCount_ < activateMax_){
			activateCount_++;

			if(activateCount_ == activateMax_){
				for(std::map<std::size_t, WaitCallback>::iterator p = callbacks_.begin(); p != callbacks_.end(); ++p){
					Schedule(p->second);
				}

				callbacks_.clear();
			}
		}
	}

	void Signal::reset(){
		boost::lock_guard<boost::mutex> lock(mutex_);
		activateCount_ = 0;
	}

	bool Signal::isActivated(){
		boost::lock_guard<boost::mutex> lock(mutex_);
		return activateCount_ == activateMax_;
	}

	CancelFunction Signal::asyncWait(WaitCallback callback){
		boost::lock_guard<boost::mutex> lock(mutex_);
		if(activateCount_ < activateMax_){
			std::size_t id = nextId_++;
			callbacks_.insert(std::pair<std::size_t, WaitCallback>(id, callback));
			return boost::bind(&Signal::cancel, this, id);
		}else{
			Schedule(callback);
			return NullFunction;
		}
	}

	void Signal::cancel(std::size_t id){
		boost::lock_guard<boost::mutex> lock(mutex_);
		std::map<std::size_t, WaitCallback>::iterator p = callbacks_.find(id);
		if(p != callbacks_.end()){
			Schedule(p->second);
			callbacks_.erase(p);
		}
	}

}

