#ifndef OPENP2P_EVENTDISPATCHER_HPP
#define OPENP2P_EVENTDISPATCHER_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Notification/AssignLayer.hpp>
#include <OpenP2P/Notification/Layer.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>
#include <OpenP2P/Notification/LayerWrapper.hpp>

namespace OpenP2P {

	class EventDispatcher: public Notification::LayerWrapper<bool> {
		public:
			inline EventDispatcher(bool initialValue = false)
				: layer_(new Notification::AssignLayer<bool>(initialValue)) { }
				
			inline void set(bool value) {
				layer_->setValue(value);
			}
			
			inline void activate() {
				layer_->setValue(true);
			}
			
			inline void reset() {
				layer_->setValue(false);
			}
			
			inline Notification::LayerPtr<bool> getLayer() const {
				return layer_;
			}
			
		private:
			boost::shared_ptr< Notification::AssignLayer<bool> > layer_;
			
	};
	
}

#endif
