#ifndef OPENP2P_NOTIFICATION_PROMISELAYER_HPP
#define OPENP2P_NOTIFICATION_PROMISELAYER_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Layer.hpp>
#include <OpenP2P/Notification/Notifier.hpp>

namespace OpenP2P{

	namespace Notification{

		template <typename T>
		class PromiseLayer: public Notification::Layer<T>, public boost::noncopyable{
			public:
				void setValue(const T& value){
					// Promise value cannot be set twice.
					if(!notifier_.isComplete()){
						value_ = value;
						notifier_.notify();
					}
				}
			
				T getValue(){
					return value_;
				}
		
				void addHandler(Notification::Handler * handler){
					notifier_.addHandler(handler);
				}
		
				void removeHandler(Notification::Handler * handler){
					notifier_.removeHandler(handler);
				}
			
			private:
				Notification::Notifier notifier_;
				T value_;
	
		};
		
	}
	
}

#endif
