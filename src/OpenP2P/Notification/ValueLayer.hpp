#ifndef OPENP2P_NOTIFICATION_VALUELAYER_HPP
#define OPENP2P_NOTIFICATION_VALUELAYER_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Layer.hpp>

namespace OpenP2P{

	namespace Notification{

		template <typename T>
		class ValueLayer: public Layer<T>, public boost::noncopyable{
			public:
				ValueLayer(){ }
			
				ValueLayer(const T& value) : value_(value){ }
				
				void addHandler(Handler * handler){
					handler->notify();
				}
				
				void removeHandler(Handler * handler){ }
				
				T getValue(){
					return value_;
				}
				
			private:
				T value_;
			
		};
		
	}
	
}

#endif
