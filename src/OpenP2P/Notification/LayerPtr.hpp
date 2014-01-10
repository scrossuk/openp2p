#ifndef OPENP2P_NOTIFICATION_LAYERPTR_HPP
#define OPENP2P_NOTIFICATION_LAYERPTR_HPP

#include <boost/shared_ptr.hpp>
#include <OpenP2P/Notification/Layer.hpp>

namespace OpenP2P {

	namespace Notification {
	
		template <typename T>
		class LayerPtr: public boost::shared_ptr< Layer<T> > {
			public:
				LayerPtr() { }
				
				template <typename P>
				LayerPtr(P ptr)
					: boost::shared_ptr< Layer<T> >(ptr) { }
					
		};
		
	}
	
}

#endif
