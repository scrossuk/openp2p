#include "SharedEndpoint.hpp"

namespace openp2p{

	namespace Core{

		SharedEndpoint::SharedEndpoint(StreamBuilder& builder){
			mSharedStream = new SharedStream(builder);
		}

		void SharedEndpoint::Connect(StreamHandlerFactory& factory){
			mSharedStream->
		}

	}

}


