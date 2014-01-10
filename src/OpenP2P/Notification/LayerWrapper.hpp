#ifndef OPENP2P_NOTIFICATION_LAYERWRAPPER_HPP
#define OPENP2P_NOTIFICATION_LAYERWRAPPER_HPP

#include <OpenP2P/Notification/LayerPtr.hpp>

namespace OpenP2P {

	namespace Notification {
	
		template <typename T>
		class LayerWrapper {
			public:
				virtual LayerPtr<T> getLayer() const = 0;
				
		};
		
	}
	
}

#endif
