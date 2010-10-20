#ifndef OPENP2P_CORE_GUARD_HPP
#define OPENP2P_CORE_GUARD_HPP

#include <queue>
#include <boost/function.hpp>
#include "Strand.hpp"
#include "SharedCount.hpp"
#include "SharedPtr.hpp"
#include "Handler.hpp"

namespace openp2p{

	namespace Core{

		class GuardDispatcher: public SharedCount{
			public:
				GuardDispatcher(){
					mIsActive = true;
					mIsConsuming = false;
				}

				inline Strand& GetStrand(){
					return mStrand;
				}

				inline void operator()(boost::function<void ()> f){
					mStrand.Schedule(SharedPtr<GuardDispatcher>(this).Wrap(boost::bind(&GuardDispatcher::DoDispatch, this, f)));
				}

				//Must be scheduled by the guard
				inline void Block(){
					mIsActive = false;
				}

			private:
				inline void DoDispatch(boost::function<void ()> f){
					if(!mIsActive){
						return;
					}

					mQueue.push(f);

					if(!mIsConsuming){
						mIsConsuming = true;
						mStrand.Schedule(boost::bind(&GuardDispatcher::DoConsume, this));
					}
				}

				inline void DoConsume(){
					if(!mIsActive || mQueue.empty()){
						mIsConsuming = false;
						return;
					}

					boost::function<void()> f = mQueue.front();
					f();
					mQueue.pop();
					mStrand.Schedule(boost::bind(&GuardDispatcher::DoConsume, this));
				}

				bool mIsActive;
				bool mIsConsuming;
				Strand mStrand;
				std::queue< boost::function<void()> > mQueue;

		};

		class Guard{
			public:
				Guard() : mDispatcher(new GuardDispatcher()), mStrand(mDispatcher->GetStrand()){ }

				~Guard(){
					mDispatcher->Block();
				}

				void Schedule(boost::function<void ()> f){
					mStrand.Schedule(f);
				}

				template <typename Handler> WrappedHandler<Handler> Wrap(Handler handler){
					return Core::Wrap(SharedPtrDispatcher<GuardDispatcher>(mDispatcher), handler);
				}

			private:
				SharedPtr<GuardDispatcher> mDispatcher;
				Strand& mStrand;

		};

	}

}

#endif
