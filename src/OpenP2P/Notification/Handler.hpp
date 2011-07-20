#ifndef OPENP2P_NOTIFICATION_HANDLER_HPP
#define OPENP2P_NOTIFICATION_HANDLER_HPP

namespace OpenP2P{

	namespace Notification{

		class Handler{
			public:
				virtual void notify() = 0;
	
		};
		
	}
	
}

#endif
