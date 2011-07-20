#ifndef OPENP2P_NOTIFICATION_COMPOSELAYER_HPP
#define OPENP2P_NOTIFICATION_COMPOSELAYER_HPP

#include <boost/function.hpp>
#include <boost/utility.hpp>

#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Layer.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/Notifier.hpp>

namespace OpenP2P{

	namespace Notification{

		template <typename In, typename Out>
		class ComposeLayer: public Layer<Out>, public Handler, public boost::noncopyable{
			public:
				ComposeLayer(LayerPtr<In> layer, boost::function<Out (In)> transform)
					: hasValue_(false), layer_(layer), transform_(transform)
					{
						layer_->addHandler(this);
					}
					
				~ComposeLayer(){
					layer_->removeHandler(this);
				}
				
				void notify(){
					notifier_.notify();
				}
				
				void addHandler(Handler * handler){
					notifier_.addHandler(handler);
				}
				
				void removeHandler(Handler * handler){
					notifier_.removeHandler(handler);
				}
				
				Out getValue(){
					Lock lock(mutex_);
					if(!hasValue_){
						memoizedValue_ = transform_(layer_->getValue());
						hasValue_ = true;
					}
					
					return memoizedValue_;
				}
				
			private:
				Mutex mutex_;
				Notifier notifier_;
				bool hasValue_;
				Out memoizedValue_;
				LayerPtr<In> layer_;
				boost::function<Out (In)> transform_;
			
		};
		
	}
	
}

#endif
