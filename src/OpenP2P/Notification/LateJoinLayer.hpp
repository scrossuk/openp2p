#ifndef OPENP2P_NOTIFICATION_LATEJOINLAYER_HPP
#define OPENP2P_NOTIFICATION_LATEJOINLAYER_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Lock.hpp>
#include <OpenP2P/Mutex.hpp>
#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Layer.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/Notifier.hpp>

namespace OpenP2P{

	namespace Notification{

		template <typename T0, typename T1>
		class LateJoinLayer: public Layer< std::pair<T0, T1> >, public Handler, public boost::noncopyable{
			public:
				LateJoinLayer(const LayerPtr<T0>& layer0, const LayerPtr<T1>& layer1)
					: hasValue_(false), notified_(false),
					layer0_(layer0), layer1_(layer1){
					
						layer0_->addHandler(this);
						layer1_->addHandler(this);
					}
					
				~LateJoinLayer(){
					layer0_->removeHandler(this);
					layer1_->removeHandler(this);
				}
					
				void notify(){
					Lock lock(mutex_);
					if(notified_){
						notifier_.notify();
					}else{
						notified_ = true;
					}
				}
				
				void addHandler(Handler * handler){
					notifier_.addHandler(handler);
				}
				
				void removeHandler(Handler * handler){
					notifier_.removeHandler(handler);
				}
				
				std::pair<T0, T1> getValue(){
					Lock lock(mutex_);
					if(!hasValue_){
						memoizedValue_ = std::make_pair(layer0_->getValue(), layer1_->getValue());
						hasValue_ = true;
					}
					
					return memoizedValue_;
				}
				
			private:
				Mutex mutex_;
				Notifier notifier_;
				bool notified_, hasValue_;
				std::pair<T0, T1> memoizedValue_;
				LayerPtr<T0> layer0_;
				LayerPtr<T1> layer1_;
			
		};
		
	}
	
}

#endif
