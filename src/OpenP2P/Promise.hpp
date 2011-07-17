#ifndef OPENP2P_PROMISE_HPP
#define OPENP2P_PROMISE_HPP

#include <map>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>

namespace OpenP2P{

	// An element in the linked list.
	template <typename T>
	struct PromiseLink{
		PromiseLink<T> * prev, * next;
		boost::function<void (T)> handler;
		
		// True if the handler is currently executing.
		bool isRunning;
		
		// Activated when the handler completes.
		Condition handlerCompleted;
		
		PromiseLink(boost::function<void (T)> h)
			: handler(h), prev(0), next(0), isRunning(false){ }
	};
	
	template <typename T>
	class PromiseImpl: boost::noncopyable{
		public:
			PromiseImpl()
				: isComplete_(false), headLink_(0){ }
		
			void setValue(const T& value){
				Lock lock(mutex_);
				if(isComplete_){
					// The promise value can't be set twice.
					return;
				}
				
				isComplete_ = true;
				value_ = value;
				
				PromiseLink<T> * ptr = headLink_;
				IOService& service = GetIOService();
				
				// Call all queued handlers.
				for(PromiseLink<T> * ptr = headLink_; ptr != 0; ptr = ptr->next){
					ptr->isRunning = true;
					
					service.post(boost::bind(&PromiseImpl::onHandler, this, ptr));
				}
			}
		
			void * addHandler(boost::function<void (T)> handler){
				Lock lock(mutex_);
				PromiseLink<T> * link = new PromiseLink<T>(handler);
						
				// Add to the linked list.
				if(headLink_ != 0){
					link->next = headLink_;
					headLink_->prev = link;
				}
				headLink_ = link;
				
				if(isComplete_){
					// Already completed - run the handler.
					GetIOService().post(boost::bind(&PromiseImpl::onHandler, this, link));
				}
				
				return link;
			}
		
			void removeHandler(void * ptr){
				Lock lock(mutex_);
				PromiseLink<T> * link = (PromiseLink<T> *) ptr;
				
				if(link->isRunning){
					// Currently executing this handler - wait for it to finish.
					(link->handlerCompleted).wait(lock);
				}
				
				// Delete from the linked list.
				PromiseLink<T> * prev = link->prev;
				PromiseLink<T> * next = link->next;
					
				if(prev != 0){
					prev->next = next;
				}
					
				if(next != 0){
					next->prev = prev;
				}
				
				if(link == headLink_){
					headLink_ = link->next;
				}
					
				delete link;
			}
			
		private:
			void onHandler(PromiseLink<T> * link){
				(link->handler)(value_);
				
				Lock lock(mutex_);
				link->isRunning = false;
				(link->handlerCompleted).notify();
			}
		
			Mutex mutex_;
			
			// Head of the linked list.
			PromiseLink<T> * headLink_;
			
			bool isComplete_;
			T value_;
	
	};
	
	// Reference count container (hides shared_ptr to make the interface simpler).
	template <typename T>
	class Promise{
		public:
			Promise() : promise_(new PromiseImpl<T>()){ }
		
			void setValue(const T& value){
				promise_->setValue(value);
			}
			
			void * addHandler(boost::function<void (T)> handler){
				return promise_->addHandler(handler);
			}
			
			void removeHandler(void * ptr){
				promise_->removeHandler(ptr);
			}
			
		private:
			boost::shared_ptr< PromiseImpl<T> > promise_;
			
	};

}

#endif
