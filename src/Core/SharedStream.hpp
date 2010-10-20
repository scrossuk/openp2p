#ifndef OPENP2P_CORE_SHAREDSTREAM_HPP
#define OPENP2P_CORE_SHAREDSTREAM_HPP

#include "StreamListener.hpp"
#include "StreamInitiator.hpp"

namespace openp2p{

	namespace Core{

		class Buffer;
		class Stream;
		class StreamHandler;

		class SharedStream{
			public:
				SharedStream(StreamHandler&);

				~SharedStream();

				void AddContext(Ptr<ContextStream> );

				void RemoveContext(Stream&);

				void OnData(Stream&, Buffer&);

			private:
				StreamHandler& mHandler;

				std::map<Uint32, ContextStream *> mLocalIDMap;
				std::map<ContextStream *, Uint32> mRemoteIDMap;

		};

	}

}

#endif
