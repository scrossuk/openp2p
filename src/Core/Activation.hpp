#ifndef OPENP2P_CORE_ACTIVATION_HPP
#define OPENP2P_CORE_ACTIVATION_HPP

#include <boost/function.hpp>
#include "Strand.hpp"
#include "SharedCount.hpp"
#include "SharedPtr.hpp"

namespace openp2p{

	namespace Core{

		class ActivationDispatcher: public SharedCount{
			public:
				ActivationDispatcher(unsigned int n, boost::function<void ()> f) : mCounter(n), mFunc(f){ }

				void Dispatch(){
					mStrand.Schedule(SharedPtr<ActivationDispatcher>(this).Wrap(boost::bind(&ActivationDispatcher::DoDispatch, this)));
				}

			private:
				inline void DoDispatch(){
					if(mCounter > 0){
						mCounter--;
					}
					if(mCounter == 0){
						mFunc();
					}
				}

				unsigned int mCounter;
				boost::function<void ()> mFunc;
				Strand mStrand;

		};

		class Activation{
			public:
				Activation(unsigned int n, boost::function<void ()> f) : mDispatcher(new ActivationDispatcher(n, f)){ }

				void operator()(){
					mDispatcher->Dispatch();
				}

			private:
				SharedPtr<ActivationDispatcher> mDispatcher;

		};

	}

}

#endif
