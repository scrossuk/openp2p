#ifndef OPENP2P_CORE_SHAREDPTR_HPP
#define OPENP2P_CORE_SHAREDPTR_HPP

#include <boost/function.hpp>

#include "Handler.hpp"

namespace openp2p{

	namespace Core{

		template <class T> class SharedPtr;

		template <class T>
		void SharedPtrDispatch(SharedPtr<T> ptr, boost::function<void ()> f){
			(*ptr)(f);
		}

		template <class T>
		void SharedPtrDirectDispatch(SharedPtr<T> ptr, boost::function<void ()> f){
			f();
		}

		template <class T>
		class SharedPtrDispatcher{
			public:
				SharedPtrDispatcher(SharedPtr<T>& ptr) : mFunc(SharedPtrDispatch<T>), mPtr(ptr){ }

				SharedPtrDispatcher(SharedPtr<T>& ptr, boost::function<void (SharedPtr<T>, boost::function<void ()>)> func) : mFunc(func), mPtr(ptr){ }

				void operator()(boost::function<void ()> f){
					mFunc(mPtr, f);
				}

			private:
				boost::function<void (SharedPtr<T>, boost::function<void ()>)> mFunc;
				SharedPtr<T> mPtr;

		};

		template<class T>
		class SharedPtr{
			public:
				SharedPtr(){
					mVal = 0;
				}

				SharedPtr(T * v){
					mVal = v;
					if(mVal != 0){
						mVal->IncRef();
					}
				}

				SharedPtr(const SharedPtr<T>& ptr){
					mVal = ptr.Get();
					if(mVal != 0){
						mVal->IncRef();
					}
				}

				~SharedPtr(){
					if(mVal != 0){
						mVal->DecRef();
					}
				}

				T * Get() const{
					return mVal;
				}

				template <typename Handler> WrappedHandler<Handler> Wrap(Handler handler){
					return Core::Wrap(SharedPtrDispatcher<T>(*this, SharedPtrDirectDispatch<T>), handler);
				}

				SharedPtr<T>& operator=(const SharedPtr<T>& ptr){
					if(mVal != 0){
						mVal->DecRef();
					}

					mVal = ptr.Get();

					if(mVal != 0){
						mVal->IncRef();
					}

					return *this;
				}

				bool operator==(const SharedPtr<T>& ptr) const{
					return mVal == ptr.mVal;
				}

				bool operator!=(const SharedPtr<T>& ptr) const{
					return mVal != ptr.mVal;
				}

				T& operator*() const{
					return *mVal;
				}

				T * operator->() const{
					return mVal;
				}

			private:
				T * mVal;

		};

	}

}

#endif
