#ifndef OPENP2P_PROMISE_HPP
#define OPENP2P_PROMISE_HPP

#include <boost/shared_ptr.hpp>

#include <OpenP2P/Notification/AssignLayer.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/LayerWrapper.hpp>

namespace OpenP2P{
	
	// Reference count container (hides shared_ptr to make the interface simpler).
	template <typename T>
	class Promise: public Notification::LayerWrapper<T>{
		public:
			Promise() : layer_(new Notification::AssignLayer<T>()){ }
		
			void setValue(const T& value){
				// Promise value cannot be set twice.
				if(!layer_->hasValue()){
					layer_->setValue(value);
				}
			}
			
			Notification::LayerPtr<T> getLayer() const{
				return layer_;
			}
			
		private:
			boost::shared_ptr< Notification::AssignLayer<T> > layer_;
			
	};

}

#endif
