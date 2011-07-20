#ifndef OPENP2P_PROMISE_HPP
#define OPENP2P_PROMISE_HPP

#include <boost/shared_ptr.hpp>

#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/LayerWrapper.hpp>
#include <OpenP2P/Notification/PromiseLayer.hpp>

namespace OpenP2P{
	
	// Reference count container (hides shared_ptr to make the interface simpler).
	template <typename T>
	class Promise: public Notification::LayerWrapper<T>{
		public:
			Promise() : layer_(new Notification::PromiseLayer<T>()){ }
		
			void setValue(const T& value){
				layer_->setValue(value);
			}
			
			Notification::LayerPtr<T> getLayer() const{
				return layer_;
			}
			
		private:
			boost::shared_ptr< Notification::PromiseLayer<T> > layer_;
			
	};

}

#endif
