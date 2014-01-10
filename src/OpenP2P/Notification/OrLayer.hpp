#ifndef OPENP2P_NOTIFICATION_ORLAYER_HPP
#define OPENP2P_NOTIFICATION_ORLAYER_HPP

#include <boost/utility.hpp>

#include <OpenP2P/Notification/Handler.hpp>
#include <OpenP2P/Notification/Layer.hpp>
#include <OpenP2P/Notification/LayerPtr.hpp>

namespace OpenP2P {

	namespace Notification {
	
		class OrLayer: public Layer<bool>, public boost::noncopyable {
			public:
				OrLayer(const LayerPtr<bool>& layer0, const LayerPtr<bool>& layer1)
					: layer0_(layer0), layer1_(layer1) { }
					
				void addHandler(Handler* handler) {
					layer0_->addHandler(handler);
					layer1_->addHandler(handler);
				}
				
				void removeHandler(Handler* handler) {
					layer0_->removeHandler(handler);
					layer1_->removeHandler(handler);
				}
				
				bool getValue() {
					return layer0_->getValue() || layer1_->getValue();
				}
				
			private:
				LayerPtr<bool> layer0_, layer1_;
				
		};
		
	}
	
}

#endif
