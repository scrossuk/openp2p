#ifndef OPENP2P_CORE_JOIN_HPP
#define OPENP2P_CORE_JOIN_HPP

#include <boost/function.hpp>
#include "Strand.hpp"
#include "SharedCount.hpp"
#include "SharedPtr.hpp"
#include "Handler.hpp"

namespace openp2p{

	namespace Core{

		class JoinDispatcher: public SharedCount{
			public:
				JoinDispatcher(){
					mIsActivated = false;
				}

				inline void operator()(boost::function<void ()> f){
					mStrand.Schedule(SharedPtr<JoinDispatcher>(this).Wrap(boost::bind(&JoinDispatcher::DoDispatch, this, f)));
				}

			private:
				inline void DoDispatch(boost::function<void ()> f){
					if(!mIsActivated){
						mIsActivated = true;
						f();
					}
				}

				bool mIsActivated;
				Strand mStrand;

		};

		class Join{
			public:
				Join() : mDispatcher(new JoinDispatcher()){ }

				template <typename Handler> WrappedHandler<Handler> Wrap(Handler handler){
					return Core::Wrap(SharedPtrDispatcher<JoinDispatcher>(mDispatcher), handler);
				}

			private:
				SharedPtr<JoinDispatcher> mDispatcher;

		};

	}

}

#endif
