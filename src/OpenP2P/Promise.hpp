#ifndef OPENP2P_PROMISE_HPP
#define OPENP2P_PROMISE_HPP

#include <map>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>

namespace OpenP2P{

	template <typename T>
	struct PromiseLink{
		PromiseLink<T> * prev, * next;
		boost::function<void (T)> handler;
		
		PromiseLink(boost::function<void (T)> h)
			: handler(h), prev(0), next(0){ }
	};
	
	template <typename T>
	class PromiseImpl: boost::noncopyable{
		public:
			PromiseImpl()
				: isComplete_(false), headLink_(0), activeLink_(0){ }
				
			PromiseImpl(const T& value)
				: isComplete_(false), headLink_(0), activeLink_(0), value_(value){ }
		
			void setValue(const T& value){
				{
					Lock lock(mutex_);
					if(isComplete_){
						// The promise value can't be set twice.
						return;
					}else{
						isComplete_ = true;
						value_ = value;
					}
				}
				
				// Call all queued handlers.
				while(true){
					{
						Lock lock(mutex_);
						activeLink_ = (activeLink_ == 0) ? headLink_ : activeLink_->next;
						activeLinkChanged_.notify();
						if(activeLink_ == 0){
							break;
						}
					}
					
					// Call the handler outside the lock, avoiding potential deadlock if it calls other methods on this promise.
					(activeLink_->handler)(value);
				}
			}
		
			void * addHandler(boost::function<void (T)> handler){
				{
					Lock lock(mutex_);
					if(!isComplete_){
						PromiseLink<T> * link = new PromiseLink<T>(handler);
						
						// Add to the linked list.
						if(headLink_ != 0){
							link->next = headLink_;
							headLink_->prev = link;
						}
						headLink_ = link;
						
						return link;
					}
				}
				
				handler(value_);
				return 0;
			}
		
			void removeHandler(void * ptr){
				if(ptr == 0) return;
				
				Lock lock(mutex_);
				if(activeLink_ == ptr){
					// Currently executing this handler - wait for it to finish.
					activeLinkChanged_.wait(lock);
				}
				
				// Delete from the linked list.
				PromiseLink<T> * link = (PromiseLink<T> *) ptr;
				PromiseLink<T> * prev = link->prev;
				PromiseLink<T> * next = link->next;
					
				if(prev != 0){
					prev->next = next;
				}
					
				if(next != 0){
					next->prev = prev;
				}
					
				delete link;
			}
			
		private:
			Mutex mutex_;
			PromiseLink<T> * headLink_, * activeLink_;
			
			// Notified every time activeLink_ changes.
			Condition activeLinkChanged_;
			
			bool isComplete_;
			T value_;
	
	};
	
	// Reference count container (hides shared_ptr to make the interface simpler).
	template <typename T>
	class Promise{
		public:
			Promise() : promise_(new PromiseImpl<T>()){ }
			
			Promise(const T& value) : promise_(new PromiseImpl<T>(value)){ }
		
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
