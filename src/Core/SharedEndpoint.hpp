#ifndef OPENP2P_CORE_SHAREDENDPOINT_HPP
#define OPENP2P_CORE_SHAREDENDPOINT_HPP

#include "SharedPtr.hpp"
#include "Endpoint.hpp"

namespace openp2p{

	namespace Core{

		class StreamBuilder;
		class StreamHandlerFactory;

		class SharedEndpoint: public Endpoint{
			public:
				SharedEndpoint(StreamBuilder&, StreamHandlerFactory&);

				void Connect(StreamHandlerFactory&);

			private:
				Ptr<Stream> mSharedStream;

		};

	}

}

#endif
