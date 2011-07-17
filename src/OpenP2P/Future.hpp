#ifndef OPENP2P_FUTURE_HPP
#define OPENP2P_FUTURE_HPP

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Condition.hpp>
#include <OpenP2P/IOService.hpp>
#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Promise.hpp>
#include <OpenP2P/Signal.hpp>

namespace OpenP2P{

	namespace FutureDetail{
	
		template <typename T>
		void valueHandler(T value, boost::function<void (T)> handler, Signal * signal){
			handler(value);
			signal->activate();
		}
	
		template <typename T>
		void * valueAdd(T value, boost::function<void (T)> handler){
			Signal * signal = new Signal();
			GetIOService().post(boost::bind(valueHandler<T>, value, handler, signal));
			return signal;
		}
		
		inline void valueRemove(void * ptr){
			Signal * signal = (Signal *) ptr;
			signal->wait();
			delete signal;
		}

		template <typename T>
		void * promiseAdd(Promise<T> promise, boost::function<void (T)> handler){
			return promise.addHandler(handler);
		}
		
		template <typename T>
		void promiseRemove(Promise<T> promise, void * ptr){
			promise.removeHandler(ptr);
		}
	
		template <typename In, typename Out>
		void compose(boost::function<Out (In)> transform, boost::function<void (Out)> sink, In value){
			sink(transform(value));
		}
	
		template <typename In, typename Out>
		void * composedAdd(boost::function<void * (boost::function<void (In)>)> addHandler, boost::function<Out (In)> transform, boost::function<void (Out)> handler){
			return addHandler(boost::bind(compose<In, Out>, transform, handler, _1));
		}
		
		template <typename In1, typename In2>
		class LateJoinData: boost::noncopyable{
			public:
				LateJoinData(boost::function<void (std::pair<In1, In2>)> function)
					: done1_(false), done2_(false),
					function_(function){ }
				
				void activate1(In1 value){
					{
						Lock lock(mutex_);
						value1_ = value;
						done1_ = true;
						if(!done2_){
							// The caller is first - wait for the other future to complete.
							return;
						}
					}
					
					// The caller was last (hence the join handler can be called).
					activate();
				}
				
				void activate2(In2 value){
					{
						Lock lock(mutex_);
						value2_ = value;
						done2_ = true;
						if(!done1_){
							// The caller is first - wait for the other future to complete.
							return;
						}
					}
					
					// The caller was last (hence the join handler can be called).
					activate();
				}
				
				void * ptr1, * ptr2;
				
			private:
				void activate(){
					// Both values have been received - call the next handler up.
					function_(std::make_pair(value1_, value2_));
				}
			
				boost::function<void (std::pair<In1, In2>)> function_;
				bool done1_, done2_;
				Mutex mutex_;
				In1 value1_;
				In2 value2_;
			
		};
		
		template <typename In1, typename In2>
		void * lateJoinAdd(boost::function<void * (boost::function<void (In1)>)> addHandler1,
			boost::function<void * (boost::function<void (In2)>)> addHandler2,
			boost::function<void (std::pair<In1, In2>)> handler){
			
			LateJoinData<In1, In2> * data = new LateJoinData<In1, In2>(handler);
			
			// Add the two handlers.
			data->ptr1 = addHandler1(boost::bind(&LateJoinData<In1, In2>::activate1, data, _1));
			data->ptr2 = addHandler2(boost::bind(&LateJoinData<In1, In2>::activate2, data, _1));
			
			return data;
		}
		
		template <typename In1, typename In2>
		void lateJoinRemove(boost::function<void (void *)> remove1, boost::function<void (void *)> remove2, void * ptr){
			LateJoinData<In1, In2> * data = (LateJoinData<In1, In2> *) ptr;
			
			// Remove the two handlers.
			remove1(data->ptr1);
			remove2(data->ptr2);
			
			delete data;
		}
		
	}

	template <typename T>
	class Future{
		public:
			Future()
				: isReady_(false),
				add_(boost::bind(FutureDetail::valueAdd<T>, value_, _1)),
				remove_(boost::bind(FutureDetail::valueRemove, _1))
				{
					ptr_ = add_(boost::bind(&Future<T>::activate, this, _1));
				}
			
			Future(const T& value)
				: isReady_(false),
				add_(boost::bind(FutureDetail::valueAdd<T>, value, _1)),
				remove_(boost::bind(FutureDetail::valueRemove, _1))
				{
					ptr_ = add_(boost::bind(&Future<T>::activate, this, _1));
				}
				
			Future(const Promise<T>& promise)
				: isReady_(false),
				add_(boost::bind(FutureDetail::promiseAdd<T>, promise, _1)),
				remove_(boost::bind(FutureDetail::promiseRemove<T>, promise, _1))
				{
					ptr_ = add_(boost::bind(&Future<T>::activate, this, _1));
				}
				
			Future(boost::function<void * (boost::function<void (T)>)> add,
				boost::function<void (void *)> remove)
				: isReady_(false), add_(add), remove_(remove)
				{
					ptr_ = add_(boost::bind(&Future<T>::activate, this, _1));
				}
				
			Future(const Future<T>& future)
				: isReady_(false), add_(future.add_), remove_(future.remove_)
				{
					ptr_ = add_(boost::bind(&Future<T>::activate, this, _1));
				}
				
			Future<T>& operator=(const Future<T>& future){
				remove_(ptr_);
				
				add_ = future.add_;
				remove_ = future.remove_;
				isReady_ = false;
				
				ptr_ = add_(boost::bind(&Future<T>::activate, this, _1));
			}
				
			~Future(){
				remove_(ptr_);
			}
			
			bool isReady(){
				Lock lock(mutex_);
				return isReady_;
			}
			
			T get(){
				Lock lock(mutex_);
				if(!isReady_){
					cond_.wait(lock);
				}
				return value_;
			}
			
			template <typename Out>
			Future<Out> compose(boost::function<Out (T)> transform){
				return Future<Out>(boost::bind(FutureDetail::composedAdd<T, Out>, add_, transform, _1),
					remove_); // The remove function is the same when composing.
			}
			
			template <typename S>
			Future< std::pair<T, S> > lateJoin(const Future<S>& future){
				return Future< std::pair<T, S> >(boost::bind(FutureDetail::lateJoinAdd<T, S>, add_, future.add_, _1),
					boost::bind(FutureDetail::lateJoinRemove<T, S>, remove_, future.remove_, _1));
			}
			
			void wait(){
				Lock lock(mutex_);
				if(!isReady_){
					cond_.wait(lock);
				}
			}
		
			bool timedWait(double secs){
				Lock lock(mutex_);
				if(!isReady_){
					cond_.timedWait(lock, secs);
				}
				return isReady_;
			}
		
		private:		
			void activate(T value){
				Lock lock(mutex_);
				value_ = value;
				isReady_ = true;
				cond_.notifyAll();
			}
			
			template <typename S>
			friend class Future;
			
			Condition cond_;
			T value_;
			Mutex mutex_;
			bool isReady_;
			void * ptr_;
			
			// Function for adding a handler.
			boost::function<void * (boost::function<void (T)>)> add_;
			
			// Function for removing a handler.
			boost::function<void (void *)> remove_;
	
	};

}

#endif
