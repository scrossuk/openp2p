#ifndef OPENP2P_NOTIFICATION_LAYER_HPP
#define OPENP2P_NOTIFICATION_LAYER_HPP

#include <OpenP2P/Notification/Handler.hpp>

namespace OpenP2P{

	namespace Notification{

		template <typename T>
		class Layer{
			public:
				virtual void addHandler(Handler * handler) = 0;
			
				virtual void removeHandler(Handler * handler) = 0;
			
				virtual T getValue() = 0;
	
		};
		
	}
	
}

#endif
